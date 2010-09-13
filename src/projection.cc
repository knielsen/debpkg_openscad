/*
 *  OpenSCAD (www.openscad.at)
 *  Copyright (C) 2009  Clifford Wolf <clifford@clifford.at>
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

#include "module.h"
#include "node.h"
#include "context.h"
#include "printutils.h"
#include "builtin.h"
#include "dxfdata.h"
#include "dxftess.h"
#include "polyset.h"
#include "export.h"
#include "progress.h"

#ifdef ENABLE_CGAL
#  include <CGAL/assertions_behaviour.h>
#  include <CGAL/exceptions.h>
#endif

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <assert.h>

#include <QApplication>
#include <QTime>
#include <QProgressDialog>

class ProjectionModule : public AbstractModule
{
public:
	ProjectionModule() { }
	virtual AbstractNode *evaluate(const Context *ctx, const ModuleInstantiation *inst) const;
};

class ProjectionNode : public AbstractPolyNode
{
public:
	int convexity;
	bool cut_mode;
	ProjectionNode(const ModuleInstantiation *mi) : AbstractPolyNode(mi) {
		cut_mode = false;
	}
	virtual PolySet *render_polyset(render_mode_e mode) const;
	virtual QString dump(QString indent) const;
};

AbstractNode *ProjectionModule::evaluate(const Context *ctx, const ModuleInstantiation *inst) const
{
	ProjectionNode *node = new ProjectionNode(inst);

	QVector<QString> argnames = QVector<QString>() << "cut";
	QVector<Expression*> argexpr;

	Context c(ctx);
	c.args(argnames, argexpr, inst->argnames, inst->argvalues);

	Value convexity = c.lookup_variable("convexity", true);
	Value cut = c.lookup_variable("cut", true);

	node->convexity = (int)convexity.num;

	if (cut.type == Value::BOOL)
		node->cut_mode = cut.b;

	foreach (ModuleInstantiation *v, inst->children) {
		AbstractNode *n = v->evaluate(inst->ctx);
		if (n)
			node->children.append(n);
	}

	return node;
}

void register_builtin_projection()
{
	builtin_modules["projection"] = new ProjectionModule();
}

#ifdef ENABLE_CGAL

PolySet *ProjectionNode::render_polyset(render_mode_e) const
{
	QString key = mk_cache_id();
	if (PolySet::ps_cache.contains(key)) {
		PRINT(PolySet::ps_cache[key]->msg);
		return PolySet::ps_cache[key]->ps->link();
	}

	print_messages_push();

	PolySet *ps = new PolySet();
	ps->convexity = this->convexity;
	ps->is2d = true;

	CGAL_Nef_polyhedron N;
	N.dim = 3;
	CGAL::Failure_behaviour old_behaviour = CGAL::set_error_behaviour(CGAL::THROW_EXCEPTION);
  try {
	foreach(AbstractNode *v, this->children) {
		if (v->modinst->tag_background)
			continue;
		N.p3 += v->render_cgal_nef_polyhedron().p3;
	}
  }
  catch (CGAL::Assertion_exception e) {
		PRINTF("ERROR: Illegal polygonal object - make sure all polygons are defined with the same winding order. Skipping affected object.");
		CGAL::set_error_behaviour(old_behaviour);
		return ps;
	}
  CGAL::set_error_behaviour(old_behaviour);

	if (cut_mode)
	{
		PolySet *cube = new PolySet();
		double infval = 1e8, eps = 0.1;
		double x1 = -infval, x2 = +infval, y1 = -infval, y2 = +infval, z1 = 0, z2 = eps;

		cube->append_poly(); // top
		cube->append_vertex(x1, y1, z2);
		cube->append_vertex(x2, y1, z2);
		cube->append_vertex(x2, y2, z2);
		cube->append_vertex(x1, y2, z2);

		cube->append_poly(); // bottom
		cube->append_vertex(x1, y2, z1);
		cube->append_vertex(x2, y2, z1);
		cube->append_vertex(x2, y1, z1);
		cube->append_vertex(x1, y1, z1);

		cube->append_poly(); // side1
		cube->append_vertex(x1, y1, z1);
		cube->append_vertex(x2, y1, z1);
		cube->append_vertex(x2, y1, z2);
		cube->append_vertex(x1, y1, z2);

		cube->append_poly(); // side2
		cube->append_vertex(x2, y1, z1);
		cube->append_vertex(x2, y2, z1);
		cube->append_vertex(x2, y2, z2);
		cube->append_vertex(x2, y1, z2);

		cube->append_poly(); // side3
		cube->append_vertex(x2, y2, z1);
		cube->append_vertex(x1, y2, z1);
		cube->append_vertex(x1, y2, z2);
		cube->append_vertex(x2, y2, z2);

		cube->append_poly(); // side4
		cube->append_vertex(x1, y2, z1);
		cube->append_vertex(x1, y1, z1);
		cube->append_vertex(x1, y1, z2);
		cube->append_vertex(x1, y2, z2);
		CGAL_Nef_polyhedron Ncube = cube->render_cgal_nef_polyhedron();
		cube->unlink();

		// N.p3 *= CGAL_Nef_polyhedron3(CGAL_Plane(0, 0, 1, 0), CGAL_Nef_polyhedron3::INCLUDED);
		N.p3 *= Ncube.p3;
		if (!N.p3.is_simple()) {
			PRINTF("WARNING: Body of projection(cut = true) isn't valid 2-manifold! Modify your design..");
			goto cant_project_non_simple_polyhedron;
		}

		PolySet *ps3 = new PolySet();
		cgal_nef3_to_polyset(ps3, &N);
		Grid2d<int> conversion_grid(GRID_COARSE);
		for (int i = 0; i < ps3->polygons.size(); i++) {
			for (int j = 0; j < ps3->polygons[i].size(); j++) {
				double x = ps3->polygons[i][j].x;
				double y = ps3->polygons[i][j].y;
				double z = ps3->polygons[i][j].z;
				if (z != 0)
					goto next_ps3_polygon_cut_mode;
				if (conversion_grid.align(x, y) == i+1)
					goto next_ps3_polygon_cut_mode;
				conversion_grid.data(x, y) = i+1;
			}
			ps->append_poly();
			for (int j = 0; j < ps3->polygons[i].size(); j++) {
				double x = ps3->polygons[i][j].x;
				double y = ps3->polygons[i][j].y;
				conversion_grid.align(x, y);
				ps->insert_vertex(x, y);
			}
		next_ps3_polygon_cut_mode:;
		}
		ps3->unlink();
	}
	else
	{
		if (!N.p3.is_simple()) {
			PRINTF("WARNING: Body of projection(cut = false) isn't valid 2-manifold! Modify your design..");
			goto cant_project_non_simple_polyhedron;
		}

		PolySet *ps3 = new PolySet();
		cgal_nef3_to_polyset(ps3, &N);
		CGAL_Nef_polyhedron np;
		np.dim = 2;
		for (int i = 0; i < ps3->polygons.size(); i++)
		{
			int min_x_p = -1;
			double min_x_val = 0;
			for (int j = 0; j < ps3->polygons[i].size(); j++) {
				double x = ps3->polygons[i][j].x;
				if (min_x_p < 0 || x < min_x_val) {
					min_x_p = j;
					min_x_val = x;
				}
			}
			int min_x_p1 = (min_x_p+1) % ps3->polygons[i].size();
			int min_x_p2 = (min_x_p+ps3->polygons[i].size()-1) % ps3->polygons[i].size();
			double ax = ps3->polygons[i][min_x_p1].x - ps3->polygons[i][min_x_p].x;
			double ay = ps3->polygons[i][min_x_p1].y - ps3->polygons[i][min_x_p].y;
			double at = atan2(ay, ax);
			double bx = ps3->polygons[i][min_x_p2].x - ps3->polygons[i][min_x_p].x;
			double by = ps3->polygons[i][min_x_p2].y - ps3->polygons[i][min_x_p].y;
			double bt = atan2(by, bx);

			double eps = 0.000001;
			if (fabs(at - bt) < eps || (fabs(ax) < eps && fabs(ay) < eps) ||
					(fabs(bx) < eps && fabs(by) < eps)) {
				// this triangle is degenerated in projection
				continue;
			}

			std::list<CGAL_Nef_polyhedron2::Point> plist;
			for (int j = 0; j < ps3->polygons[i].size(); j++) {
				double x = ps3->polygons[i][j].x;
				double y = ps3->polygons[i][j].y;
				CGAL_Nef_polyhedron2::Point p = CGAL_Nef_polyhedron2::Point(x, y);
				if (at > bt)
					plist.push_front(p);
				else
					plist.push_back(p);
			}
			np.p2 += CGAL_Nef_polyhedron2(plist.begin(), plist.end(),
					CGAL_Nef_polyhedron2::INCLUDED);
		}
		DxfData dxf(np);
		dxf_tesselate(ps, &dxf, 0, true, false, 0);
		dxf_border_to_ps(ps, &dxf);
		ps3->unlink();
	}

cant_project_non_simple_polyhedron:
	PolySet::ps_cache.insert(key, new PolySet::ps_cache_entry(ps->link()));
	print_messages_pop();

	return ps;
}

#else // ENABLE_CGAL

PolySet *ProjectionNode::render_polyset(render_mode_e) const
{
	PRINT("WARNING: Found projection() statement but compiled without CGAL support!");
	PolySet *ps = new PolySet();
	ps->is2d = true;
	return ps;
}

#endif // ENABLE_CGAL

QString ProjectionNode::dump(QString indent) const
{
	if (dump_cache.isEmpty()) {
		QString text;
		text.sprintf("projection(cut = %s, convexity = %d) {\n",
				this->cut_mode ? "true" : "false", this->convexity);
		foreach (AbstractNode *v, this->children)
			text += v->dump(indent + QString("\t"));
		text += indent + "}\n";
		((AbstractNode*)this)->dump_cache = indent + QString("n%1: ").arg(idx) + text;
	}
	return dump_cache;
}

