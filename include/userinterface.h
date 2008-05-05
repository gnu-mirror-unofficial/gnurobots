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

#ifndef __USER_INTERFACE_H__
#define __USER_INTERFACE_H__

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/xpm.h>
#include <X11/keysym.h>

#include <glib-object.h>
/*#include "userinterface.h"*/
#include "map.h"

G_BEGIN_DECLS

#define G_TYPE_USER_INTERFACE			  user_interface_get_type()
#define G_IS_USER_INTERFACE(obj)		  G_TYPE_CHECK_INSTANCE_TYPE ((obj), \
                                          G_TYPE_USER_INTERFACE)
#define G_IS_USER_INTERFACE_CLASS(klass)  G_TYPE_CHECK_CLASS_TYPE ((klass), \
                                          G_TYPE_USER_INTERFACE)
#define USER_INTERFACE_GET_CLASS(obj)	  G_TYPE_INSTANCE_GET_CLASS ((obj), \
                                          G_TYPE_USER_INTERFACE, \
                                          UserInterfaceClass)
#define USER_INTERFACE(obj)			      G_TYPE_CHECK_INSTANCE_CAST ((obj), \
                                          G_TYPE_USER_INTERFACE, UserInterface)
#define USER_INTERFACE_CLASS(klass)		  G_TYPE_CHECK_CLASS_CAST ((klass), \
                                          G_TYPE_USER_INTERFACE, \
                                          UserInterfaceClass)

typedef struct _UserInterface UserInterface;
typedef struct _UserInterfaceClass UserInterfaceClass;

struct _UserInterface {
  GObject 	object;
  Map 		*map;
  MapSize 	*map_size;

  Display 	*dpy;
  Window 	x_win;
  Atom      wm_delete_win;
  Atom      wm_protocols;
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

struct _UserInterfaceClass {
  GObjectClass	parent_class;
};

/* normal GObject stuff */
GType       user_interface_get_type		(void);

UserInterface*	user_interface_new 		(Map* map);

void    user_interface_add_thing        (UserInterface *ui,
                     gint x,
                     gint y,
                     gint thing);

void    user_interface_run              (UserInterface *ui);

void    user_interface_draw             (UserInterface *ui);

void    user_interface_update_status    (UserInterface *ui,
                     const gchar *s,
                     glong energy,
                     glong score,
                     glong shields);

void    user_interface_move_robot       (UserInterface *ui,
                     gint from_x,
                     gint from_y,
                     gint to_x,
                     gint to_y,
                     gint cdir,
                     glong energy,
                     glong score,
                     glong shields);

void    user_interface_robot_smell      (UserInterface *ui,
                     gint x,
                     gint y,
                     gint cdir,
                     glong energy,
                     glong score,
                     glong shields);

void    user_interface_robot_zap        (UserInterface *ui,
                     gint x,
                     gint y,
                     gint cdir,
                     gint x_to,
                     gint y_to,
                     glong energy,
                     glong score,
                     glong shields);

void    user_interface_robot_feel       (UserInterface *ui,
                     gint x,
                     gint y,
                     gint cdir,
                     gint x_to,
                     gint y_to,
                     glong energy,
                     glong score,
                     glong shields);

void    user_interface_robot_grab       (UserInterface *ui,
                     gint x,
                     gint y,
                     gint cdir,
                     gint x_to,
                     gint y_to,
                     glong energy,
                     glong score,
                     glong shields);

void    user_interface_robot_look       (UserInterface *ui,
                     gint x,
                     gint y,
                     gint cdir,
                     gint x_to,
                     gint y_to,
                     glong energy,
                     glong score,
                     glong shields);

void    user_interface_get_string       (UserInterface *ui,
                     gchar *prompt,
                     gchar *buff,
                     gint len);

/* SYMBOLIC CONSTANTS */
#define dist(f_x, f_y, t_x, t_y) (abs((f_x)-(t_x))+abs((f_y)-(t_y)))

G_END_DECLS

#endif /* __USER_INTERFACE_H__ */
