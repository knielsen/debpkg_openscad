// csg test core, used by throwntegether test and opencsg test
#include "csgtestcore.h"

#include "system-gl.h"
#include "openscad.h"
#include "handle_dep.h"
#include "builtin.h"
#include "context.h"
#include "node.h"
#include "module.h"
#include "polyset.h"
#include "Tree.h"
#include "CSGTermEvaluator.h"
#include "CGALEvaluator.h"
#include "PolySetCGALEvaluator.h"

#include "OpenCSGRenderer.h"
#include "ThrownTogetherRenderer.h"

#include "csgterm.h"
#include "OffscreenView.h"

#include <QApplication>
#include <QFile>
#include <QDir>
#include <QSet>
#include <QTimer>
#include <sstream>

using std::cerr;
using std::cout;

std::string commandline_commands;
QString librarydir;

//#define DEBUG

class CsgInfo
{
public:
	CsgInfo();
	CSGTerm *root_norm_term;          // Normalized CSG products
	class CSGChain *root_chain;
	std::vector<CSGTerm*> highlight_terms;
	CSGChain *highlights_chain;
	std::vector<CSGTerm*> background_terms;
	CSGChain *background_chain;
	OffscreenView *glview;
};

CsgInfo::CsgInfo() {
        root_norm_term = NULL;
        root_chain = NULL;
        highlight_terms = std::vector<CSGTerm*>();
        highlights_chain = NULL;
        background_terms = std::vector<CSGTerm*>();
        background_chain = NULL;
        glview = NULL;
}

AbstractNode *find_root_tag(AbstractNode *n)
{
	foreach(AbstractNode *v, n->children) {
		if (v->modinst->tag_root) return v;
		if (AbstractNode *vroot = find_root_tag(v)) return vroot;
	}
	return NULL;
}

