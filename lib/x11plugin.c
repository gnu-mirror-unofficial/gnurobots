/* $Id: x11plugin.c,v 1.1 2004/10/21 19:24:30 zeenix Exp $ */

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
#include "x11plugin.h"

enum
{
  ARG_0,
  ARG_MAP 
};

GType _x11_plugin_type = 0;
static GType _parent_type = 0;

static void x11_plugin_class_init (X11PluginClass * klass);
static void x11_plugin_init (GObject * object);
static void x11_plugin_interface_init (gpointer g_iface, gpointer iface_data);

static GObject * x11_plugin_constructor (GType type, 
    guint n_construct_properties, 
    GObjectConstructParam *construct_properties);

static void x11_plugin_finalize (GObject * object);
static void x11_plugin_dispose (GObject * object);

static void x11_plugin_set_property (GObject * object, guint prop_id,
    const GValue * value, GParamSpec * pspec);

static void x11_plugin_get_property (GObject * object, guint prop_id,
    GValue * value, GParamSpec * pspec);

static void put_tile (X11Plugin *x11, XImage *image, gint x, gint y);
static void put_winbuf (X11Plugin *x11);
static void setup_winbuf (X11Plugin *x11);
void create_image (X11Plugin *x11, gchar **data, XImage ** image);
inline void x11_plugin_update_status (X11Plugin *x11,
				   const gchar *s, 
				   glong energy, 
				   glong score, 
				   glong shields);

static GObjectClass *parent_class = NULL;

GType
x11_plugin_get_type (void)
{
  if (!_x11_plugin_type) {
    static const GTypeInfo object_info = {
      sizeof (X11PluginClass),
      NULL,
      NULL,
      (GClassInitFunc) x11_plugin_class_init,
      NULL,
      NULL,
      sizeof (X11Plugin),
      0,
      (GInstanceInitFunc) x11_plugin_init,
      NULL
    };

    static const GInterfaceInfo interface_info = {
      (GInterfaceInitFunc) x11_plugin_interface_init,
      NULL,
      NULL
    };

    _x11_plugin_type =
        g_type_register_static (G_TYPE_OBJECT, 
			"X11Plugin", 
			&object_info,
			0);
    
    g_type_add_interface_static (_x11_plugin_type,
                                 _parent_type,
                                 &interface_info);
  }

  return _x11_plugin_type;
}

static void
x11_plugin_class_init (X11PluginClass * klass)
{
  GObjectClass *gobject_class;

  gobject_class = (GObjectClass *) klass;

  parent_class = g_type_class_ref (G_TYPE_OBJECT);

  gobject_class->constructor = x11_plugin_constructor;
  gobject_class->set_property = x11_plugin_set_property;
  gobject_class->get_property = x11_plugin_get_property;
  gobject_class->dispose = x11_plugin_dispose;
  gobject_class->finalize = x11_plugin_finalize;

  g_object_class_override_property (gobject_class, ARG_MAP, "map");
}

static void
x11_plugin_init (GObject * object)
{
  X11Plugin *x11 = X11_PLUGIN (object);

  x11->map = NULL;
  x11->map_size = NULL;
}

