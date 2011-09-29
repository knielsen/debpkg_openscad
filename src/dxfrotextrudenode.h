#ifndef DXFROTEXTRUDENODE_H_
#define DXFROTEXTRUDENODE_H_

#include "node.h"
#include "visitor.h"

class DxfRotateExtrudeNode : public AbstractPolyNode
{
public:
	DxfRotateExtrudeNode(const ModuleInstantiation *mi) : AbstractPolyNode(mi) {
		convexity = 0;
		fn = fs = fa = 0;
		origin_x = origin_y = scale = 0;
	}
  virtual Response accept(class State &state, Visitor &visitor) const {
		return visitor.visit(state, *this);
	}
	virtual std::string toString() const;
	virtual std::string name() const { return "rotate_extrude"; }

	int convexity;
	double fn, fs, fa;
	double origin_x, origin_y, scale;
	std::string filename, layername;
	virtual PolySet *evaluate_polyset(class PolySetEvaluator *) const;
};

#endif