int csgtestcore(int argc, char *argv[], test_type_e test_type)
{
	if (argc != 3) {
		fprintf(stderr, "Usage: %s <file.scad> <output.png>\n", argv[0]);
		exit(1);
	}

	std::string filename(argv[1]);
	std::string outfile(argv[2]);

	initialize_builtin_functions();
	initialize_builtin_modules();

	QApplication app(argc, argv, false);

	QDir original_path = QDir::current();

	QString currentdir = QDir::currentPath();

	QDir libdir(QApplication::instance()->applicationDirPath());
#ifdef Q_WS_MAC
	libdir.cd("../Resources"); // Libraries can be bundled
	if (!libdir.exists("libraries")) libdir.cd("../../..");
#elif defined(Q_OS_UNIX)
	if (libdir.cd("../share/openscad/libraries")) {
		librarydir = libdir.path();
	} else
	if (libdir.cd("../../share/openscad/libraries")) {
		librarydir = libdir.path();
	} else
	if (libdir.cd("../../libraries")) {
		librarydir = libdir.path();
	} else
#endif
	if (libdir.cd("libraries")) {
		librarydir = libdir.path();
	}

	Context root_ctx;
	root_ctx.functions_p = &builtin_functions;
	root_ctx.modules_p = &builtin_modules;
	root_ctx.set_variable("$fn", Value(0.0));
	root_ctx.set_variable("$fs", Value(1.0));
	root_ctx.set_variable("$fa", Value(12.0));
	root_ctx.set_variable("$t", Value(0.0));

	Value zero3;
	zero3.type = Value::VECTOR;
	zero3.append(new Value(0.0));
	zero3.append(new Value(0.0));
	zero3.append(new Value(0.0));
	root_ctx.set_variable("$vpt", zero3);
	root_ctx.set_variable("$vpr", zero3);


	AbstractModule *root_module;
	ModuleInstantiation root_inst;

	QFileInfo fileInfo(filename.c_str());
	handle_dep(filename);
	FILE *fp = fopen(filename.c_str(), "rt");
	if (!fp) {
		fprintf(stderr, "Can't open input file `%s'!\n", filename.c_str());
		exit(1);
	} else {
		std::stringstream text;
		char buffer[513];
		int ret;
		while ((ret = fread(buffer, 1, 512, fp)) > 0) {
			buffer[ret] = 0;
			text << buffer;
		}
		fclose(fp);
		text << commandline_commands;
		root_module = parse(text.str().c_str(), fileInfo.absolutePath().toLocal8Bit(), false);
		if (!root_module) {
			exit(1);
		}
	}

	QDir::setCurrent(fileInfo.absolutePath());

	AbstractNode::resetIndexCounter();
	AbstractNode *absolute_root_node = root_module->evaluate(&root_ctx, &root_inst);
	AbstractNode *root_node;
	// Do we have an explicit root node (! modifier)?
	if (!(root_node = find_root_tag(absolute_root_node))) root_node = absolute_root_node;

	Tree tree(root_node);

	CsgInfo csgInfo = CsgInfo();
	CGALEvaluator cgalevaluator(tree);
	CSGTermEvaluator evaluator(tree, &cgalevaluator.psevaluator);
	CSGTerm *root_raw_term = evaluator.evaluateCSGTerm(*root_node, 
																										 csgInfo.highlight_terms, 
																										 csgInfo.background_terms);

	if (!root_raw_term) {
		cerr << "Error: CSG generation failed! (no top level object found)\n";
		return 1;
	}

	// CSG normalization
	csgInfo.root_norm_term = root_raw_term->link();
	while (1) {
		CSGTerm *n = csgInfo.root_norm_term->normalize();
		csgInfo.root_norm_term->unlink();
		if (csgInfo.root_norm_term == n)
			break;
		csgInfo.root_norm_term = n;
	}
		
	assert(csgInfo.root_norm_term);
	
	csgInfo.root_chain = new CSGChain();
	csgInfo.root_chain->import(csgInfo.root_norm_term);
	fprintf(stderr, "Normalized CSG tree has %d elements\n", csgInfo.root_chain->polysets.size());
	
	if (csgInfo.highlight_terms.size() > 0) {
		cerr << "Compiling highlights (" << csgInfo.highlight_terms.size() << " CSG Trees)...\n";
		
		csgInfo.highlights_chain = new CSGChain();
		for (unsigned int i = 0; i < csgInfo.highlight_terms.size(); i++) {
			while (1) {
				CSGTerm *n = csgInfo.highlight_terms[i]->normalize();
				csgInfo.highlight_terms[i]->unlink();
				if (csgInfo.highlight_terms[i] == n)
					break;
				csgInfo.highlight_terms[i] = n;
			}
			csgInfo.highlights_chain->import(csgInfo.highlight_terms[i]);
		}
	}
	
	if (csgInfo.background_terms.size() > 0) {
		cerr << "Compiling background (" << csgInfo.background_terms.size() << " CSG Trees)...\n";
		
		csgInfo.background_chain = new CSGChain();
		for (unsigned int i = 0; i < csgInfo.background_terms.size(); i++) {
			while (1) {
				CSGTerm *n = csgInfo.background_terms[i]->normalize();
				csgInfo.background_terms[i]->unlink();
				if (csgInfo.background_terms[i] == n)
					break;
				csgInfo.background_terms[i] = n;
			}
			csgInfo.background_chain->import(csgInfo.background_terms[i]);
		}
	}
	
	QDir::setCurrent(original_path.absolutePath());

	try {
		csgInfo.glview = new OffscreenView(512,512);
	} catch (int error) {
		fprintf(stderr,"Can't create OpenGL OffscreenView. Code: %i. Exiting.\n", error);
		exit(1);
	}
	BoundingBox bbox = csgInfo.root_chain->getBoundingBox();

	Vector3d center = (bbox.min() + bbox.max()) / 2;
	double radius = (bbox.max() - bbox.min()).norm() / 2;


	Vector3d cameradir(1, 1, -0.5);
	Vector3d camerapos = center - radius*1.8*cameradir;
	csgInfo.glview->setCamera(camerapos, center);

	OpenCSGRenderer opencsgRenderer(csgInfo.root_chain, csgInfo.highlights_chain, csgInfo.background_chain, csgInfo.glview->shaderinfo);
	ThrownTogetherRenderer thrownTogetherRenderer(csgInfo.root_chain, csgInfo.highlights_chain, csgInfo.background_chain);

	if (test_type == TEST_THROWNTOGETHER)
		csgInfo.glview->setRenderer(&thrownTogetherRenderer);
	else
		csgInfo.glview->setRenderer(&opencsgRenderer);

	csgInfo.glview->paintGL();

	csgInfo.glview->save(outfile.c_str());
	
	destroy_builtin_functions();
	destroy_builtin_modules();

	return 0;
}
