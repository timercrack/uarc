/*
 * CFomular.cpp
 *
 *  Created on: 2013-4-27
 *      Author: jeffchen
 */

#include "fomular.h"
#include "exprtk.hpp"

struct CFomular::impl {

	exprtk::parser<double>              parser;
	exprtk::expression<double>			expression;
	exprtk::function_compositor<double>	compositor;
};

CFomular::CFomular() :  _impl(new impl) {}

CFomular::~CFomular() {}

void CFomular::AddConstants()
{
	_impl->compositor.symbol_table().add_constants();
}

void CFomular::AddVariable(const char *x, double& val)
{
	_impl->compositor.symbol_table().add_variable(x, val);
}

void CFomular::AddFunction(const char *name, const char *expr, const char *v0)
{
	_impl->compositor.add(name, expr, v0);
}

void CFomular::AddFunction(const char *name, const char *expr, const char *v0, const char *v1)
{
	_impl->compositor.add(name, expr, v0, v1);
}

void CFomular::AddFunction(const char *name, const char *expr, const char *v0, const char *v1, const char *v2)
{
	_impl->compositor.add(name, expr, v0, v1, v2);
}

void CFomular::AddFunction(const char *name, const char *expr, const char *v0, const char *v1, const char *v2, const char *v3)
{
	_impl->compositor.add(name, expr, v0, v1, v2, v3);
}

void CFomular::AddFunction(const char *name, const char *expr, const char *v0, const char *v1, const char *v2, const char *v3, const char *v4)
{
	_impl->compositor.add(name, expr, v0, v1, v2, v3, v4);
}

void CFomular::AddFunction(const char *name, const char *expr, const char *v0, const char *v1, const char *v2, const char *v3, const char *v4, const char *v5)
{
	_impl->compositor.add(name, expr, v0, v1, v2, v3, v4, v5);
}

int CFomular::Compile(const char* expr)
{
	_impl->expression.register_symbol_table( _impl->compositor.symbol_table() );

	return _impl->parser.compile(expr,_impl->expression);
}

const char *CFomular::CompileError()
{
	return _impl->parser.error().c_str();
}

double CFomular::Value()
{
	return _impl->expression.value();
}

void CFomular::clear()
{
	_impl->expression.release();
	_impl->compositor.symbol_table().clear();
	_impl->compositor.clear();
}
