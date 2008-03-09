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

#ifndef __X11_PLUGIN_H__
#define __X11_PLUGIN_H__

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/xpm.h>
#include <X11/keysym.h>

#include <glib-object.h>
#include "userinterface.h"
#include "map.h"

G_BEGIN_DECLS

typedef struct _X11Plugin X11Plugin;
typedef struct _X11PluginClass X11PluginClass;

struct _X11Plugin {
  GObject 	object;
  Map 		*map;
  MapSize 	*map_size;
  gint 		errors;

  Display 	*dpy;
  Window 	x_win;
  GC 		gc;
  GC 		buf_gc;
  Font 		text;

#ifdef USE_MITSHM
#include <sys/ipc.h>
#include <sys/shm.h>
#include <X11/extensions/XShm.h>
  XShmSegmentInfo shm_info;
  guchar 	      use_mitshm = 1;
#endif

  gint 		win_width;
  gint 		win_height;

  Pixmap 	win_buf;

  XImage 	*win_bufi;
  XImage 	*statusbar;
  XImage 	*space;
  XImage 	*food;
  XImage 	*wall;
  XImage 	*prize;
  XImage 	*baddie;
  XImage 	*robotDirs[4];
  XImage 	*robotPix;
};

struct _X11PluginClass {
  GObjectClass	parent_class;
};

#define G_TYPE_X11_PLUGIN			  (x11_plugin_get_type())
#define G_IS_X11_PLUGIN(obj)		  (G_TYPE_CHECK_INSTANCE_TYPE ((obj), \
                                          G_TYPE_X11_PLUGIN))
#define G_IS_X11_PLUGIN_CLASS(klass)  (G_TYPE_CHECK_CLASS_TYPE ((klass), \
                                          G_TYPE_X11_PLUGIN))
#define X11_PLUGIN_GET_CLASS(obj)	  (G_TYPE_INSTANCE_GET_CLASS ((obj), \
                                          G_TYPE_X11_PLUGIN, \
                                          X11PluginClass))
#define X11_PLUGIN(obj)			      (G_TYPE_CHECK_INSTANCE_CAST ((obj), \
                                          G_TYPE_X11_PLUGIN, X11Plugin))
#define X11_PLUGIN_CLASS(klass)		  (G_TYPE_CHECK_CLASS_CAST ((klass), \
                                          G_TYPE_X11_PLUGIN, \
                                          X11PluginClass))

/* normal GObject stuff */
GType       x11_plugin_get_type		(void);

X11Plugin*	x11_plugin_new 			(void);

/* SYMBOLIC CONSTANTS */
#define dist(f_x, f_y, t_x, t_y) (abs((f_x)-(t_x))+abs((f_y)-(t_y)))

G_END_DECLS

#endif /* __X11_PLUGIN_H__ */
