/* Copyright (C) 1998 Jim Hall <jhall1@isd.net>
 * Copyright (C) 2008 Bradley Smith <brad@brad-smith.co.uk>
 *
 * GNU Robots game engine. This is the main() program, using GNU
 * Guile as the backend to handle the language.
 *
 * GNU Robots is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * GNU Robots is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with GNU Robots.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdio.h>
#include <unistd.h>			/* for getopt */
#include <string.h>			/* for strdup */

#include <glib.h>
#include <glib/gprintf.h>

#include <libguile.h>

#include <getopt.h>			/* for GNU getopt_long */

#include "grobot.h"			/* the robot structure, and robot manipulation
								   routines */

#include "api.h"			/* robot API, as Scheme functions */
#include "ui-cmdwin.h"
#include "ui-arena.h"
#include "ui-window.h"
#include "ui-window-private.h"
#include "config.h"
#include "configs.h"
#include "map.h"			/* Game Map */
#include "main.h"			/* for this source file */

#define BUFF_LEN 1024

/* Globals (share with api.c) */
GList *robots = NULL;
GRobot *robot = NULL;		/* The current robot */
UIWindow *ui;
UIArena *arena;
Map *map;

gpointer callback(gpointer data);
SCM catch_handler(void *data, SCM tag, SCM throw_args);
gint is_file_readable(const gchar * filename);

/************************************************************************
 * gint main(gint argc, gchar *argv[])                                  *
 *                                                                      *
 * Entry point                                                          *
 ************************************************************************/
gint main(gint argc, gchar *argv[])
{
	gint opt;				/* the option read from getopt */
	gint flag;				/* flag passed back from getopt - NOT USED */

	gchar maps_path[MAX_PATH], scripts_path[MAX_PATH];

	gchar *main_argv[5] = { "GNU Robots",
		NULL,
		NULL,
		NULL,
		NULL
	};

	struct option long_opts[] = {
		{"version", 0, NULL, 'V'},
		{"help", 0, NULL, 'h'},
		{"map-file", 1, NULL, 'f'},
		{"shields", 1, NULL, 's'},
		{"energy", 1, NULL, 'e'},
		{NULL, 0, NULL, 0}
	};

	/* Initialize the GType system first */
	g_type_init();

	/* Check command line */

	/* Create a robot Object */
	robot = g_robot_new(1, 1, 1, 0, DEFAULT_ENERGY, DEFAULT_SHIELDS, 0, 0,
		NULL, NULL);

	g_assert(robot != NULL);

	/* And add to to the list of robots */
	robots = g_list_append(robots, robot);

	while ((opt = getopt_long(argc, argv, "Vhf:s:e:p:", long_opts,
				&flag)) != EOF)
	{
		switch (opt)
		{
		case 'V':
			/* Display version, then quit */
			g_printf("%s\n\n", PKGINFO);
			g_printf("%s\n\n", COPYRIGHT);
			g_printf
("GNU Robots is free software: you can redistribute it and/or modify\n"
 "it under the terms of the GNU General Public License as published by\n"
 "the Free Software Foundation, either version 3 of the License, or\n"
 "(at your option) any later version.\n");
			g_printf
("\nGNU Robots is distributed in the hope that it will be useful,\n"
 "but WITHOUT ANY WARRANTY; without even the implied warranty of\n"
 "MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the\n"
 "GNU General Public License for more details.\n");
			g_printf
("\nYou should have received a copy of the GNU General Public License\n"
 "along with GNU Robots.  If not, see <http://www.gnu.org/licenses/>.\n");

			exit(0);
			break;

		case 'h':
			/* Display help, then quit. */
			usage(argv[0]);
			exit(0);
			break;

		case 'f':
			/* Set map file */
			main_argv[1] = optarg;	/* pointer assignment */
			break;

		case 's':
			/* Set shields */
			robot->shields = (glong) atol(optarg);
			break;

		case 'e':
			/* Set energy */
			robot->energy = (glong) atol(optarg);
			break;

		default:
			/* invalid option */
			usage(argv[0]);
			exit(1);
			break;
		}						/* switch */
	}							/* while */

	/* Extra arg is the Scheme file */

	if (optind < argc)
	{
		/* Set Scheme file */
		main_argv[2] = argv[optind];	/* pointer assignment */
	}

	/* Check that files have been given */
	if (main_argv[1] == NULL)
	{
		main_argv[1] = g_malloc(MAX_PATH);
		g_strlcpy(main_argv[1], DEFAULT_MAP, MAX_PATH);
		g_fprintf(stderr, "map file not specified, trying default: %s\n",
			main_argv[1]);
	}

	/* Check that files exist */
	g_strlcpy(maps_path, main_argv[1], MAX_PATH);

	if (!is_file_readable(maps_path))
	{
		g_strlcpy(maps_path, MAPS_PATH, MAX_PATH);
		g_strlcat(maps_path, "/", MAX_PATH);
		g_strlcat(maps_path, main_argv[1], MAX_PATH);

		if (!is_file_readable(maps_path))
		{
			gchar *env = getenv(MAPS_PATH_ENV);

			if (env != NULL)
			{
				g_strlcpy(maps_path, env, MAX_PATH);
				g_strlcat(maps_path, "/", MAX_PATH);
				g_strlcat(maps_path, main_argv[1], MAX_PATH);

				if (!is_file_readable(maps_path))
				{
					g_fprintf(stderr,
				"%s: %s: game map file does not exist or is not readable\n",
						argv[0], main_argv[1]);
					exit(1);
				}
			}
			else
			{
				g_fprintf(stderr,
				"%s: %s: game map file does not exist or is not readable\n",
					argv[0], main_argv[1]);
				exit(1);
			}
		}
	}

	main_argv[1] = maps_path;

	/* Now the Scheme file */
	if (main_argv[2] != NULL)
	{
		g_strlcpy(scripts_path, main_argv[2], MAX_PATH);

		if (!is_file_readable(scripts_path))
		{
			g_strlcpy(scripts_path, SCRIPTS_PATH, MAX_PATH);
			g_strlcat(scripts_path, "/", MAX_PATH);
			g_strlcat(scripts_path, main_argv[2], MAX_PATH);

			if (!is_file_readable(scripts_path))
			{
				gchar *env = getenv(SCRIPTS_PATH_ENV);

				if (env != NULL)
				{
					g_strlcpy(scripts_path, env, MAX_PATH);
					g_strlcat(scripts_path, "/", MAX_PATH);
					g_strlcat(scripts_path, main_argv[2], MAX_PATH);

					if (!is_file_readable(scripts_path))
					{
						g_fprintf(stderr,
				"%s: %s: Scheme file does not exist or is not readable\n",
							argv[0], main_argv[2]);
						exit(1);
					}
				}
				else
				{
					g_fprintf(stderr,
				"%s: %s: Scheme file does not exist or is not readable\n",
						argv[0], main_argv[2]);
					exit(1);
				}
			}
		}

		main_argv[2] = scripts_path;
	}
	else
	{
		/* argv[2] can't be NULL as argv[3] may also be NULL */
		main_argv[2] = "";
	}

	/* Start Guile environment.  Does not exit */
	g_printf("%s\n", PKGINFO);
	g_printf("%s\n", COPYRIGHT);
	g_printf("GNU Robots comes with ABSOLUTELY NO WARRANTY\n");
	g_printf
		("This is free software, and you are welcome to redistribute it\n");
	g_printf
		("under certain conditions; see the file `COPYING' for details.\n");
	g_printf("Loading Guile ... Please wait\n\n");

	scm_boot_guile(3, main_argv, main_prog, NULL);

	return 0;				/* never gets here, but keeps compiler happy */
}

