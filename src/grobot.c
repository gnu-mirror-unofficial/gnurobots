/* Copyright (C) 1998 Jim Hall <jhall1@isd.net>
 * Copyright (C) 2008 Bradley Smith <brad@brad-smith.co.uk>
 *
 * Robot object for the GNU Robots game
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

#include "grobot.h"
#include "configs.h"
#include <stdio.h>
#include <glib.h>
#include <string.h>
#include <stdlib.h>

G_DEFINE_TYPE(GRobot, g_robot, G_TYPE_OBJECT)

enum
{
	DEATH,
	LAST_SIGNAL
};

enum
{
	ARG_0,
	ARG_POS_X,
	ARG_POS_Y,
	ARG_DIRECTION,
	ARG_SCORE,
	ARG_ENERGY,
	ARG_SHIELDS,
	ARG_UNITS,
	ARG_SHOTS,
	ARG_USER_INTERFACE,
	ARG_MAP
};

static gchar *things[] = { "space", "food", "prize", "wall", "baddie",
	"robot"
};
static gint cthings[] = { SPACE, FOOD, PRIZE, WALL, BADDIE, ROBOT };

static guint g_robot_signals[LAST_SIGNAL] = { 0 };

static void g_robot_dispose(GObject *object);
static void g_robot_finalize(GObject *object);

static void g_robot_set_property(GObject *object, guint prop_id,
	const GValue *value, GParamSpec *pspec);

static void g_robot_get_property(GObject *object, guint prop_id,
	GValue *value, GParamSpec * pspec);

static GObjectClass *parent_class = NULL;

static gint what_thing(const gchar *th);

static void g_robot_init(GRobot *robot)
{
	/* Nothing yet, need to do priv stuff */
}

static void g_robot_class_init(GRobotClass *klass)
{
	GObjectClass *gobject_class;

	gobject_class = (GObjectClass *) klass;

	parent_class = g_type_class_ref(G_TYPE_OBJECT);

	gobject_class->dispose = g_robot_dispose;
	gobject_class->finalize = g_robot_finalize;
	gobject_class->set_property = g_robot_set_property;
	gobject_class->get_property = g_robot_get_property;

	g_object_class_install_property(G_OBJECT_CLASS(klass), ARG_POS_X,
		g_param_spec_int("x",
			"x",
			"X co-ordinate of current Position of the Robot",
			G_MININT, G_MAXINT, 0, G_PARAM_READWRITE | G_PARAM_CONSTRUCT));

	g_object_class_install_property(G_OBJECT_CLASS(klass), ARG_POS_Y,
		g_param_spec_int("y",
			"y",
			"y co-ordinate of current Position of the Robot",
			G_MININT, G_MAXINT, 0, G_PARAM_READWRITE | G_PARAM_CONSTRUCT));

	g_object_class_install_property(G_OBJECT_CLASS(klass), ARG_DIRECTION,
		g_param_spec_int("direction",
			"direction",
			"current Direction of the Robot",
			G_MININT, G_MAXINT, 0, G_PARAM_READWRITE | G_PARAM_CONSTRUCT));

	g_object_class_install_property(G_OBJECT_CLASS(klass), ARG_SCORE,
		g_param_spec_long("score",
			"Score",
			"current Score of the Robot",
			G_MINLONG, G_MAXLONG, 0,
			G_PARAM_READWRITE | G_PARAM_CONSTRUCT));

	g_object_class_install_property(G_OBJECT_CLASS(klass), ARG_ENERGY,
		g_param_spec_long("energy",
			"Energy",
			"current Energy-level of the Robot",
			G_MINLONG, G_MAXLONG, 0,
			G_PARAM_READWRITE | G_PARAM_CONSTRUCT));

	g_object_class_install_property(G_OBJECT_CLASS(klass), ARG_SHIELDS,
		g_param_spec_long("shields",
			"Shields",
			"current Shield-level of the Robot",
			G_MINLONG, G_MAXLONG, 0,
			G_PARAM_READWRITE | G_PARAM_CONSTRUCT));

	g_object_class_install_property(G_OBJECT_CLASS(klass), ARG_UNITS,
		g_param_spec_long("units",
			"Units",
			"Units walked by the Robot so far",
			G_MINLONG, G_MAXLONG, 0,
			G_PARAM_READWRITE | G_PARAM_CONSTRUCT));

	g_object_class_install_property(G_OBJECT_CLASS(klass), ARG_SHOTS,
		g_param_spec_long("shots",
			"Shots",
			"Number of Shots fired by the Robot",
			G_MINLONG, G_MAXLONG, 0,
			G_PARAM_READWRITE | G_PARAM_CONSTRUCT));

	g_object_class_install_property(G_OBJECT_CLASS(klass),
		ARG_USER_INTERFACE,
		g_param_spec_object("user-interface",
			"UserInterface",
			"Reference to the UI object",
			G_TYPE_OBJECT, G_PARAM_READWRITE | G_PARAM_CONSTRUCT));

	g_object_class_install_property(G_OBJECT_CLASS(klass), ARG_MAP,
		g_param_spec_object("map",
			"Map",
			"Reference to the Game Map object",
			G_TYPE_OBJECT, G_PARAM_READWRITE | G_PARAM_CONSTRUCT));

	g_robot_signals[DEATH] =
		g_signal_new("death",
		G_TYPE_FROM_CLASS(klass),
		G_SIGNAL_RUN_LAST,
		G_STRUCT_OFFSET(GRobotClass, death),
		NULL, NULL, g_cclosure_marshal_VOID__VOID, G_TYPE_NONE, 0, NULL);
}