static GObject *
x11_plugin_constructor (GType type, 
    guint n_construct_properties, 
    GObjectConstructParam *construct_properties)
{
  /* Initialize X11 */
  GObject *object;
  X11Plugin *x11;
#include "xpm/statusbar.xpm"
#include "xpm/space.xpm"
#include "xpm/food.xpm"
#include "xpm/wall.xpm"
#include "xpm/prize.xpm"
#include "xpm/baddie.xpm"
#include "xpm/robot_north.xpm"
#include "xpm/robot_east.xpm"
#include "xpm/robot_south.xpm"
#include "xpm/robot_west.xpm"
#include "xpm/robot.xpm"
  Atom prots[6];
  XClassHint classhint;
  XWMHints wmhints;
  XGCValues values;
  Atom delete_win;
  gint x;

  /* Chain up to the parent first */
  object = parent_class->constructor (type, n_construct_properties, construct_properties);
  
  x11 = X11_PLUGIN (object);

  if ((x11->dpy = XOpenDisplay ("")) == NULL) {
    g_printf ("Couldn't open the X Server Display!\n");
    exit (1);                   /* Exit nicely isn't needed yet, and causes segfault */
  }

  delete_win = XInternAtom (x11->dpy, "WM_DELETE_WINDOW", False);

  x11->win_width = x11->map_size->num_cols * TILE_SIZE;
  x11->win_height = x11->map_size->num_rows * TILE_SIZE + 32;
  x11->x_win = XCreateSimpleWindow (x11->dpy, DefaultRootWindow (x11->dpy), 0, 0,
      x11->win_width, x11->win_height, 0, 0, 0);

  prots[0] = delete_win;
  XSetWMProtocols (x11->dpy, x11->x_win, prots, 1);

  XStoreName (x11->dpy, x11->x_win, "GNU Robots");

  classhint.res_name = "robots";
  classhint.res_class = "Robots";
  XSetClassHint (x11->dpy, x11->x_win, &classhint);

  /* XSetCommand() seems to segfault... */

  wmhints.input = True;
  wmhints.flags = InputHint;
  XSetWMHints (x11->dpy, x11->x_win, &wmhints);

  XSelectInput (x11->dpy, x11->x_win,
      KeyPressMask | KeyReleaseMask | StructureNotifyMask | FocusChangeMask);

  XMapWindow (x11->dpy, x11->x_win);

  x11->text = XLoadFont (x11->dpy, "-*-helvetica-medium-r-*-*-*-120-*-*-*-*-*-*");
  values.font = x11->text;
  values.foreground = WhitePixel (x11->dpy, DefaultScreen (x11->dpy));

  x11->gc = XCreateGC (x11->dpy, x11->x_win, GCFont | GCForeground, &values);

  create_image (x11, statusbar_xpm, &x11->statusbar);
  create_image (x11, space_xpm, &x11->space);
  create_image (x11, food_xpm, &x11->food);
  create_image (x11, wall_xpm, &x11->wall);
  create_image (x11, prize_xpm, &x11->prize);
  create_image (x11, baddie_xpm, &x11->baddie);
  create_image (x11, robot_north_xpm, &x11->robotDirs[0]);
  create_image (x11, robot_east_xpm, &x11->robotDirs[1]);
  create_image (x11, robot_south_xpm, &x11->robotDirs[2]);
  create_image (x11, robot_west_xpm, &x11->robotDirs[3]);
  create_image (x11, robot_xpm, &x11->robotPix);

  setup_winbuf (x11);

  //update_status ("Welcome to GNU Robots");
  x11->errors = 0;

  return object;
}

static void 
x11_plugin_dispose (GObject * object)
{
  X11Plugin *x11 = X11_PLUGIN (object);
 
  if (x11->map != NULL) {
    g_object_unref (G_OBJECT (x11->map));

    if (x11->map_size != NULL) {
      g_free (x11->map_size);
    }
  }

  parent_class->dispose (object);
}

/* finalize is called when the object has to free its resources */
static void
x11_plugin_finalize (GObject * object)
{
  X11Plugin *x11 = X11_PLUGIN (object);

  /* End X11 mode */
#ifdef USE_MITSHM
  if (use_mitshm) {
    XShmDetach (x11->dpy, &shm_info);
    if (shm_info.shmaddr)
      shmdt (shm_info.shmaddr);
    if (shm_info.shmid >= 0)
      shmctl (shm_info.shmid, IPC_RMID, 0);
  }
#endif
  XDestroyWindow (x11->dpy, x11->x_win);
  XUnloadFont (x11->dpy, x11->text);

  parent_class->finalize (object);
}

static void
x11_plugin_set_property (GObject * object, guint prop_id,
    const GValue * value, GParamSpec * pspec)
{
  X11Plugin *x11;
  GObject *obj;

  /* it's not null if we got it, but it might not be ours */
  g_return_if_fail (G_IS_X11_PLUGIN (object));

  x11 = X11_PLUGIN (object);

  switch (prop_id) {
    case ARG_MAP:
      obj = g_value_get_object (value);
      g_return_if_fail (obj != NULL);
      
      if (x11->map != NULL) {
	g_object_unref (x11->map);
      }

      x11->map = MAP (g_object_ref (obj));

      if (x11->map_size != NULL) {
        g_free (x11->map_size);
      }

      g_object_get (G_OBJECT (x11->map),
		"size", &x11->map_size,
		NULL);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
  }
}

