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

#ifndef __TEXT_PLUGIN_H__
#define __TEXT_PLUING_H__

#include <glib-object.h>
#include "userinterface.h"
#include "map.h"

G_BEGIN_DECLS

extern GType _text_plugin_type;

typedef struct _TextPlugin TextPlugin;

struct _TextPlugin {
  GObject 	object;
  Map 		*map;
  MapSize 	*map_size;
  gint 		errors;
};

typedef struct _TextPluginClass TextPluginClass;

struct _TextPluginClass {
  GObjectClass	parent_class;
};

#define G_TYPE_TEXT_PLUGIN			(_text_plugin_type)
#define G_IS_TEXT_PLUGIN(obj)		(G_TYPE_CHECK_INSTANCE_TYPE ((obj), G_TYPE_TEXT_PLUGIN))
#define G_IS_TEXT_PLUGIN_CLASS(klass)	(G_TYPE_CHECK_CLASS_TYPE ((klass), G_TYPE_TEXT_PLUGIN))
#define TEXT_PLUGIN_GET_CLASS(obj)	(G_TYPE_INSTANCE_GET_CLASS ((obj), G_TYPE_TEXT_PLUGIN, TextPluginClass))
#define TEXT_PLUGIN(obj)			(G_TYPE_CHECK_INSTANCE_CAST ((obj), G_TYPE_TEXT_PLUGIN, TextPlugin))
#define TEXT_PLUGIN_CLASS(klass)		(G_TYPE_CHECK_CLASS_CAST ((klass), G_TYPE_TEXT_PLUGIN, TextPluginClass))

/* normal GObject stuff */
GType		text_plugin_get_type		(void);

TextPlugin*	text_plugin_new 		(void);

G_END_DECLS

#endif /* __TEXT_PLUGIN_H__*/
