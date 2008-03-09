/* Copyright (C) 1998 Jim Hall <jhall1@isd.net>
 * Copyright (C) 2008 Bradley Smith <brad@brad-smith.co.uk>
 *
 * GNU Robots game engine.
 *
 * GNU Robots is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * GNU Robots is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with GNU Robots; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include <glib.h>
#include <glib/gprintf.h>
#include <stdio.h>
#include <stdlib.h>
#include "configs.h"
#include "textplugin.h"

#include <readline.h>
#include <history.h>

enum
{
  ARG_0,
  ARG_MAP
};

GType _text_plugin_type = 0;
static GType _parent_type = 0;

static void text_plugin_class_init (TextPluginClass * klass);
static void text_plugin_init (GObject * object);
static void text_plugin_interface_init (gpointer g_iface, gpointer iface_data);

static GObject * text_plugin_constructor (GType type, 
    guint n_construct_properties, 
    GObjectConstructParam *construct_properties);

static void text_plugin_finalize (GObject * object);
static void text_plugin_dispose (GObject * object);

static void text_plugin_set_property (GObject * object, guint prop_id,
    const GValue * value, GParamSpec * pspec);

static void text_plugin_get_property (GObject * object, guint prop_id,
    GValue * value, GParamSpec * pspec);

inline void text_plugin_update_status (TextPlugin *text,
				   const gchar *s, 
				   glong energy, 
				   glong score, 
				   glong shields);

static GObjectClass *parent_class = NULL;

/* For translation of directions to strings */
static gchar *str_dir[] = { "North", "East", "South", "West" };

GType
text_plugin_get_type (void)
{
  if (!_text_plugin_type) {
    static const GTypeInfo object_info = {
      sizeof (TextPluginClass),
      NULL,
      NULL,
      (GClassInitFunc) text_plugin_class_init,
      NULL,
      NULL,
      sizeof (TextPlugin),
      0,
      (GInstanceInitFunc) text_plugin_init,
      NULL
    };

    static const GInterfaceInfo interface_info = {
      (GInterfaceInitFunc) text_plugin_interface_init,
      NULL,
      NULL
    };

    _text_plugin_type =
        g_type_register_static (G_TYPE_OBJECT, 
			"TextPlugin", 
			&object_info,
			0);
    
    g_type_add_interface_static (_text_plugin_type,
                                 _parent_type,
                                 &interface_info);
  }

  return _text_plugin_type;
}

static void
text_plugin_class_init (TextPluginClass * klass)
{
  GObjectClass *gobject_class;

  gobject_class = (GObjectClass *) klass;

  parent_class = g_type_class_ref (G_TYPE_OBJECT);

  gobject_class->constructor = text_plugin_constructor;
  gobject_class->set_property = text_plugin_set_property;
  gobject_class->get_property = text_plugin_get_property;
  gobject_class->dispose = text_plugin_dispose;
  gobject_class->finalize = text_plugin_finalize;

  g_object_class_override_property (gobject_class, ARG_MAP, "map");
}

static void
text_plugin_init (GObject * object)
{
  TextPlugin *text = TEXT_PLUGIN (object);

  text->map = NULL;
  text->map_size = NULL;
}

static GObject *
text_plugin_constructor (GType type,
    guint n_construct_properties,
    GObjectConstructParam *construct_properties)
{
  GObject *object;

  /* Chain up to the parent first */
  object = parent_class->constructor (type, n_construct_properties, construct_properties);

  g_printf ("GNU Robots starting..\n");

  return object;
}

static void
text_plugin_dispose (GObject * object)
{
  TextPlugin *text = TEXT_PLUGIN (object);

  if (text->map != NULL) {
    g_object_unref (G_OBJECT (text->map));

    if (text->map_size != NULL) {
      g_free (text->map_size);
    }
  }

  parent_class->dispose (object);
}

