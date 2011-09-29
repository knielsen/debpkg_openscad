#ifndef POLYSETEVALUATOR_H_
#define POLYSETEVALUATOR_H_

#include "node.h"
#include "Tree.h"
#include "memory.h"

class PolySetEvaluator
{
public:
	PolySetEvaluator(const Tree &tree) : tree(tree) {}
	virtual ~PolySetEvaluator() {}

	const Tree &getTree() const { return this->tree; }

	virtual shared_ptr<PolySet> getPolySet(const class AbstractNode &, bool cache);

	virtual PolySet *evaluatePolySet(const class ProjectionNode &) { return NULL; }
	virtual PolySet *evaluatePolySet(const class DxfLinearExtrudeNode &) { return NULL; }
	virtual PolySet *evaluatePolySet(const class DxfRotateExtrudeNode &) { return NULL; }
	virtual PolySet *evaluatePolySet(const class CgaladvNode &) { return NULL; }
	virtual PolySet *evaluatePolySet(const class RenderNode &) { return NULL; }

private:
	const Tree &tree;
};

#endif
