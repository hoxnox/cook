/*
 *	cook - file construction tool
 *	Copyright (C) 1993, 1994, 1995, 1997, 1998, 1999, 2001 Peter Miller;
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
 * MANIFEST: operating system start point, and parse command line options
 */

#include <ac/stdio.h>
#include <ac/stddef.h>
#include <ac/string.h>
#include <ac/stdlib.h>

#include <arglex.h>
#include <error_intl.h>
#include <help.h>
#include <mem.h>
#include <os_interface.h>
#include <progname.h>
#include <str.h>
#include <version.h>


static void usage _((void));

static void
usage()
{
	char		*progname;

	progname = progname_get();
	fprintf(stderr, "usage: %s [ -L<path>... ] -l<name>...\n", progname);
	fprintf(stderr, "       %s -Help\n", progname);
	fprintf(stderr, "       %s -VERSion\n", progname);
	exit(1);
}


enum
{
	arglex_token_path,
	arglex_token_search
};

static arglex_table_ty argtab[] =
{
	{ "-\\L*",	(arglex_token_ty)arglex_token_path,	},
	{ "-\\L",	(arglex_token_ty)arglex_token_path,	},
	{ "-\\l*",	(arglex_token_ty)arglex_token_search,	},
	{ "-\\l",	(arglex_token_ty)arglex_token_search,	},
	{ 0, (arglex_token_ty)0, }, /* end marker */
};


static char *copy_string _((char *));

static char *
copy_string(s)
	char	*s;
{
	char	*cp;

	cp = mem_alloc(strlen(s) + 1);
	strcpy(cp, s);
	return cp;
}


static void get_sys_libs _((int *, char **));

static void
get_sys_libs(nlibs, lib)
	int	*nlibs;
	char	**lib;
{
	char	*cp;
	char	*start;

	cp = getenv("LD_LIBRARY_PATH");
	if (!cp)
	{
		lib[0] = "/usr/lib";
		lib[1] = "/lib";
		*nlibs = 2;
		return;
	}
	*nlibs = 0;
	start = cp;
	for (;;)
	{
		if (!*cp || *cp == ':')
		{
			char	c;

			c = *cp;
			*cp = 0;
			if (start < cp)
				lib[(*nlibs)++] = copy_string(start);
			*cp = c;
			if (!c)
				break;
			start = cp + 1;
		}
		cp++;
	}
}


int main _((int, char **));

int
main(argc, argv)
	int	argc;
	char	**argv;
{
	char	*path[1000];
	int	npaths = 0;
	int	k;
	char	pathname[2000];
	char	*syspath[100];
	int	nsyspaths;

	arglex_init(argc, argv, argtab);
	str_initialize();

	get_sys_libs(&nsyspaths, syspath);

	switch (arglex())
	{
	case arglex_token_help:
		help((char *)0, usage);
		exit(0);

	case arglex_token_version:
		version();
		exit(0);

	default:
		break;
	}

	while (arglex_token != arglex_token_eoln)
	{
		switch (arglex_token)
		{
		default:
			generic_argument(usage);
			continue;

		case arglex_token_search:
			if (arglex() != arglex_token_string)
			{
				arg_needs_string(arglex_token_search, usage);
				/* NOTREACHED */
			}
			for (k = 0; k < nsyspaths; ++k)
				path[npaths++] = syspath[k];
			for (k = 0; k < npaths; ++k)
			{
				sprintf
				(
					pathname,
					"%s/lib%s.a",
					path[k],
					arglex_value.alv_string
				);
				if (os_exists(pathname))
				{
					printf("%s\n", pathname);
					break;
				}
				sprintf
				(
					pathname,
					"%s/lib%s.so",
					path[k],
					arglex_value.alv_string
				);
				if (os_exists(pathname))
				{
					printf("%s\n", pathname);
					break;
				}
			}
			if (k >= npaths)
			{
				sub_context_ty	*scp;

				scp = sub_context_new();
				sub_var_set
				(
					scp,
					"Name",
					"%s",
					arglex_value.alv_string
				);
				fatal_intl
				(
					scp,
					i18n("library \"$name\" not found")
				);
			}
			npaths -= nsyspaths;
			break;

		case arglex_token_path:
			if (arglex() != arglex_token_string)
			{
				arg_needs_string(arglex_token_path, usage);
				/* NOTREACHED */
			}
			path[npaths++] = arglex_value.alv_string;
			break;
		}
		arglex();
	}
	exit(0);
	return 0;
}