/* finalize is called when the object has to free its resources */
static void
text_plugin_finalize (GObject * object)
{
  g_printf ("GNU Robots done.\n");
  
  parent_class->finalize (object);
}

static void
text_plugin_set_property (GObject * object, guint prop_id,
    const GValue * value, GParamSpec * pspec)
{
  TextPlugin *text;
  GObject *obj;

  /* it's not null if we got it, but it might not be ours */
  g_return_if_fail (G_IS_TEXT_PLUGIN (object));

  text = TEXT_PLUGIN (object);

  switch (prop_id) {
    case ARG_MAP:
      obj = g_value_get_object (value);
      g_return_if_fail (obj != NULL);
      
      if (text->map != NULL) {
	g_object_unref (text->map);
      }

      text->map = MAP (g_object_ref (obj));

      if (text->map_size != NULL) {
        g_free (text->map_size);
      }

      g_object_get (G_OBJECT (text->map),
		"size", &text->map_size,
		NULL);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
  }
}

static void
text_plugin_get_property (GObject * object, guint prop_id,
    GValue * value, GParamSpec * pspec)
{
  TextPlugin *text;

  /* it's not null if we got it, but it might not be ours */
  g_return_if_fail (G_IS_TEXT_PLUGIN (object));

  text = TEXT_PLUGIN (object);

  switch (prop_id) {
    case ARG_MAP:
      g_value_set_object (value, g_object_ref (G_OBJECT (text->map)));
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
  }
}

UserInterface *
user_interface_new (Map *map, GType parent_type)
{
  TextPlugin *text;

  g_return_val_if_fail (map != NULL, NULL);
  g_return_val_if_fail (_parent_type != 0 || parent_type != 0, NULL);

  if (!_parent_type) {
     _parent_type = parent_type;
  }

  text = TEXT_PLUGIN (g_object_new (text_plugin_get_type (),
				  "map", map,
				  NULL));
  if (text->errors) {
    g_object_unref (G_OBJECT (text));
    return NULL;
  }

  return USER_INTERFACE (text);
}

inline void text_plugin_add_thing (TextPlugin *text,
				      gint x, 
				      gint y, 
				      gint thing)
{
  g_printf ("thing `%c' added at %d,%d\n", thing, x, y);
}

inline void text_plugin_draw (TextPlugin *text)
{
  gint i, j;

  for (j = 0; j < text->map_size->num_rows; j++) {
    for (i = 0; i < text->map_size->num_cols; i++) {
      g_printf ("thing `%c' added at %d,%d\n", MAP_GET_OBJECT (text->map, i, j), i, j);
    }
  }
}

inline void text_plugin_move_robot (TextPlugin *text,
				       gint from_x, 
				       gint from_y, 
				       gint x, 
				       gint y, 
				       gint cdir,
				       glong energy,
				       glong score, 
				       glong shields)
{
  g_printf ("robot now at %d,%d facing %s\n", x, y, str_dir[cdir]);
}

/* function to animate the robot */
inline void text_plugin_robot_smell (TextPlugin *text,
					gint x, 
					gint y, 
					gint cdir,
				        glong energy,
				        glong score, 
				        glong shields)
{
  g_printf ("the robot sniffs..\n");
}

inline void text_plugin_robot_zap (TextPlugin *text,
				      gint x, 
				      gint y, 
				      gint cdir, 
				      gint x_to, 
				      gint y_to,
				      glong energy,
				      glong score, 
				      glong shields)
{
  g_printf ("robot fires gun at space %d,%d\n", x_to, y_to);
}

inline void text_plugin_robot_feel (TextPlugin *text,
				       gint x, 
				       gint y, 
				       gint cdir, 
				       gint x_to, 
				       gint y_to,
				       glong energy,
				       glong score, 
				       glong shields)
{
  g_printf ("robot feels space %d,%d\n", x_to, y_to);
}

