#ifndef MODULE_H_
#define MODULE_H_

#include <QString>
#include <QVector>
#include <QHash>
#include "value.h"

class ModuleInstantiation
{
public:
	QString label;
	QString modname;
	QVector<QString> argnames;
	QVector<class Expression*> argexpr;
	QVector<Value> argvalues;
	QVector<ModuleInstantiation*> children;

	bool tag_root;
	bool tag_highlight;
	bool tag_background;
	const class Context *ctx;

	ModuleInstantiation() : tag_root(false), tag_highlight(false), tag_background(false), ctx(NULL) { }
	virtual ~ModuleInstantiation();

	QString dump(QString indent) const;
	class AbstractNode *evaluate(const Context *ctx) const;
};

class IfElseModuleInstantiation : public ModuleInstantiation {
public:
	virtual ~IfElseModuleInstantiation();

	QVector<ModuleInstantiation*> else_children;
};

class AbstractModule
{
public:
	virtual ~AbstractModule();
	virtual class AbstractNode *evaluate(const Context *ctx, const ModuleInstantiation *inst) const;
	virtual QString dump(QString indent, QString name) const;
};

class Module : public AbstractModule
{
public:
	QHash< QString, Module*> usedlibs;

	struct libs_cache_ent {
		Module *mod;
		QString cache_id, msg;
	};
	static QHash<QString, libs_cache_ent> libs_cache;
	static Module *compile_library(QString filename);

	QVector<QString> argnames;
	QVector<Expression*> argexpr;

	QVector<QString> assignments_var;
	QVector<Expression*> assignments_expr;

	QHash<QString, class AbstractFunction*> functions;
	QHash<QString, AbstractModule*> modules;

	QVector<ModuleInstantiation*> children;

	Module() { }
	virtual ~Module();

	virtual AbstractNode *evaluate(const Context *ctx, const ModuleInstantiation *inst) const;
	virtual QString dump(QString indent, QString name) const;
};

#endif
