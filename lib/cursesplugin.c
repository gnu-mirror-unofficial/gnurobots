/* $Id: cursesplugin.c,v 1.1 2004/10/21 19:24:30 zeenix Exp $ */

/* GNU Robots game engine. */

/* Copyright (C) 1998 Jim Hall, jhall1@isd.net */

/*
   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include <glib.h>
#include <stdio.h>
#include "configs.h"
#include "cursesplugin.h"

enum
{
  ARG_0,
  ARG_MAP 
};

GType _curses_plugin_type = 0;
static GType _parent_type = 0;

static void curses_plugin_class_init (CursesPluginClass * klass);
static void curses_plugin_init (GObject * object);
static void curses_plugin_interface_init (gpointer g_iface, gpointer iface_data);

static GObject * curses_plugin_constructor (GType type, 
    guint n_construct_properties, 
    GObjectConstructParam *construct_properties);

static void curses_plugin_finalize (GObject * object);
static void curses_plugin_dispose (GObject * object);

static void curses_plugin_set_property (GObject * object, guint prop_id,
    const GValue * value, GParamSpec * pspec);

static void curses_plugin_get_property (GObject * object, guint prop_id,
    GValue * value, GParamSpec * pspec);

inline void curses_plugin_update_status (CursesPlugin *curses,
				   const gchar *s, 
				   glong energy, 
				   glong score, 
				   glong shields);

static GObjectClass *parent_class = NULL;

GType
curses_plugin_get_type (void)
{
  if (!_curses_plugin_type) {
    static const GTypeInfo object_info = {
      sizeof (CursesPluginClass),
      NULL,
      NULL,
      (GClassInitFunc) curses_plugin_class_init,
      NULL,
      NULL,
      sizeof (CursesPlugin),
      0,
      (GInstanceInitFunc) curses_plugin_init,
      NULL
    };

    static const GInterfaceInfo interface_info = {
      (GInterfaceInitFunc) curses_plugin_interface_init,
      NULL,
      NULL
    };

    _curses_plugin_type =
        g_type_register_static (G_TYPE_OBJECT, 
			"CursesPlugin", 
			&object_info,
			0);
    
    g_type_add_interface_static (_curses_plugin_type,
                                 _parent_type,
                                 &interface_info);
  }

  return _curses_plugin_type;
}

static void
curses_plugin_class_init (CursesPluginClass * klass)
{
  GObjectClass *gobject_class;

  gobject_class = (GObjectClass *) klass;

  parent_class = g_type_class_ref (G_TYPE_OBJECT);

  gobject_class->constructor = curses_plugin_constructor;
  gobject_class->set_property = curses_plugin_set_property;
  gobject_class->get_property = curses_plugin_get_property;
  gobject_class->dispose = curses_plugin_dispose;
  gobject_class->finalize = curses_plugin_finalize;

  g_object_class_override_property (gobject_class, ARG_MAP, "map");
}

static void
curses_plugin_init (GObject * object)
{
  CursesPlugin *curses = CURSES_PLUGIN (object);

  curses->map = NULL;
  curses->map_size = NULL;
}

static GObject *
curses_plugin_constructor (GType type, 
    guint n_construct_properties, 
    GObjectConstructParam *construct_properties)
{
  GObject *object;
  CursesPlugin *curses;

  /* Chain up to the parent first */
  object = parent_class->constructor (type, n_construct_properties, construct_properties);
  
  curses = CURSES_PLUGIN (object);

  /* Initialize curses mode */
  gint color_pair;

  curses->win = initscr ();

  cbreak ();
  noecho ();

  scrollok (curses->win, TRUE); 	/* Put scrolling on */
  setscrreg (LINES - 2, LINES - 1);	/* Where to scroll  */

  //clearok (stdscr, TRUE);
  nonl ();
  intrflush (stdscr, FALSE);
  keypad (stdscr, TRUE);

  if (has_colors()) {
    start_color();

    /*
     * Simple color assignment, often all we need.
     **/
    init_pair (COLOR_BLACK, COLOR_BLACK, COLOR_BLACK);
    init_pair (COLOR_GREEN, COLOR_GREEN, COLOR_BLACK);
    init_pair (COLOR_RED, COLOR_RED, COLOR_BLACK);
    init_pair (COLOR_CYAN, COLOR_CYAN, COLOR_BLACK);
    init_pair (COLOR_WHITE, COLOR_WHITE, COLOR_BLACK);
    init_pair (COLOR_MAGENTA, COLOR_MAGENTA, COLOR_BLACK);
    init_pair (COLOR_BLUE, COLOR_BLUE, COLOR_BLACK);
    init_pair (COLOR_YELLOW, COLOR_YELLOW, COLOR_BLACK);
  }

  clear ();
  refresh ();
  curses->errors = 0;

  return object;
}