inline void text_plugin_robot_grab (TextPlugin *text,
				       gint x, 
				       gint y, 
				       gint cdir, 
				       gint x_to, 
				       gint y_to,
				       glong energy,
				       glong score, 
				       glong shields)
{
  g_printf ("robot grabs for space %d,%d\n", x_to, y_to);
}

inline void text_plugin_robot_look (TextPlugin *text,
				       gint x, 
				       gint y, 
				       gint cdir, 
				       gint x_to, 
				       gint y_to,
				       glong energy,
				       glong score, 
				       glong shields)
{
  g_printf ("robot looks towards %s from %d,%d\n", str_dir[cdir], x, y);
}

/* function to get data from user */
inline void text_plugin_get_string (TextPlugin *text,
				       gchar *prompt, 
				       gchar *buff, 
				       gint len)
{
  char* line = (char*)NULL;

  line = readline(prompt);

  if(line && *line)
  {
    add_history(line);

    g_strlcpy(buff, line, len);
  }
  else
    buff = "";

  free(line);
}

inline void text_plugin_update_status (TextPlugin *text,
				   const gchar *s, 
				   glong energy, 
				   glong score, 
				   glong shields)
{
  puts (s);
}

static void text_plugin_interface_init (gpointer g_iface, gpointer iface_data)
{
  UserInterfaceClass *klass = (UserInterfaceClass *)g_iface;

  klass->user_interface_add_thing = (void (*) (UserInterface *ui,
				      	       gint x, 
				               gint y, 
				               gint thing)) 
	  			    text_plugin_add_thing;

  klass->user_interface_draw = (void (*) (UserInterface *ui)) text_plugin_draw;
  klass->user_interface_update_status = (void (*) (UserInterface *ui,
				  	 	   const gchar *s, 
				  	 	   glong energy,
				  	 	   glong score, 
				  	 	   glong shields)) 
	  				text_plugin_update_status;
  klass->user_interface_move_robot = (void (*) (UserInterface *ui, 
					 	gint from_x, 
					 	gint from_y, 
					 	gint to_x, 
					 	gint to_y, 
					 	gint cdir,
		 			 	glong energy, 
					 	glong score, 
					 	glong shields))
	  			     text_plugin_move_robot;
  klass->user_interface_robot_smell = (void (*) (UserInterface *ui, 
					 	 gint x, 
					 	 gint y, 
					 	 gint cdir,
		 			 	 glong energy, 
					 	 glong score, 
					 	 glong shields))
	  			      text_plugin_robot_smell;
  klass->user_interface_robot_zap = (void (*) (UserInterface *ui,
			  		       gint x, 
					       gint y, 
					       gint cdir, 
			       		       gint x_to, 
					       gint y_to,
		 			       glong energy, 
					       glong score, 
					       glong shields))
	  			    text_plugin_robot_zap;
  klass->user_interface_robot_feel = (void (*) (UserInterface *ui, 
					 gint x, 
					 gint y, 
					 gint cdir, 
					 gint x_to, 
					 gint y_to,
		 			 glong energy, 
					 glong score, 
					 glong shields))
	  			     text_plugin_robot_grab;
  klass->user_interface_robot_grab = (void (*) (UserInterface *ui,
				         	gint x, 
				         	gint y, 
				         	gint cdir, 
				         	gint x_to, 
				         	gint y_to,
				         	glong energy,
				         	glong score, 
				         	glong shields))
	  	                     text_plugin_robot_grab;
  klass->user_interface_robot_look = (void (*) (UserInterface *ui,
					 	gint x, 
					 	gint y, 
					 	gint cdir, 
					 	gint x_to, 
					 	gint y_to,
		 			 	glong energy, 
					 	glong score, 
					 	glong shields))
	  			     text_plugin_robot_look;
  klass->user_interface_get_string = (void (*) (UserInterface *ui,
					 gchar *prompt, 
					 gchar *buff, 
					 gint len))
	  			     text_plugin_get_string;
}
