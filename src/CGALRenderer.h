#ifndef CGALRENDERER_H_
#define CGALRENDERER_H_

#include "renderer.h"
#include "CGAL_Nef_polyhedron.h"

class CGALRenderer : public Renderer
{
public:
	CGALRenderer(const CGAL_Nef_polyhedron &root);
	~CGALRenderer();
	void draw(bool showfaces, bool showedges) const;

public:
	const CGAL_Nef_polyhedron &root;
	class Polyhedron *polyhedron;
	class PolySet *polyset;
};

#endif