/************************************************************************
 * void main_prog(void *closure, gint argc, gchar *argv[])              *
 *                                                                      *
 * The main program code that is executed after Guile starts up. Pass   *
 * the Scheme program as argv[1] and the map file as argv[2]. The       *
 * program name is still argv[0].                                       *
 ************************************************************************/
void main_prog(void *closure, gint argc, gchar *argv[])
{
	gchar *map_file = argv[1];
	gchar *robot_program = argv[2];
	gboolean loading = TRUE;

	api_init();

	g_printf("Map file: %s\n", map_file);

	map = map_new_from_file(map_file, DEFAULT_MAP_ROWS,
		DEFAULT_MAP_COLUMNS);

	if (map == NULL)
	{
		exit_nicely();
	}

	g_thread_init(NULL);
	gdk_threads_init();
	g_thread_create(callback, &loading, FALSE, NULL);

	/* ensure the robot is placed properly */
	MAP_SET_OBJECT(map, G_ROBOT_POSITION_Y(robot),
		G_ROBOT_POSITION_X(robot), ROBOT);

	g_printf("Loading GTK Interface ... Please wait\n\n");
	while(loading);

	/* Now initialize the rest of the Robot properties */
	g_object_set(G_OBJECT(robot),
		"user-interface", G_OBJECT(arena), "map", G_OBJECT(map), NULL);

	g_signal_connect(G_OBJECT(robot), "death", G_CALLBACK(death), NULL);

	if (strlen(robot_program) != 0)
	{
		/* execute a Scheme file */
		g_printf("Robot program: %s\n", robot_program);
		scm_c_primitive_load(robot_program);
	}
	else
	{
		gchar buf[BUFF_LEN];

		g_printf
			("Robot program not specified. Entering interactive mode..\n");

		while (1)
		{
			memset(&buf, 0, BUFF_LEN);

			ui_cmdwin_get_string(UI_CMDWIN(ui->priv->cmdwin), "guile> ",
				buf, BUFF_LEN);

			scm_internal_catch(SCM_BOOL_T,
				(scm_t_catch_body) scm_c_eval_string, buf,
				catch_handler, NULL);
		}
	}

	/* done */
	exit_nicely();
}