static void
g_robot_set_property(GObject *object, guint prop_id, const GValue *value,
	GParamSpec *pspec)
{
	GRobot *robot;
	GObject *obj;

	/* it's not null if we got it, but it might not be ours */
	g_return_if_fail(G_IS_ROBOT(object));

	robot = G_ROBOT(object);

	switch (prop_id)
	{
	case ARG_POS_X:
		robot->x = g_value_get_int(value);
		break;
	case ARG_POS_Y:
		robot->y = g_value_get_int(value);
		break;
	case ARG_DIRECTION:
		robot->dir = g_value_get_int(value);
		break;
	case ARG_SCORE:
		robot->score = g_value_get_long(value);
		break;
	case ARG_ENERGY:
		robot->energy = g_value_get_long(value);
		break;
	case ARG_SHIELDS:
		robot->shields = g_value_get_long(value);
		break;
	case ARG_SHOTS:
		robot->shots = g_value_get_long(value);
		break;
	case ARG_UNITS:
		robot->units = g_value_get_long(value);
		break;
	case ARG_USER_INTERFACE:
		if (robot->ui != NULL)
		{
			g_object_unref(robot->ui);
		}

		obj = g_value_get_object(value);
		if (obj != NULL)
		{
			robot->ui = g_object_ref(obj);
		}

		else
		{
			robot->ui = NULL;
		}
		break;
	case ARG_MAP:
		if (robot->map != NULL)
		{
			g_object_unref(robot->map);
		}

		obj = g_value_get_object(value);
		if (obj != NULL)
		{
			robot->map = g_object_ref(obj);
		}

		else
		{
			robot->map = NULL;
		}
		break;
	default:
		G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
		break;
	}
}

static void
g_robot_get_property(GObject *object, guint prop_id, GValue *value,
	GParamSpec *pspec)
{
	GRobot *robot;

	/* it's not null if we got it, but it might not be ours */
	g_return_if_fail(G_IS_ROBOT(object));

	robot = G_ROBOT(object);

	switch (prop_id)
	{
	case ARG_POS_X:
		g_value_set_int(value, robot->x);
		break;
	case ARG_POS_Y:
		g_value_set_int(value, robot->y);
		break;
	case ARG_DIRECTION:
		g_value_set_int(value, robot->dir);
		break;
	case ARG_SCORE:
		g_value_set_long(value, robot->score);
		break;
	case ARG_ENERGY:
		g_value_set_long(value, robot->energy);
		break;
	case ARG_SHIELDS:
		g_value_set_long(value, robot->shields);
		break;
	case ARG_SHOTS:
		g_value_set_long(value, robot->shots);
		break;
	case ARG_UNITS:
		g_value_set_long(value, robot->units);
		break;
	case ARG_USER_INTERFACE:
		g_value_set_object(value, g_object_ref(G_OBJECT(robot->ui)));
		break;
	case ARG_MAP:
		g_value_set_object(value, g_object_ref(G_OBJECT(robot->map)));
		break;
	default:
		G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
		break;
	}
}