static void 
curses_plugin_dispose (GObject * object)
{
  CursesPlugin *curses = CURSES_PLUGIN (object);
 
  if (curses->map != NULL) {
    g_object_unref (G_OBJECT (curses->map));

    if (curses->map_size != NULL) {
      g_free (curses->map_size);
    }
  }

  parent_class->dispose (object);
}

/* finalize is called when the object has to free its resources */
static void
curses_plugin_finalize (GObject * object)
{
  /* End curses mode */
  endwin ();

  parent_class->finalize (object);
}

static void
curses_plugin_set_property (GObject * object, guint prop_id,
    const GValue * value, GParamSpec * pspec)
{
  CursesPlugin *curses;
  GObject *obj;

  /* it's not null if we got it, but it might not be ours */
  g_return_if_fail (G_IS_CURSES_PLUGIN (object));

  curses = CURSES_PLUGIN (object);

  switch (prop_id) {
    case ARG_MAP:
      obj = g_value_get_object (value);
      g_return_if_fail (obj != NULL);
      
      if (curses->map != NULL) {
	g_object_unref (curses->map);
      }

      curses->map = MAP (g_object_ref (obj));

      if (curses->map_size != NULL) {
        g_free (curses->map_size);
      }

      g_object_get (G_OBJECT (curses->map),
		"size", &curses->map_size,
		NULL);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
  }
}

static void
curses_plugin_get_property (GObject * object, guint prop_id,
    GValue * value, GParamSpec * pspec)
{
  CursesPlugin *curses;

  /* it's not null if we got it, but it might not be ours */
  g_return_if_fail (G_IS_CURSES_PLUGIN (object));

  curses = CURSES_PLUGIN (object);

  switch (prop_id) {
    case ARG_MAP:
      g_value_set_object (value, g_object_ref (G_OBJECT (curses->map)));
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
  }
}

UserInterface *
user_interface_new (Map *map, GType parent_type)
{
  g_return_val_if_fail (map != NULL, NULL);
  g_return_val_if_fail (_parent_type != 0 || parent_type != 0, NULL);

  if (!_parent_type) {
     _parent_type = parent_type;
  }

  CursesPlugin *curses =
  	CURSES_PLUGIN (g_object_new (curses_plugin_get_type (),
				  "map", map,
				  NULL));
  if (curses->errors) {
    g_object_unref (G_OBJECT (curses));
    return NULL;
  }

  return USER_INTERFACE (curses);
}

inline void curses_plugin_add_thing (CursesPlugin *curses,
				      gint x, 
				      gint y, 
				      gint thing)
{
  gshort color;

  /* Highlight the unusual chars */
  if (thing == '?') {
    standout ();
  }

  switch (thing) {
    case SPACE:
	color = COLOR_PAIR (COLOR_BLACK);
	break;
    case FOOD:
	color = COLOR_PAIR (COLOR_GREEN);
	break;
    case PRIZE:
	color = COLOR_PAIR (COLOR_YELLOW);
	break;
    case WALL:
	color = COLOR_PAIR (COLOR_WHITE);
	break;
    case BADDIE:
	color = COLOR_PAIR (COLOR_RED);
	break;
    case ROBOT:
	color = COLOR_PAIR (COLOR_BLUE);
	break;
    default:
	/* What here? */
	color = COLOR_PAIR (COLOR_BLACK);
  }

  mvaddch (y, x, thing | color);

  if (thing == '?') {
    standend ();
  }

  redrawwin (curses->win);
}

