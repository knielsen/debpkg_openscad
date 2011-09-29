/*
 *  OpenSCAD (www.openscad.org)
 *  Copyright (C) 2009-2011 Clifford Wolf <clifford@clifford.at> and
 *                          Marius Kintel <marius@kintel.net>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  As a special exception, you have permission to link this program
 *  with the CGAL library and distribute executables, as long as you
 *  follow the requirements of the GNU GPL in regard to all of the
 *  software in the executable aside from CGAL.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

#include "csgterm.h"
#include "polyset.h"
#include <sstream>

/*!
	\class CSGTerm

	A CSGTerm is either a "primitive" or a CSG operation with two
	children terms. A primitive in this context is any PolySet, which
	may or may not have a subtree which is already evaluated (e.g. using
	the render() module).

 */

/*!
	\class CSGChain

	A CSGChain is just a vector of primitives, each having a CSG type associated with it.
	It's created by importing a CSGTerm tree.

 */


CSGTerm::CSGTerm(const shared_ptr<PolySet> &polyset, const double matrix[16], const double color[4], const std::string &label)
	: type(TYPE_PRIMITIVE), polyset(polyset), label(label), left(NULL), right(NULL)
{
	for (int i = 0; i < 16; i++) this->m[i] = matrix[i];
	for (int i = 0; i < 4; i++) this->color[i] = color[i];
	refcounter = 1;
}

CSGTerm::CSGTerm(type_e type, CSGTerm *left, CSGTerm *right)
	: type(type), left(left), right(right)
{
	refcounter = 1;
}

CSGTerm *CSGTerm::normalize()
{
	// This function implements the CSG normalization
	// Reference: Florian Kirsch, Juergen Doeller,
	// OpenCSG: A Library for Image-Based CSG Rendering,
	// University of Potsdam, Hasso-Plattner-Institute, Germany
	// http://www.opencsg.org/data/csg_freenix2005_paper.pdf

	if (type == TYPE_PRIMITIVE)
		return link();

	CSGTerm *t1, *t2, *x, *y;

	x = left->normalize();
	y = right->normalize();

	if (x != left || y != right) {
		t1 = new CSGTerm(type, x, y);
	} else {
		t1 = link();
		x->unlink();
		y->unlink();
	}

	while (1) {
		t2 = t1->normalize_tail();
		t1->unlink();
		if (t1 == t2)
			break;
		t1 = t2;
	}

	return t1;
}

CSGTerm *CSGTerm::normalize_tail()
{
	CSGTerm *x, *y, *z;

	// Part A: The 'x . (y . z)' expressions

	x = left;
	y = right->left;
	z = right->right;

	// 1.  x - (y + z) -> (x - y) - z
	if (type == TYPE_DIFFERENCE && right->type == TYPE_UNION)
		return new CSGTerm(TYPE_DIFFERENCE, new CSGTerm(TYPE_DIFFERENCE, x->link(), y->link()), z->link());

	// 2.  x * (y + z) -> (x * y) + (x * z)
	if (type == TYPE_INTERSECTION && right->type == TYPE_UNION)
		return new CSGTerm(TYPE_UNION, new CSGTerm(TYPE_INTERSECTION, x->link(), y->link()), new CSGTerm(TYPE_INTERSECTION, x->link(), z->link()));

	// 3.  x - (y * z) -> (x - y) + (x - z)
	if (type == TYPE_DIFFERENCE && right->type == TYPE_INTERSECTION)
		return new CSGTerm(TYPE_UNION, new CSGTerm(TYPE_DIFFERENCE, x->link(), y->link()), new CSGTerm(TYPE_DIFFERENCE, x->link(), z->link()));

	// 4.  x * (y * z) -> (x * y) * z
	if (type == TYPE_INTERSECTION && right->type == TYPE_INTERSECTION)
		return new CSGTerm(TYPE_INTERSECTION, new CSGTerm(TYPE_INTERSECTION, x->link(), y->link()), z->link());

	// 5.  x - (y - z) -> (x - y) + (x * z)
	if (type == TYPE_DIFFERENCE && right->type == TYPE_DIFFERENCE)
		return new CSGTerm(TYPE_UNION, new CSGTerm(TYPE_DIFFERENCE, x->link(), y->link()), new CSGTerm(TYPE_INTERSECTION, x->link(), z->link()));

	// 6.  x * (y - z) -> (x * y) - z
	if (type == TYPE_INTERSECTION && right->type == TYPE_DIFFERENCE)
		return new CSGTerm(TYPE_DIFFERENCE, new CSGTerm(TYPE_INTERSECTION, x->link(), y->link()), z->link());

	// Part B: The '(x . y) . z' expressions

	x = left->left;
	y = left->right;
	z = right;

	// 7. (x - y) * z  -> (x * z) - y
	if (left->type == TYPE_DIFFERENCE && type == TYPE_INTERSECTION)
		return new CSGTerm(TYPE_DIFFERENCE, new CSGTerm(TYPE_INTERSECTION, x->link(), z->link()), y->link());

	// 8. (x + y) - z  -> (x - z) + (y - z)
	if (left->type == TYPE_UNION && type == TYPE_DIFFERENCE)
		return new CSGTerm(TYPE_UNION, new CSGTerm(TYPE_DIFFERENCE, x->link(), z->link()), new CSGTerm(TYPE_DIFFERENCE, y->link(), z->link()));

	// 9. (x + y) * z  -> (x * z) + (y * z)
	if (left->type == TYPE_UNION && type == TYPE_INTERSECTION)
		return new CSGTerm(TYPE_UNION, new CSGTerm(TYPE_INTERSECTION, x->link(), z->link()), new CSGTerm(TYPE_INTERSECTION, y->link(), z->link()));

	return link();
}