GRobot *g_robot_new(gint x, gint y, gint dir, glong score, glong energy,
	glong shields, glong units, glong shots, UIArena *ui, Map *map)
{

	return G_ROBOT(g_object_new(g_robot_get_type(),
			"x", x,
			"y", y,
			"direction", dir,
			"score", score,
			"energy", energy,
			"shields", shields,
			"units", units,
			"shots", shots, "user_interface", ui, "map", map, NULL));
}

static void g_robot_dispose(GObject *object)
{
	GRobot *robot = G_ROBOT(object);

	if (robot->ui != NULL)
	{
		g_object_unref(G_OBJECT(robot->ui));
	}

	if (robot->map != NULL)
	{
		g_object_unref(G_OBJECT(robot->map));
	}

	parent_class->dispose(object);
}

static void g_robot_finalize(GObject *object)
{
	parent_class->finalize(object);
}

void g_robot_turn(GRobot *robot, gint num_turns)
{
	gint i;
	gint incr;

	/* turn left or right? */

	incr = sign(num_turns);

	for (i = 0; i < abs(num_turns); i++)
	{
		robot->dir += incr;

		if (robot->dir > 3)
		{
			robot->dir = 0;
		}

		else if (robot->dir < 0)
		{
			robot->dir = 3;
		}

		/* animate the robot */
		gdk_threads_enter();
		ui_arena_move_robot(robot->ui, robot->x, robot->y, robot->x,
			robot->y, robot->dir, robot->energy,
			robot->score, robot->shields);
		gdk_threads_leave();

		robot->energy -= 2;

		if (robot->energy < 1)
		{
			g_signal_emit(robot, g_robot_signals[DEATH], 0);
		}
	}							/* for */
}

gboolean g_robot_move(GRobot *robot, gint steps)
{
	gint x_to, y_to;
	gint dx, dy;
	gint i;

	g_return_val_if_fail(robot->ui != NULL && robot->map != NULL, FALSE);

	/* determine changes to x,y */

	switch (robot->dir)
	{
	case NORTH:				/* N */
		dx = 0;
		dy = -1 * sign(steps);
		break;
	case EAST:					/* E */
		dx = sign(steps);
		dy = 0;
		break;
	case SOUTH:				/* S */
		dx = 0;
		dy = sign(steps);
		break;
	case WEST:					/* W */
		dx = -1 * sign(steps);
		dy = 0;
		break;
	}

	/* Move the robot */

	for (i = 0; i < abs(steps); i++)
	{
		/* check for a space */

		x_to = robot->x + dx;
		y_to = robot->y + dy;

		/* no matter what, this took energy */

		robot->energy -= 2;
		if (robot->energy < 1)
		{
			g_signal_emit(robot, g_robot_signals[DEATH], 0);
		}

		switch (MAP_GET_OBJECT(robot->map, x_to, y_to))
		{
		case SPACE:
			/* move the robot there */
			MAP_SET_OBJECT(robot->map, robot->x, robot->y, SPACE);
			MAP_SET_OBJECT(robot->map, x_to, y_to, ROBOT);

			gdk_threads_enter();
			ui_arena_move_robot(robot->ui, robot->x, robot->y, x_to, y_to,
				robot->dir, robot->energy, robot->score, robot->shields);
			gdk_threads_leave();

			robot->x = x_to;
			robot->y = y_to;
			robot->units++;

			break;

		case BADDIE:
			/* Damage */
			robot->shields -= 10;
			if (robot->shields < 1)
			{
				g_signal_emit(robot, g_robot_signals[DEATH], 0);
			}
			return FALSE;

			break;

		case WALL:
			/* less damage */
			robot->shields -= 2;
			if (robot->shields < 1)
			{
				g_signal_emit(robot, g_robot_signals[DEATH], 0);
			}
			return FALSE;

			break;

		default:
			/* even less damage */
			if (--robot->shields < 1)
			{
				g_signal_emit(robot, g_robot_signals[DEATH], 0);
			}
			return FALSE;

			break;
		}
	}							/* for */

	return TRUE;
}