inline void curses_plugin_draw (CursesPlugin *curses)
{
  gshort color;
  gint i, j;

  /* Draw the map for the GNU Robots game. */

  for (j = 0; j < curses->map_size->num_rows; j++) {
    for (i = 0; i < curses->map_size->num_cols; i++) {
      /* Special cases */

      switch (MAP_GET_OBJECT (curses->map, i, j)) {
        case '\0':
  	   /* Highlight the unusual chars */
  	   if (MAP_GET_OBJECT (curses->map, i, j) == '?') {
	     standout ();
  	   }
	   color = COLOR_PAIR (COLOR_BLACK);
           break;
    	case SPACE:
	   color = COLOR_PAIR (COLOR_BLACK);
	   break;
    	case FOOD:
	   color = COLOR_PAIR (COLOR_GREEN);
	   break;
    	case PRIZE:
	   color = COLOR_PAIR (COLOR_YELLOW);
	   break;
    	case WALL:
	   color = COLOR_PAIR (COLOR_WHITE);
	   break;
    	case BADDIE:
	  color = COLOR_PAIR (COLOR_RED);
	  break;
    	case ROBOT:
	  color = COLOR_PAIR (COLOR_BLUE);
	  break;
        default:
	  /* What here? */
	  color = COLOR_PAIR (COLOR_BLACK);
          break;
      }                         /* switch */
  
      mvaddch (j, i, MAP_GET_OBJECT (curses->map, i, j) | color);

      if (MAP_GET_OBJECT (curses->map, i, j) == '?') {
    	standend ();
      }
    }                           /* for i */
  }                             /* for j */

  redrawwin (curses->win);
}

inline void curses_plugin_move_robot (CursesPlugin *curses,
				       gint from_x, 
				       gint from_y, 
				       gint to_x, 
				       gint to_y, 
				       gint cdir,
				       glong energy,
				       glong score, 
				       glong shields)
{
  /* Clear previous tile */

  mvaddch (from_y, from_x, ' ' | COLOR_PAIR (COLOR_BLACK));

  standout ();
  switch (cdir) {
    case NORTH:
      mvaddch (to_y, to_x, '^' | COLOR_PAIR (COLOR_BLUE));
      break;
    case EAST:
      mvaddch (to_y, to_x, '>' | COLOR_PAIR (COLOR_BLUE));
      break;
    case SOUTH:
      mvaddch (to_y, to_x, 'v' | COLOR_PAIR (COLOR_BLUE));
      break;
    case WEST:
      mvaddch (to_y, to_x, '<' | COLOR_PAIR (COLOR_BLUE));
      break;
  }

  standend ();
  refresh ();
  
  g_usleep (USLEEP_TIME);
}

/* function to animate the robot */
inline void curses_plugin_robot_smell (CursesPlugin *curses,
					gint x, 
					gint y, 
					gint cdir,
				        glong energy,
				        glong score, 
				        glong shields)
{
  curses_plugin_update_status (curses, "robot smells...", energy, score, shields);
}

inline void curses_plugin_robot_zap (CursesPlugin *curses,
				      gint x, 
				      gint y, 
				      gint cdir, 
				      gint x_to, 
				      gint y_to,
				      glong energy,
				      glong score, 
				      glong shields)
{
  curses_plugin_update_status (curses, "robot fires his little gun...", energy, score, shields);
}

inline void curses_plugin_robot_feel (CursesPlugin *curses,
				       gint x, 
				       gint y, 
				       gint cdir, 
				       gint x_to, 
				       gint y_to,
				       glong energy,
				       glong score, 
				       glong shields)
{
  curses_plugin_update_status (curses, "robot feels for a thing...", energy, score, shields);
}

inline void curses_plugin_robot_grab (CursesPlugin *curses,
				       gint x, 
				       gint y, 
				       gint cdir, 
				       gint x_to, 
				       gint y_to,
				       glong energy,
				       glong score, 
				       glong shields)
{
  curses_plugin_update_status (curses, "robot grabs thing...", energy, score, shields);
}