static void
x11_plugin_get_property (GObject * object, guint prop_id,
    GValue * value, GParamSpec * pspec)
{
  X11Plugin *x11;

  /* it's not null if we got it, but it might not be ours */
  g_return_if_fail (G_IS_X11_PLUGIN (object));

  x11 = X11_PLUGIN (object);

  switch (prop_id) {
    case ARG_MAP:
      g_value_set_object (value, g_object_ref (G_OBJECT (x11->map)));
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

  X11Plugin *x11 =
  	X11_PLUGIN (g_object_new (x11_plugin_get_type (),
				  "map", map,
				  NULL));
  if (x11->errors) {
    g_object_unref (G_OBJECT (x11));
    return NULL;
  }

  return USER_INTERFACE (x11);
}

/* note that hook_delete_thing(x,y) is the same as
   hook_add_thing(x,y,space) */
inline void x11_plugin_add_thing (X11Plugin *x11,
				      gint x, 
				      gint y, 
				      gint thing)
{
  gint w_x, w_y;

  w_x = x * TILE_SIZE;
  w_y = y * TILE_SIZE;

  switch (thing) {
    case SPACE:
      put_tile (x11, x11->space, w_x, w_y);
      break;
    case FOOD:
      put_tile (x11, x11->food, w_x, w_y);
      break;
    case PRIZE:
      put_tile (x11, x11->prize, w_x, w_y);
      break;
    case WALL:
      put_tile (x11, x11->wall, w_x, w_y);
      break;
    case BADDIE:
      put_tile (x11, x11->baddie, w_x, w_y);
      break;
    case ROBOT:
      put_tile (x11, x11->robotPix, w_x, w_y);
      break;
    default:
      put_tile (x11, x11->wall, w_x, w_y);
      break;
  }

  put_winbuf (x11);
  XFlush (x11->dpy);
}

inline void x11_plugin_draw (X11Plugin *x11)
{
  gint i, j;
  
  /* Draw the map for the GNU Robots game. */
  for (j = 0; j < x11->map_size->num_rows; j++) {
    for (i = 0; i < x11->map_size->num_cols; i++) {
      /* Special cases */
      switch (MAP_GET_OBJECT (x11->map, i, j)) {
          /* Add something for the ROBOT?? */
        case '\0':
          put_tile (x11, x11->wall, i * TILE_SIZE, j * TILE_SIZE);
          break;
        case SPACE:
          put_tile (x11, x11->space, i * TILE_SIZE, j * TILE_SIZE);
      	  break;
    	case FOOD:
      	  put_tile (x11, x11->food, i * TILE_SIZE, j * TILE_SIZE);
      	  break;
    	case PRIZE:
      	  put_tile (x11, x11->prize, i * TILE_SIZE, j * TILE_SIZE);
      	  break;
    	case WALL:
      	  put_tile (x11, x11->wall, i * TILE_SIZE, j * TILE_SIZE);
      	  break;
    	case BADDIE:
      	  put_tile (x11, x11->baddie, i * TILE_SIZE, j * TILE_SIZE);
          break;
    	case ROBOT:
      	  put_tile (x11, x11->robotPix, i * TILE_SIZE, j * TILE_SIZE);
      	  break;
    	default:
      	  put_tile (x11, x11->wall, i * TILE_SIZE, j * TILE_SIZE);
      	  break;
      }                         /* switch */
    }                           /* for i */
  }                             /* for j */
  
  put_winbuf (x11);
  XSync (x11->dpy, FALSE);
}

inline void x11_plugin_move_robot (X11Plugin *x11,
				       gint from_x, 
				       gint from_y, 
				       gint to_x, 
				       gint to_y, 
				       gint cdir,
				       glong energy,
				       glong score, 
				       glong shields)
{
  const static gint movement = TILE_SIZE / 16;
  guint8 distance = dist (from_x, from_y, to_x, to_y);
  guint w_x = to_x * TILE_SIZE, w_y = to_y * TILE_SIZE, tw_x, tw_y;
  Bool ok;

  g_assert (distance <= 1);

  x11_plugin_update_status (x11, "robot moves..", energy, score, shields);

  /* Check if robot is moving withing a single box */
  if (distance == 0) {
    put_tile (x11, x11->space, from_x * TILE_SIZE, from_y * TILE_SIZE);
    put_tile (x11, x11->robotDirs[cdir], to_x * TILE_SIZE, to_y * TILE_SIZE);

    put_winbuf (x11);
    XSync (x11->dpy, False);
    g_usleep (USLEEP_TIME / 16);

    return;
  }

  from_x *= TILE_SIZE;
  from_y *= TILE_SIZE;
  tw_y = w_y;
  tw_x = w_x;
  switch (cdir) {
    case NORTH:
      tw_y = from_y - movement;
      break;
    case SOUTH:
      tw_y = from_y + movement;
      break;
    case EAST:
      tw_x = from_x + movement;
      break;
    case WEST:
      tw_x = from_x - movement;
      break;
    default:
      g_printf ("Weird unknown robot direction. I'm Confused.\n");
  }

  while (1) {
    put_tile (x11, x11->space, from_x, from_y);
    put_tile (x11, x11->robotDirs[cdir], tw_x, tw_y);

    ok = False;
    if (tw_x < w_x) {
      tw_x += movement;
      ok = True;
    } else if (tw_x > w_x) {
      tw_x -= movement;
      ok = True;
    }
    if (tw_y < w_y) {
      tw_y += movement;
      ok = True;
    } else if (tw_y > w_y) {
      tw_y -= movement;
      ok = True;
    }
    put_winbuf (x11);
    XSync (x11->dpy, False);
    g_usleep (USLEEP_TIME / 16);
    if (!ok)
      break;
  }

  g_usleep (USLEEP_TIME);
}

/* hooks to animate the robot */
inline void x11_plugin_robot_smell (X11Plugin *x11,
					gint x, 
					gint y, 
					gint cdir,
				        glong energy,
				        glong score, 
				        glong shields)
{
  /* If we want to change the pic, do it here */
  x11_plugin_update_status (x11, "robot sniffs...", energy, score, shields);
  g_usleep (USLEEP_TIME);
}

inline void x11_plugin_robot_zap (X11Plugin *x11,
				      gint x, 
				      gint y, 
				      gint cdir, 
				      gint x_to, 
				      gint y_to,
				      glong energy,
				      glong score, 
				      glong shields)
{
  x11_plugin_update_status (x11, "robot fires his little gun...", energy, score, shields);
  g_usleep (USLEEP_TIME);
}

inline void x11_plugin_robot_feel (X11Plugin *x11,
				       gint x, 
				       gint y, 
				       gint cdir, 
				       gint x_to, 
				       gint y_to,
				       glong energy,
				       glong score, 
				       glong shields)
{
  x11_plugin_update_status (x11, "robot feels for a thing...", energy, score, shields);
  g_usleep (USLEEP_TIME);
}

inline void x11_plugin_robot_grab (X11Plugin *x11,
				       gint x, 
				       gint y, 
				       gint cdir, 
				       gint x_to, 
				       gint y_to,
				       glong energy,
				       glong score, 
				       glong shields)
{
  x11_plugin_update_status (x11, "robot grabs thing...", energy, score, shields);
  g_usleep (USLEEP_TIME);
}

inline void x11_plugin_robot_look (X11Plugin *x11,
				       gint x, 
				       gint y, 
				       gint cdir, 
				       gint x_to, 
				       gint y_to,
				       glong energy,
				       glong score, 
				       glong shields)
{
  x11_plugin_update_status (x11, "robot looks for a thing...", energy, score, shields);
  g_usleep (USLEEP_TIME);
}

/* hooks to get/display data from/to user */
inline void x11_plugin_get_string (X11Plugin *x11,
				       gchar *prompt, 
				       gchar *buff, 
				       gint len)
{
  /* You want me to write a text-box using xlib? 
   * You got to be kidding me
   * */
  fputs (prompt, stdout);
  fgets (buff, len, stdin);
}

inline void x11_update_status (X11Plugin *x11,
				   const gchar *s, 
				   glong energy, 
				   glong score, 
				   glong shields)
{
  gchar status[20];
  gint x = 0;

  while (x < x11->win_width) {
    XPutImage (x11->dpy, x11->win_buf, x11->buf_gc, x11->statusbar, 0, 0, x, x11->map_size->num_rows * TILE_SIZE, 96,
        32);
    x = x + 96;
  }

  XDrawString (x11->dpy, x11->win_buf, x11->gc, 3, x11->map_size->num_rows * TILE_SIZE + 16, s, strlen (s));

  if (energy > -1) {
    g_sprintf (status, "Robot Energy: %3d", energy);
    XDrawString (x11->dpy, x11->win_buf, x11->gc, 240, x11->map_size->num_rows * TILE_SIZE + 12, status,
        strlen (status));
  }

  if (score > -1) {
    g_sprintf (status, "Robot Score: %3d", score);
    XDrawString (x11->dpy, x11->win_buf, x11->gc, 240, x11->map_size->num_rows * TILE_SIZE + 25, status,
        strlen (status));
  }

  if (shields > -1) {
    g_sprintf (status, "Robot Shields: %3d", shields);
    XDrawString (x11->dpy, x11->win_buf, x11->gc, 480, x11->map_size->num_rows * TILE_SIZE + 12, status,
        strlen (status));
  }
}

inline void x11_plugin_update_status (X11Plugin *x11,
				   const gchar *s, 
				   glong energy, 
				   glong score, 
				   glong shields)
{
  gint x;
  XEvent ev;

  x11_update_status (x11, s, energy, score, shields);

  while (XPending (x11->dpy)) {
    XNextEvent (x11->dpy, &ev);

    switch (ev.type) {
      case KeyPress:
      case KeyRelease:
        switch (XKeycodeToKeysym (x11->dpy, ev.xkey.keycode, 0)) {
          case XK_Escape:
            exit (0);
            break;
        }
    }
  }

  put_winbuf (x11);
}

#ifdef USE_MITSHM
inline gint
shm_error_handler (X11Plugin *x11, Display * d, XErrorEvent * e)
{
  x11->use_mitshm = 0;
  return 0;
}
#endif

static void
setup_winbuf (X11Plugin *x11)
{
  gint major, minor;
  Bool shared;
  XVisualInfo *matches;
  XVisualInfo plate;
  gint count;
  guint depth;
  XGCValues values;
  Visual *vis;

  vis = DefaultVisualOfScreen (DefaultScreenOfDisplay (x11->dpy));
  plate.visualid = XVisualIDFromVisual (vis);
  matches = XGetVisualInfo (x11->dpy, VisualIDMask, &plate, &count);
  depth = matches[0].depth;

#ifdef USE_MITSHM
  x11->use_mitshm = 1;
  x11->shm_info.shmid = shmget (IPC_PRIVATE, win_height * win_width * depth,
      IPC_CREAT | 0777);
  if (x11->shm_info.shmid < 0) {
    g_fprintf (stderr, "shmget failed, looks like I'll have to use XPutImage\n");
    x11->use_mitshm = 0;
  } else {
    x11->shm_info.shmaddr = (gchar *) shmat (x11->shm_info.shmid, 0, 0);

    if (!x11->shm_info.shmaddr) {
      g_fprintf (stderr, "shmat failed, looks like I'll have to use XPutImage\n");
      shmctl (x11->shm_info.shmid, IPC_RMID, 0);
      use_mitshm = 0;
    } else {
      XErrorHandler error_handler = XSetErrorHandler (shm_error_handler);

      x11->win_bufi = XShmCreateImage (x11->dpy, vis, depth, ZPixmap, x11->shm_info.shmaddr,
          &x11->shm_info, x11->win_width, x11->win_height);
      x11->shm_info.readOnly = False;
      XShmAttach (x11->dpy, &x11->shm_info);
      win_buf = XShmCreatePixmap (x11->dpy, x11->x_win, x11->shm_info.shmaddr, &x11->shm_info,
          x11->win_width, x11->win_height, depth);
      XSync (x11->dpy, False);
      (void) XSetErrorHandler (error_handler);
      if (!use_mitshm) {
        p_fprintf (stderr,
            "XShmAttach failed, looks like I'll have to use XPutImage\n");
        XFreePixmap (x11->dpy, x11->win_buf);
        XDestroyImage (x11->win_bufi);
        shmdt (x11->shm_info.shmaddr);
        shmctl (x11->shm_info.shmid, IPC_RMID, 0);
      }
    }
  }

  if (!x11->use_mitshm) {
#endif /* USE_MITSHM */
    x11->win_buf = XCreatePixmap (x11->dpy, x11->x_win, x11->win_width, x11->win_height, depth);
#ifdef USE_MITSHM
  } else {
    g_printf ("Using MIT Shared Memory Pixmaps. Good.\n", major, minor);
  }
#endif

  values.font = x11->text;
  values.foreground = WhitePixel (x11->dpy, DefaultScreen (x11->dpy));

  x11->buf_gc = XCreateGC (x11->dpy, x11->win_buf, GCFont | GCForeground, &values);
}

void
create_image (X11Plugin *x11, gchar **data, XImage ** image)
{
  XpmAttributes a;
  gint err;

  a.valuemask = XpmCloseness | XpmAllocCloseColors;
  a.closeness = 40000;          /* the xpm manual suggests 40000 */
  a.alloc_close_colors = 1;     /* allocate the colours chosen */
  err = XpmCreateImageFromData (x11->dpy, data, image, NULL, &a);
  if (err != 0) {
    g_fprintf (stderr, "Cannot create image from xpm: %s\n",
        XpmGetErrorString (err));
    exit (1);
  }
}

static void
put_tile (X11Plugin *x11, XImage *image, gint x, gint y)
{
  XPutImage (x11->dpy, x11->win_buf, x11->gc, image, 0, 0, x, y, TILE_SIZE, TILE_SIZE);
}

static void put_winbuf (X11Plugin *x11)
{
#ifdef USE_MITSHM
  if (use_mitshm)
    XShmPutImage (x11->dpy, x11->x_win, x11->gc, x11->win_bufi, 0, 0, 0, 0, x11->win_width, x11->win_height,
        False);
  else
#endif
    XCopyArea (x11->dpy, x11->win_buf, x11->x_win, x11->gc, 0, 0, x11->win_width, x11->win_height, 0, 0);
  
  XSync (x11->dpy, 0);
}

static void x11_plugin_interface_init (gpointer g_iface, gpointer iface_data)
{
  UserInterfaceClass *klass = (UserInterfaceClass *)g_iface;

  klass->user_interface_add_thing = (void (*) (UserInterface *ui,
				      	       gint x, 
				               gint y, 
				               gint thing)) 
	  			    x11_plugin_add_thing;

  klass->user_interface_draw = (void (*) (UserInterface *ui)) x11_plugin_draw;
  klass->user_interface_update_status = (void (*) (UserInterface *ui,
				  	 	   const gchar *s, 
				  	 	   glong energy,
				  	 	   glong score, 
				  	 	   glong shields)) 
	  				x11_plugin_update_status;
  klass->user_interface_move_robot = (void (*) (UserInterface *ui, 
					 	gint from_x, 
					 	gint from_y, 
					 	gint to_x, 
					 	gint to_y, 
					 	gint cdir,
		 			 	glong energy, 
					 	glong score, 
					 	glong shields))
	  			     x11_plugin_move_robot;
  klass->user_interface_robot_smell = (void (*) (UserInterface *ui, 
					 	 gint x, 
					 	 gint y, 
					 	 gint cdir,
		 			 	 glong energy, 
					 	 glong score, 
					 	 glong shields))
	  			      x11_plugin_robot_smell;
  klass->user_interface_robot_zap = (void (*) (UserInterface *ui,
			  		       gint x, 
					       gint y, 
					       gint cdir, 
			       		       gint x_to, 
					       gint y_to,
		 			       glong energy, 
					       glong score, 
					       glong shields))
	  			    x11_plugin_robot_zap;
  klass->user_interface_robot_feel = (void (*) (UserInterface *ui, 
					 gint x, 
					 gint y, 
					 gint cdir, 
					 gint x_to, 
					 gint y_to,
		 			 glong energy, 
					 glong score, 
					 glong shields))
	  			     x11_plugin_robot_grab;
  klass->user_interface_robot_grab = (void (*) (UserInterface *ui,
				         	gint x, 
				         	gint y, 
				         	gint cdir, 
				         	gint x_to, 
				         	gint y_to,
				         	glong energy,
				         	glong score, 
				         	glong shields))
	  	                     x11_plugin_robot_grab;
  klass->user_interface_robot_look = (void (*) (UserInterface *ui,
					 	gint x, 
					 	gint y, 
					 	gint cdir, 
					 	gint x_to, 
					 	gint y_to,
		 			 	glong energy, 
					 	glong score, 
					 	glong shields))
	  			     x11_plugin_robot_look;
  klass->user_interface_get_string = (void (*) (UserInterface *ui,
					 gchar *prompt, 
					 gchar *buff, 
					 gint len))
	  			     x11_plugin_get_string;
}