gboolean g_robot_smell(GRobot *robot, gchar *str)
{
	gint th;
	gint i, j;

	g_return_val_if_fail(robot->ui != NULL && robot->map != NULL, FALSE);

	th = what_thing(str);

	/* no matter what, this took energy */

	if (--robot->energy < 1)
	{
		g_signal_emit(robot, g_robot_signals[DEATH], 0);
	}

	gdk_threads_enter();
	ui_arena_robot_smell(robot->ui, robot->x, robot->y, robot->dir,
		robot->energy, robot->score, robot->shields);
	gdk_threads_leave();

	/* Smell for the thing */

	for (i = robot->x - 1; i <= robot->x + 1; i++)
	{
		for (j = robot->y - 1; j <= robot->y + 1; j++)
		{
			if (!(i == robot->x && j == robot->y)
				&& MAP_GET_OBJECT(robot->map, i, j) == th)
			{
				/* Found it */
				return TRUE;
			}
		}						/* for */
	}							/* for */

	/* Failed to find it */

	return FALSE;
}

gboolean g_robot_feel(GRobot *robot, gchar *str)
{
	gint th;
	gint x_to, y_to;
	gint dx, dy;

	g_return_val_if_fail(robot->ui != NULL && robot->map != NULL, FALSE);

	th = what_thing(str);

	/* determine changes to x,y */
	switch (robot->dir)
	{
	case NORTH:				/* N */
		dx = 0;
		dy = -1;
		break;
	case EAST:					/* E */
		dx = 1;
		dy = 0;
		break;
	case SOUTH:				/* S */
		dx = 0;
		dy = 1;
		break;
	case WEST:					/* W */
		dx = -1;
		dy = 0;
		break;
	}

	/* no matter what, this took energy */
	if (--robot->energy < 1)
	{
		g_signal_emit(robot, g_robot_signals[DEATH], 0);
	}

	/* Feel for the thing */
	x_to = robot->x + dx;
	y_to = robot->y + dy;

	gdk_threads_enter();
	ui_arena_robot_feel(robot->ui, robot->x, robot->y, robot->dir, x_to,
		y_to, robot->energy, robot->score, robot->shields);
	gdk_threads_leave();

	if (MAP_GET_OBJECT(robot->map, x_to, y_to) == BADDIE)
	{
		/* touching a baddie is hurtful */
		if (robot->shields < 1)
		{
			g_signal_emit(robot, g_robot_signals[DEATH], 0);
		}
	}

	if (MAP_GET_OBJECT(robot->map, x_to, y_to) == th)
	{
		return TRUE;
	}

	/* Did not feel it */
	return FALSE;
}

gboolean g_robot_look(GRobot *robot, gchar *str)
{
	gint th;
	gint x_to, y_to;
	gint dx, dy;

	g_return_val_if_fail(robot->ui != NULL && robot->map != NULL, FALSE);

	th = what_thing(str);

	/* determine changes to x,y */
	switch (robot->dir)
	{
	case 0:					/* N */
		dx = 0;
		dy = -1;
		break;
	case 1:					/* E */
		dx = 1;
		dy = 0;
		break;
	case 2:					/* S */
		dx = 0;
		dy = 1;
		break;
	case 3:					/* W */
		dx = -1;
		dy = 0;
		break;
	}

	/* no matter what, this took energy */
	if (--robot->energy < 1)
	{
		g_signal_emit(robot, g_robot_signals[DEATH], 0);
	}

	/* Look for the thing */
	x_to = robot->x + dx;
	y_to = robot->y + dy;

	gdk_threads_enter();
	ui_arena_robot_look(robot->ui, robot->x, robot->y, robot->dir, x_to,
		y_to, robot->energy, robot->score, robot->shields);
	gdk_threads_leave();

	while (MAP_GET_OBJECT(robot->map, x_to, y_to) == SPACE)
	{
		/* move the focus */
		x_to += dx;
		y_to += dy;
	}

	/* Outside the loop, we have found something */
	if (MAP_GET_OBJECT(robot->map, x_to, y_to) == th)
	{
		return TRUE;
	}

	/* else, we did not find it */
	return FALSE;
}