inline void curses_plugin_robot_look (CursesPlugin *curses,
				       gint x, 
				       gint y, 
				       gint cdir, 
				       gint x_to, 
				       gint y_to,
				       glong energy,
				       glong score, 
				       glong shields)
{
  curses_plugin_update_status (curses, "robot looks for a thing...", energy, score, shields);
}

/* function to get data from user */
inline void curses_plugin_get_string (CursesPlugin *curses,
				       gchar *prompt, 
				       gchar *buff, 
				       gint len)
{
  mvaddstr (LINES - 2, 0, prompt);
  refresh ();

  echo ();
  getnstr (buff, len);
  noecho ();
  
  move (LINES - 2, 0);
  clrtoeol ();
  refresh ();
}

inline void curses_plugin_update_status (CursesPlugin *curses,
				   const gchar *s, 
				   glong energy, 
				   glong score, 
				   glong shields)
{
  /* print on mode line (y=LINES-1) */
  mvaddstr (LINES - 1, 1, s);
  refresh ();

  /* Sleep, then erase it */
  napms (SLEEP_TIME);

  move (LINES - 1, 1);
  clrtoeol ();
  refresh ();
}

static void curses_plugin_interface_init (gpointer g_iface, gpointer iface_data)
{
  UserInterfaceClass *klass = (UserInterfaceClass *)g_iface;

  klass->user_interface_add_thing = (void (*) (UserInterface *ui,
				      	       gint x, 
				               gint y, 
				               gint thing)) 
	  			    curses_plugin_add_thing;

  klass->user_interface_draw = (void (*) (UserInterface *ui)) curses_plugin_draw;
  klass->user_interface_update_status = (void (*) (UserInterface *ui,
				  	 	   const gchar *s, 
				  	 	   glong energy,
				  	 	   glong score, 
				  	 	   glong shields)) 
	  				curses_plugin_update_status;
  klass->user_interface_move_robot = (void (*) (UserInterface *ui, 
					 	gint from_x, 
					 	gint from_y, 
					 	gint to_x, 
					 	gint to_y, 
					 	gint cdir,
		 			 	glong energy, 
					 	glong score, 
					 	glong shields))
	  			     curses_plugin_move_robot;
  klass->user_interface_robot_smell = (void (*) (UserInterface *ui, 
					 	 gint x, 
					 	 gint y, 
					 	 gint cdir,
		 			 	 glong energy, 
					 	 glong score, 
					 	 glong shields))
	  			      curses_plugin_robot_smell;
  klass->user_interface_robot_zap = (void (*) (UserInterface *ui,
			  		       gint x, 
					       gint y, 
					       gint cdir, 
			       		       gint x_to, 
					       gint y_to,
		 			       glong energy, 
					       glong score, 
					       glong shields))
	  			    curses_plugin_robot_zap;
  klass->user_interface_robot_feel = (void (*) (UserInterface *ui, 
					 gint x, 
					 gint y, 
					 gint cdir, 
					 gint x_to, 
					 gint y_to,
		 			 glong energy, 
					 glong score, 
					 glong shields))
	  			     curses_plugin_robot_grab;
  klass->user_interface_robot_grab = (void (*) (UserInterface *ui,
				         	gint x, 
				         	gint y, 
				         	gint cdir, 
				         	gint x_to, 
				         	gint y_to,
				         	glong energy,
				         	glong score, 
				         	glong shields))
	  	                     curses_plugin_robot_grab;
  klass->user_interface_robot_look = (void (*) (UserInterface *ui,
					 	gint x, 
					 	gint y, 
					 	gint cdir, 
					 	gint x_to, 
					 	gint y_to,
		 			 	glong energy, 
					 	glong score, 
					 	glong shields))
	  			     curses_plugin_robot_look;
  klass->user_interface_get_string = (void (*) (UserInterface *ui,
					 gchar *prompt, 
					 gchar *buff, 
					 gint len))
	  			     curses_plugin_get_string;
}