gpointer callback(gpointer data)
{
	gtk_init(0, NULL);

	ui = UI_WINDOW(ui_window_new());
	ui_window_postinit(ui, map);

	arena = UI_ARENA(ui->priv->arena);

	/* draw the map */
	ui_arena_draw(arena);
	ui_arena_update_status(arena, "Welcome to GNU Robots", -1, -1, -1);

	*(gboolean*)data = FALSE;

	gdk_threads_enter();
	gtk_main();
	gdk_threads_leave();

	return NULL;
}

/************************************************************************
 * SCM catch_handler (void *data, SCM tag, SCM throw_args);             *
 *                                                                      *
 * Responsible for handling errors                                      *
 ************************************************************************/
SCM catch_handler(void *data, SCM tag, SCM throw_args)
{

/*
  gchar *message = "Couldn't get error message\n";

  if (scm_ilength (throw_args) > 1
      && SCM_NFALSEP (scm_string_p (SCM_CADR (throw_args))))
  {
    message = SCM_STRING_CHARS (SCM_CADR (throw_args));
  }

  else if (SCM_NFALSEP (scm_symbol_p (tag)))
  {
    message = SCM_SYMBOL_CHARS (tag);
  }

  user_interface_update_status (ui, message, -1, -1, -1);
*/

	g_printf("Invalid Instruction\n");

	return SCM_BOOL_F;
}

void death(GRobot *robot)
{
	/* We get a ref increment on a signal */
	g_object_unref(G_OBJECT(robot));

	exit_nicely();
}

/************************************************************************
 * void exit_nicely()                                                   *
 *                                                                      *
 * A function that allows the program to exit nicely, after freeing all *
 * memory pointers, etc.                                                *
 ************************************************************************/
void exit_nicely()
{
	glong score, energy, shields, shots, units;

	/* Stop the UI */
	if (ui != NULL)
	{
		g_object_unref(G_OBJECT(ui));
	}

	/* Get rid of the map object */
	if (map != NULL)
	{
		g_object_unref(G_OBJECT(map));
	}

	/* Show statistics */
	g_object_get(G_OBJECT(robot),
		"shields", &shields,
		"energy", &energy,
		"units", &units, "shots", &shots, "score", &score, NULL);

	g_list_foreach(robots, (GFunc) g_object_unref, NULL);
	g_list_free(robots);

	g_printf(
		"\n-----------------------STATISTICS-----------------------\n");
	g_printf("Shields: %ld\n", (shields < 0 ? 0 : shields));
	g_printf("Energy: %ld\n", (energy < 0 ? 0 : energy));
	g_printf("Units walked: %ld\n", (units < 0 ? 0 : units));
	g_printf("Shots: %ld\n", (shots < 0 ? 0 : shots));
	g_printf("Score: %ld\n", score);

	/* Show results, if any */
	if (shields < 1)
	{
		g_printf("** Robot took too much damage, and died.\n");
	}

	else if (energy < 1)
	{
		g_printf("** Robot ran out of energy.\n");
	}

	/* Quit program */
	exit(0);
}

/************************************************************************
 * void usage(const gchar *argv0)                                       *
 *                                                                      *
 * A function that prints the usage of GNU Robots to the user. Assume   *
 * text mode for this function. We have not initialized X Windows yet.  *
 ************************************************************************/
void usage(const gchar *argv0)
{
	g_printf("%s\n", PKGINFO);
	g_printf("%s\n", COPYRIGHT);
	g_printf
	("Game/diversion where you construct a program for a little robot\n");
	g_printf
	("then set him loose and watch him explore a world on his own.\n\n");

	g_printf("Usage: %s [OPTION]... [FILE]\n\n", argv0);
	g_printf("  -f, --map-file=FILE    Load map file\n");
	g_printf("  -s, --shields=N        Set initial shields to N\n");
	g_printf("  -e, --energy=N         Set initial energy to N\n");
	g_printf
		("  -V, --version          Output version information and exit\n");
	g_printf("  -h, --help             Display this help and exit\n");
	g_printf("\nNote: FILE refers to a scheme file and %s enters into \n",
		argv0);
	g_printf("      interactive mode if it is not specified.\n");

	g_printf("\nReport bugs to <%s>.\n", PACKAGE_BUGREPORT);
}

/************************************************************************
 * gint is_file_readable (const gchar *filename)                        *
 *                                                                      *
 * Checks if a file is a readable file. We will use this function as    *
 * part of a sanity check, before we get anywhere near having to open   *
 * files. This will save on error checking later on, when we may have   *
 * already initialized another environment (Curses, X Windows, ...)     *
 ************************************************************************/
gint is_file_readable(const gchar *filename)
{
	FILE *stream;

	stream = fopen(filename, "r");
	if (stream == NULL)
	{
		/* Failed */
		return (0);
	}

	/* Success */
	fclose(stream);
	return (1);
}
