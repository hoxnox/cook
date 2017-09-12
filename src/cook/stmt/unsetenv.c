/*
 *	cook - file construction tool
 *	Copyright (C) 1997 Peter Miller;
 *	All rights reserved.
 *
 *	This program is free software; you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation; either version 2 of the License, or
 *	(at your option) any later version.
 *
 *	This program is distributed in the hope that it will be useful,
 *	but WITHOUT ANY WARRANTY; without even the implied warranty of
 *	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *	GNU General Public License for more details.
 *
 *	You should have received a copy of the GNU General Public License
 *	along with this program; if not, write to the Free Software
 *	Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111, USA.
 *
 * MANIFEST: functions to manipulate unsetenv statement nodes
 */

#include <expr/list.h>
#include <expr/position.h>
#include <opcode/list.h>
#include <opcode/push.h>
#include <opcode/unsetenv.h>
#include <stmt.h>
#include <stmt/unsetenv.h>
#include <trace.h>


typedef struct stmt_unsetenv_ty stmt_unsetenv_ty;
struct stmt_unsetenv_ty
{
	stmt_ty		inherited;
	expr_list_ty	args;
};


/*
 *  NAME
 *	destructor - free a unsetenv statement node
 *
 *  SYNOPSIS
 *	void destructor(stmt_ty *sp);
 *
 *  DESCRIPTION
 *	The destructor function releases the resources held by a unsetenv
 *	statement instance after it is finished with.
 *
 *  CAVEAT
 *	Do not free the node itself, this the the destructor, not
 *	delete.
 */

static void destructor _((stmt_ty *));

static void
destructor(sp)
	stmt_ty		*sp;
{
	stmt_unsetenv_ty *this;

	trace(("destructor(sp = %08X)\n{\n"/*}*/, sp));
	/* assert(sp); */
	/* assert(sp->method == &method); */
	this = (stmt_unsetenv_ty *)sp;

	expr_list_destructor(&this->args);

	trace((/*{*/"}\n"));
}


/*
 * NAME
 *	code_generate
 *
 * SYNOPSIS
 *	stmt_result_ty code_generate(stmt_ty *sp, opcode_list_ty *olp);
 *
 * DESCRIPTION
 *	The code_generate function is used to generate the opcodes for
 *	this statement node.
 *
 * RETURNS
 *	The value returned indicates why the code generation terminated.
 */

static stmt_result_ty code_generate _((stmt_ty *, opcode_list_ty *));

static stmt_result_ty
code_generate(sp, olp)
	stmt_ty		*sp;
	opcode_list_ty	*olp;
{
	stmt_unsetenv_ty *this;
	expr_position_ty *pp;

	trace(("code_generate(sp = %08X)\n{\n"/*}*/, sp));
	assert(sp);
	this = (stmt_unsetenv_ty *)sp;

	opcode_list_append(olp, opcode_push_new());
	expr_list_code_generate(&this->args, olp);
	pp = expr_list_position(&this->args);
	opcode_list_append(olp, opcode_unsetenv_new(pp));
	trace((/*{*/"}\n"));
	return STMT_OK;
}


/*
 * NAME
 *	method - class method table
 *
 * DESCRIPTION
 *	This is the class method table.  It contains a description of
 *	the class, its name, size and pointers to its virtual methods.
 *
 * CAVEAT
 *	This symbol is NOT to be exported from this file scope.
 */

static stmt_method_ty method =
{
	"unsetenv",
	sizeof(stmt_unsetenv_ty),
	destructor,
	code_generate,
};


/*
 * NAME
 *	stmt_unsetenv_new - create a new unsetenv statement node
 *
 * SYNOPSIS
 *	stmt_ty *stmt_unsetenv_new(string_ty *);
 *
 * DESCRIPTION
 *	The stmt_unsetenv_new function is used to create a new instance
 *	of a unsetenv statement node.
 *
 * RETURNS
 *	stmt_ty *; pointer to polymorphic statement instance.
 *
 * CAVEAT
 *	This function allocates data in dynamic memory.  It is the
 *	caller's responsibility to free this data, using stmt_delete,
 *	when it is no longer required.
 */

stmt_ty *
stmt_unsetenv_new(args)
	expr_list_ty	*args;
{
	stmt_ty		*sp;
	stmt_unsetenv_ty	*this;

	trace(("stmt_unsetenv_new()\n{\n"/*}*/));
	sp = stmt_private_new(&method);
	this = (stmt_unsetenv_ty *)sp;

	expr_list_copy_constructor(&this->args, args);

	trace(("return %8.8lX;\n", (long)sp));
	trace((/*{*/"}\n"));
	return sp;
}