gboolean g_robot_grab(GRobot *robot)
{
	gint x_to, y_to;
	gint dx, dy;

	g_return_val_if_fail(robot->ui != NULL && robot->map != NULL, FALSE);

	/* determine changes to x,y */

	switch (robot->dir)
	{
	case NORTH:				/* N */
		dx = 0;
		dy = -1;
		break;
	case EAST:					/* E */
		dx = 1;
		dy = 0;
		break;
	case SOUTH:				/* S */
		dx = 0;
		dy = 1;
		break;
	case WEST:					/* W */
		dx = -1;
		dy = 0;
		break;
	}

	/* Try to grab the thing */

	x_to = robot->x + dx;
	y_to = robot->y + dy;

	robot->energy -= 5;
	if (robot->energy < 1)
	{
		g_signal_emit(robot, g_robot_signals[DEATH], 0);
	}

	gdk_threads_enter();
	ui_arena_robot_grab(robot->ui, robot->x, robot->y, robot->dir, x_to,
		y_to, robot->energy, robot->score, robot->shields);
	gdk_threads_leave();

	/* Did we grab it? */

	switch (MAP_GET_OBJECT(robot->map, x_to, y_to))
	{
	case SPACE:
	case WALL:
	case ROBOT:
		return FALSE;
		break;

	case BADDIE:
		robot->shields -= 10;
		if (robot->shields < 1)
		{
			g_signal_emit(robot, g_robot_signals[DEATH], 0);
		}
		return (FALSE);

	case FOOD:
		/* I want the net gain to be +10 */
		robot->energy += 15;
		break;

	case PRIZE:
		robot->score++;
		break;
	}

	/* only successful grabs get here */
	MAP_SET_OBJECT(robot->map, x_to, y_to, SPACE);
	gdk_threads_enter();
	ui_arena_add_thing(robot->ui, x_to, y_to, SPACE);
	gdk_threads_leave();

	return TRUE;
}

gboolean g_robot_zap(GRobot *robot)
{
	gint x_to, y_to;
	gint dx, dy;

	g_return_val_if_fail(robot->ui != NULL && robot->map != NULL, FALSE);

	/* determine changes to x,y */

	switch (robot->dir)
	{
	case NORTH:				/* N */
		dx = 0;
		dy = -1;
		break;
	case EAST:					/* E */
		dx = 1;
		dy = 0;
		break;
	case SOUTH:				/* S */
		dx = 0;
		dy = 1;
		break;
	case WEST:					/* W */
		dx = -1;
		dy = 0;
		break;
	}

	/* Try to zap the thing */

	x_to = robot->x + dx;
	y_to = robot->y + dy;

	robot->energy -= 10;
	if (robot->energy < 1)
	{
		g_signal_emit(robot, g_robot_signals[DEATH], 0);
	}
	robot->shots++;
	gdk_threads_enter();
	ui_arena_robot_zap(robot->ui, robot->x, robot->y, robot->dir, x_to,
		y_to, robot->energy, robot->score, robot->shields);
	gdk_threads_leave();

	/* Did we destroy it? */
	switch (MAP_GET_OBJECT(robot->map, x_to, y_to))
	{
	case SPACE:
	case WALL:
	case ROBOT:				/* what to w/ robots? */
		return (FALSE);
		break;

	case BADDIE:
	case FOOD:
	case PRIZE:
		gdk_threads_enter();
		ui_arena_add_thing(robot->ui, x_to, y_to, SPACE);
		gdk_threads_leave();
		break;
	}

	/* only success gets here */
	MAP_SET_OBJECT(robot->map, x_to, y_to, SPACE);
	gdk_threads_enter();
	ui_arena_add_thing(robot->ui, x_to, y_to, SPACE);
	gdk_threads_leave();

	return TRUE;
}

gboolean g_robot_stop(GRobot *robot)
{
	/* Must be a SCM function, even though it returns no value */
	/* Stop the robot immediately */

	g_signal_emit(robot, g_robot_signals[DEATH], 0);

	return TRUE;				/* never gets here */
}

static gint what_thing(const gchar *th)
{
	/* what_thing - this function scans the list of possible things
	   (strings) and returns a cthing.  Returns -1 if not found in the
	   list. */

	/* My idea here is that by return -1 on error, this won't match
	   anything in the list of cthings.  That way, the function that
	   uses what_thing to determine the cthing doesn't have to care if
	   the call failed or not.  This helps me keep the code simple,
	   since now I don't have to add a branch for failure, but which
	   also decrements energy. */

	gint i;

	for (i = 0; i < 6; i++)
	{
		if (strcmp(th, things[i]) == 0)
		{
			return cthings[i];
		}
	}							/* for */

	/* not found */

	return -1;
}