CSGTerm *CSGTerm::link()
{
	refcounter++;
	return this;
}

void CSGTerm::unlink()
{
	if (--refcounter <= 0) {
		if (left)
			left->unlink();
		if (right)
			right->unlink();
		delete this;
	}
}

std::string CSGTerm::dump()
{
	std::stringstream dump;

	if (type == TYPE_UNION)
		dump << "(" << left->dump() << " + " << right->dump() << ")";
	else if (type == TYPE_INTERSECTION)
		dump << "(" << left->dump() << " * " << right->dump() << ")";
	else if (type == TYPE_DIFFERENCE)
		dump << "(" << left->dump() << " - " << right->dump() << ")";
	else 
		dump << this->label;

	return dump.str();
}

CSGChain::CSGChain()
{
}

void CSGChain::add(const shared_ptr<PolySet> &polyset, double *m, double *color, CSGTerm::type_e type, std::string label)
{
	polysets.push_back(polyset);
	matrices.push_back(m);
	colors.push_back(color);
	types.push_back(type);
	labels.push_back(label);
}

void CSGChain::import(CSGTerm *term, CSGTerm::type_e type)
{
	if (term->type == CSGTerm::TYPE_PRIMITIVE) {
		add(term->polyset, term->m, term->color, type, term->label);
	} else {
		import(term->left, type);
		import(term->right, term->type);
	}
}

std::string CSGChain::dump()
{
	std::stringstream dump;

	for (size_t i = 0; i < types.size(); i++)
	{
		if (types[i] == CSGTerm::TYPE_UNION) {
			if (i != 0) dump << "\n";
			dump << "+";
		}
		else if (types[i] == CSGTerm::TYPE_DIFFERENCE)
			dump << " -";
		else if (types[i] == CSGTerm::TYPE_INTERSECTION)
			dump << " *";
		dump << labels[i];
	}
	dump << "\n";
	return dump.str();
}

BoundingBox CSGChain::getBoundingBox() const
{
	BoundingBox bbox;
	for (size_t i=0;i<polysets.size();i++) {
		if (types[i] != CSGTerm::TYPE_DIFFERENCE) {
			BoundingBox psbox = polysets[i]->getBoundingBox();
			if (!psbox.isNull()) {
				Eigen::Transform3d t;
				// Column-major vs. Row-major
				t.matrix()	<< 
					matrices[i][0], matrices[i][4], matrices[i][8], matrices[i][12],
					matrices[i][1], matrices[i][5], matrices[i][9], matrices[i][13],
					matrices[i][2], matrices[i][6], matrices[i][10], matrices[i][14],
					matrices[i][3], matrices[i][7], matrices[i][11], matrices[i][15];
				bbox.extend(t * psbox.min());
				bbox.extend(t * psbox.max());
			}
		}
	}
	return bbox;
}
