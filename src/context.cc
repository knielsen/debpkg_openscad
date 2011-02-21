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

#include "context.h"
#include "expression.h"
#include "function.h"
#include "module.h"
#include "printutils.h"
#include <QFileInfo>
#include <QDir>

Context::Context(const Context *parent)
{
	this->parent = parent;
	functions_p = NULL;
	modules_p = NULL;
	usedlibs_p = NULL;
	inst_p = NULL;
	if (parent) document_path = parent->document_path;
	ctx_stack.append(this);
}

Context::~Context()
{
	ctx_stack.pop_back();
}

void Context::args(const QVector<QString> &argnames, const QVector<Expression*> &argexpr,
		const QVector<QString> &call_argnames, const QVector<Value> &call_argvalues)
{
	for (int i=0; i<argnames.size(); i++) {
		set_variable(argnames[i], i < argexpr.size() && argexpr[i] ? argexpr[i]->evaluate(this->parent) : Value());
	}

	int posarg = 0;
	for (int i=0; i<call_argnames.size(); i++) {
		if (call_argnames[i].isEmpty()) {
			if (posarg < argnames.size())
				set_variable(argnames[posarg++], call_argvalues[i]);
		} else {
			set_variable(call_argnames[i], call_argvalues[i]);
		}
	}
}

QVector<const Context*> Context::ctx_stack;

void Context::set_variable(QString name, Value value)
{
	if (name.startsWith("$"))
		config_variables[name] = value;
	else
		variables[name] = value;
}

Value Context::lookup_variable(QString name, bool silent) const
{
	if (name.startsWith("$")) {
		for (int i = ctx_stack.size()-1; i >= 0; i--) {
			if (ctx_stack[i]->config_variables.contains(name))
				return ctx_stack[i]->config_variables[name];
		}
		return Value();
	}
	if (variables.contains(name))
		return variables[name];
	if (parent)
		return parent->lookup_variable(name, silent);
	if (!silent)
		PRINTA("WARNING: Ignoring unknown variable '%1'.", name);
	return Value();
}

Value Context::evaluate_function(QString name, const QVector<QString> &argnames, const QVector<Value> &argvalues) const
{
	if (functions_p && functions_p->contains(name))
		return functions_p->value(name)->evaluate(this, argnames, argvalues);
	if (usedlibs_p) {
		QHashIterator<QString, Module*> i(*usedlibs_p);
		while (i.hasNext()) {
			i.next();
			if (i.value()->functions.contains(name)) {
				Module *lib = i.value();
				Context ctx(parent);
				ctx.functions_p = &lib->functions;
				ctx.modules_p = &lib->modules;
				ctx.usedlibs_p = &lib->usedlibs;
				for (int j = 0; j < lib->assignments_var.size(); j++) {
					ctx.set_variable(lib->assignments_var[j], lib->assignments_expr[j]->evaluate(&ctx));
				}
				return i.value()->functions.value(name)->evaluate(&ctx, argnames, argvalues);
			}
		}
	}
	if (parent)
		return parent->evaluate_function(name, argnames, argvalues);
	PRINTA("WARNING: Ignoring unkown function '%1'.", name);
	return Value();
}

AbstractNode *Context::evaluate_module(const ModuleInstantiation *inst) const
{
	if (modules_p && modules_p->contains(inst->modname))
		return modules_p->value(inst->modname)->evaluate(this, inst);
	if (usedlibs_p) {
		QHashIterator<QString, Module*> i(*usedlibs_p);
		while (i.hasNext()) {
			i.next();
			if (i.value()->modules.contains(inst->modname)) {
				Module *lib = i.value();
				Context ctx(parent);
				ctx.functions_p = &lib->functions;
				ctx.modules_p = &lib->modules;
				ctx.usedlibs_p = &lib->usedlibs;
				for (int j = 0; j < lib->assignments_var.size(); j++) {
					ctx.set_variable(lib->assignments_var[j], lib->assignments_expr[j]->evaluate(&ctx));
				}
				return i.value()->modules.value(inst->modname)->evaluate(&ctx, inst);
			}
		}
	}
	if (parent)
		return parent->evaluate_module(inst);
	PRINTA("WARNING: Ignoring unkown module '%1'.", inst->modname);
	return NULL;
}

QString Context::get_absolute_path(const QString &filename) const
{
	return QFileInfo(QDir(this->document_path), filename).absoluteFilePath();
}

