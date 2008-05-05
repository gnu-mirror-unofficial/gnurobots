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
#include <string.h>
#include "configs.h"
#include "userinterface.h"

#include <readline.h>
#include <history.h>

G_DEFINE_TYPE(UserInterface, user_interface, G_TYPE_OBJECT)

enum
{
  ARG_0,
  ARG_MAP
};

static GObject * user_interface_constructor (GType type,
    guint n_construct_properties,
    GObjectConstructParam *construct_properties);

static void user_interface_finalize (GObject * object);
static void user_interface_dispose (GObject * object);

static void user_interface_set_property (GObject * object, guint prop_id,
    const GValue * value, GParamSpec * pspec);

static void user_interface_get_property (GObject * object, guint prop_id,
    GValue * value, GParamSpec * pspec);

static void put_tile (UserInterface *ui, XImage *image, gint x, gint y);
static void put_winbuf (UserInterface *ui);
static void setup_winbuf (UserInterface *ui);
void create_image (UserInterface *ui, gchar **data, XImage ** image);

static GObjectClass *parent_class = NULL;

static void
user_interface_class_init (UserInterfaceClass * klass)
{
  GObjectClass *gobject_class;

  gobject_class = (GObjectClass *) klass;

  parent_class = g_type_class_ref (G_TYPE_OBJECT);

  gobject_class->constructor = user_interface_constructor;
  gobject_class->set_property = user_interface_set_property;
  gobject_class->get_property = user_interface_get_property;
  gobject_class->dispose = user_interface_dispose;
  gobject_class->finalize = user_interface_finalize;

  g_object_class_install_property (G_OBJECT_CLASS (klass), ARG_MAP,
      g_param_spec_object ("map",
            "Map",
            "Reference to the Game Map object",
            G_TYPE_MAP,
            G_PARAM_READWRITE | G_PARAM_CONSTRUCT));
}

static void
user_interface_init (UserInterface* ui)
{
  ui->map = NULL;
  ui->map_size = NULL;
}

static GObject *
user_interface_constructor (GType type,
    guint n_construct_properties,
    GObjectConstructParam *construct_properties)
{
  /* Initialize ui */
  GObject *object;
  UserInterface *ui;
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
  XClassHint classhint;
  XWMHints wmhints;
  XGCValues values;

  /* Chain up to the parent first */
  object = parent_class->constructor (type, n_construct_properties,
          construct_properties);

  ui = USER_INTERFACE (object);

  XInitThreads();

  if ((ui->dpy = XOpenDisplay ("")) == NULL) {
    g_printf ("Couldn't open the X Server Display!\n");
    exit (1);       /* Exit nicely isn't needed yet, and causes segfault */
  }

  ui->wm_delete_win = XInternAtom (ui->dpy, "WM_DELETE_WINDOW", False);

  ui->win_width = ui->map_size->num_cols * TILE_SIZE;
  ui->win_height = ui->map_size->num_rows * TILE_SIZE + 32;
  ui->x_win = XCreateSimpleWindow (ui->dpy, DefaultRootWindow (ui->dpy),
          0, 0, ui->win_width, ui->win_height, 0, 0, 0);

  XSetWMProtocols (ui->dpy, ui->x_win, &(ui->wm_delete_win), 1);

  ui->wm_protocols = XInternAtom(ui->dpy, "WM_PROTOCOLS", False);

  XStoreName (ui->dpy, ui->x_win, "GNU Robots");

  classhint.res_name = "robots";
  classhint.res_class = "Robots";
  XSetClassHint (ui->dpy, ui->x_win, &classhint);

  /* XSetCommand() seems to segfault... */

  wmhints.input = True;
  wmhints.flags = InputHint;
  XSetWMHints (ui->dpy, ui->x_win, &wmhints);

  XSelectInput (ui->dpy, ui->x_win,
      ExposureMask | KeyPressMask | KeyReleaseMask | StructureNotifyMask
      | FocusChangeMask);

  XMapWindow (ui->dpy, ui->x_win);

  ui->text = XLoadFont (ui->dpy,
          "-*-helvetica-medium-r-*-*-*-120-*-*-*-*-*-*");
  values.font = ui->text;
  values.foreground = WhitePixel (ui->dpy, DefaultScreen (ui->dpy));

  ui->gc = XCreateGC (ui->dpy, ui->x_win,
          GCFont | GCForeground, &values);

  create_image (ui, statusbar_xpm, &ui->statusbar);
  create_image (ui, space_xpm, &ui->space);
  create_image (ui, food_xpm, &ui->food);
  create_image (ui, wall_xpm, &ui->wall);
  create_image (ui, prize_xpm, &ui->prize);
  create_image (ui, baddie_xpm, &ui->baddie);
  create_image (ui, robot_north_xpm, &ui->robotDirs[0]);
  create_image (ui, robot_east_xpm, &ui->robotDirs[1]);
  create_image (ui, robot_south_xpm, &ui->robotDirs[2]);
  create_image (ui, robot_west_xpm, &ui->robotDirs[3]);
  create_image (ui, robot_xpm, &ui->robotPix);

  setup_winbuf (ui);

  return object;
}

static void
user_interface_dispose (GObject * object)
{
  UserInterface *ui;
  ui = USER_INTERFACE (object);

  if (ui->map != NULL) {
    g_object_unref (G_OBJECT (ui->map));

    if (ui->map_size != NULL) {
      g_free (ui->map_size);
    }
  }

  parent_class->dispose (object);
}

/* finalize is called when the object has to free its resources */
static void
user_interface_finalize (GObject * object)
{
  UserInterface *ui = USER_INTERFACE (object);

  /* End ui mode */
#ifdef USE_MITSHM
  if (use_mitshm) {
    XShmDetach (ui->dpy, &shm_info);
    if (shm_info.shmaddr)
      shmdt (shm_info.shmaddr);
    if (shm_info.shmid >= 0)
      shmctl (shm_info.shmid, IPC_RMID, 0);
  }
#endif
  XDestroyWindow (ui->dpy, ui->x_win);
  XUnloadFont (ui->dpy, ui->text);

  parent_class->finalize (object);
}

static void
user_interface_set_property (GObject * object, guint prop_id,
    const GValue * value, GParamSpec * pspec)
{
  UserInterface *ui;
  GObject *obj;

  /* it's not null if we got it, but it might not be ours */
  g_return_if_fail (G_IS_USER_INTERFACE (object));

  ui = USER_INTERFACE (object);

  switch (prop_id) {
    case ARG_MAP:
      obj = g_value_get_object (value);
      g_return_if_fail (obj != NULL);

      if (ui->map != NULL) {
	g_object_unref (ui->map);
      }

      ui->map = MAP (g_object_ref (obj));

      if (ui->map_size != NULL) {
        g_free (ui->map_size);
      }

      g_object_get (G_OBJECT (ui->map),
		"size", &ui->map_size,
		NULL);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
  }
}

static void
user_interface_get_property (GObject * object, guint prop_id,
    GValue * value, GParamSpec * pspec)
{
  UserInterface *ui;

  /* it's not null if we got it, but it might not be ours */
  g_return_if_fail (G_IS_USER_INTERFACE (object));

  ui = USER_INTERFACE (object);

  switch (prop_id) {
    case ARG_MAP:
      g_value_set_object (value, g_object_ref (G_OBJECT (ui->map)));
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
  }
}

UserInterface *
user_interface_new (Map *map)
{
  UserInterface *ui;

  g_return_val_if_fail (map != NULL, NULL);

  ui = USER_INTERFACE (g_object_new (user_interface_get_type (),
				  "map", map,
				  NULL));

  return USER_INTERFACE (ui);
}

/* note that hook_delete_thing(x,y) is the same as
   hook_add_thing(x,y,space) */
void user_interface_add_thing (UserInterface *ui,
				      gint x,
				      gint y,
				      gint thing)
{
  gint w_x, w_y;

  w_x = x * TILE_SIZE;
  w_y = y * TILE_SIZE;

  switch (thing) {
    case SPACE:
      put_tile (ui, ui->space, w_x, w_y);
      break;
    case FOOD:
      put_tile (ui, ui->food, w_x, w_y);
      break;
    case PRIZE:
      put_tile (ui, ui->prize, w_x, w_y);
      break;
    case WALL:
      put_tile (ui, ui->wall, w_x, w_y);
      break;
    case BADDIE:
      put_tile (ui, ui->baddie, w_x, w_y);
      break;
    case ROBOT:
      put_tile (ui, ui->robotPix, w_x, w_y);
      break;
    default:
      put_tile (ui, ui->wall, w_x, w_y);
      break;
  }

  put_winbuf (ui);
  XFlush (ui->dpy);
}

void user_interface_draw (UserInterface *ui)
{
  gint i, j;

  /* Draw the map for the GNU Robots game. */
  for (j = 0; j < ui->map_size->num_rows; j++) {
    for (i = 0; i < ui->map_size->num_cols; i++) {
      /* Special cases */
      switch (MAP_GET_OBJECT (ui->map, i, j)) {
          /* Add something for the ROBOT?? */
        case '\0':
          put_tile (ui, ui->wall, i * TILE_SIZE, j * TILE_SIZE);
          break;
        case SPACE:
          put_tile (ui, ui->space, i * TILE_SIZE, j * TILE_SIZE);
      	  break;
    	case FOOD:
      	  put_tile (ui, ui->food, i * TILE_SIZE, j * TILE_SIZE);
      	  break;
    	case PRIZE:
      	  put_tile (ui, ui->prize, i * TILE_SIZE, j * TILE_SIZE);
      	  break;
    	case WALL:
      	  put_tile (ui, ui->wall, i * TILE_SIZE, j * TILE_SIZE);
      	  break;
    	case BADDIE:
      	  put_tile (ui, ui->baddie, i * TILE_SIZE, j * TILE_SIZE);
          break;
    	case ROBOT:
      	  put_tile (ui, ui->robotPix, i * TILE_SIZE, j * TILE_SIZE);
      	  break;
    	default:
      	  put_tile (ui, ui->wall, i * TILE_SIZE, j * TILE_SIZE);
      	  break;
      }                         /* switch */
    }                           /* for i */
  }                             /* for j */

  put_winbuf (ui);
  XSync (ui->dpy, FALSE);
}

void user_interface_move_robot (UserInterface *ui,
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

  user_interface_update_status (ui, "robot moves..", energy, score, shields);

  /* Check if robot is moving withing a single box */
  if (distance == 0) {
    put_tile (ui, ui->space, from_x * TILE_SIZE, from_y * TILE_SIZE);
    put_tile (ui, ui->robotDirs[cdir], to_x * TILE_SIZE, to_y * TILE_SIZE);

    put_winbuf (ui);
    XSync (ui->dpy, False);
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
    put_tile (ui, ui->space, from_x, from_y);
    put_tile (ui, ui->robotDirs[cdir], tw_x, tw_y);

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
    put_winbuf (ui);
    XSync (ui->dpy, False);
    g_usleep (USLEEP_TIME / 16);
    if (!ok)
      break;
  }

  g_usleep (USLEEP_TIME);
}

/* hooks to animate the robot */
void user_interface_robot_smell (UserInterface *ui,
					gint x,
					gint y,
					gint cdir,
				        glong energy,
				        glong score,
				        glong shields)
{
  /* If we want to change the pic, do it here */
  user_interface_update_status (ui, "robot sniffs...", energy, score, shields);
  g_usleep (USLEEP_TIME);
}

void user_interface_robot_zap (UserInterface *ui,
				      gint x,
				      gint y,
				      gint cdir,
				      gint x_to,
				      gint y_to,
				      glong energy,
				      glong score,
				      glong shields)
{
  user_interface_update_status (ui, "robot fires his little gun...", energy, score, shields);
  g_usleep (USLEEP_TIME);
}

void user_interface_robot_feel (UserInterface *ui,
				       gint x,
				       gint y,
				       gint cdir,
				       gint x_to,
				       gint y_to,
				       glong energy,
				       glong score,
				       glong shields)
{
  user_interface_update_status (ui, "robot feels for a thing...", energy, score, shields);
  g_usleep (USLEEP_TIME);
}

void user_interface_robot_grab (UserInterface *ui,
				       gint x,
				       gint y,
				       gint cdir,
				       gint x_to,
				       gint y_to,
				       glong energy,
				       glong score,
				       glong shields)
{
  user_interface_update_status (ui, "robot grabs thing...", energy, score, shields);
  g_usleep (USLEEP_TIME);
}

void user_interface_robot_look (UserInterface *ui,
				       gint x,
				       gint y,
				       gint cdir,
				       gint x_to,
				       gint y_to,
				       glong energy,
				       glong score,
				       glong shields)
{
  user_interface_update_status (ui, "robot looks for a thing...", energy, score, shields);
  g_usleep (USLEEP_TIME);
}

/* hooks to get/display data from/to user */
void user_interface_get_string (UserInterface *ui,
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

void ui_update_status (UserInterface *ui,
				   const gchar *s,
				   glong energy,
				   glong score,
				   glong shields)
{
  gchar status[20];
  gint x = 0;

  while (x < ui->win_width) {
    XPutImage (ui->dpy, ui->win_buf, ui->buf_gc, ui->statusbar, 0, 0, x, ui->map_size->num_rows * TILE_SIZE, 96,
        32);
    x = x + 96;
  }

  XDrawString (ui->dpy, ui->win_buf, ui->gc, 3, ui->map_size->num_rows * TILE_SIZE + 16, s, strlen (s));

  if (energy > -1) {
    g_sprintf (status, "Robot Energy: %3ld", energy);
    XDrawString (ui->dpy, ui->win_buf, ui->gc, 240, ui->map_size->num_rows * TILE_SIZE + 12, status,
        strlen (status));
  }

  if (score > -1) {
    g_sprintf (status, "Robot Score: %3ld", score);
    XDrawString (ui->dpy, ui->win_buf, ui->gc, 240, ui->map_size->num_rows * TILE_SIZE + 25, status,
        strlen (status));
  }

  if (shields > -1) {
    g_sprintf (status, "Robot Shields: %3ld", shields);
    XDrawString (ui->dpy, ui->win_buf, ui->gc, 480, ui->map_size->num_rows * TILE_SIZE + 12, status,
        strlen (status));
  }
}

void user_interface_update_status (UserInterface *ui,
				   const gchar *s,
				   glong energy,
				   glong score,
				   glong shields)
{
  ui_update_status (ui, s, energy, score, shields);
}

void user_interface_run(UserInterface* ui)
{
  XEvent ev;
  XClientMessageEvent* evt;

  while (TRUE)
  {
    XNextEvent (ui->dpy, &ev);

    switch (ev.type)
    {
      case KeyPress:
      case KeyRelease:
        switch (XKeycodeToKeysym (ui->dpy, ev.xkey.keycode, 0))
        {
          case XK_Escape:
            exit (0);
            break;
        }
      case ClientMessage:
        evt = (XClientMessageEvent*)&ev;
        if (evt->message_type == ui->wm_protocols
                && evt->data.l[0] == ui->wm_delete_win)
        {
          g_printf("Exited\n");
          exit(0);
        }
        break;
      case Expose:
        user_interface_draw(ui);
        break;
    }
  }

  put_winbuf (ui);
}

#ifdef USE_MITSHM
static gint
shm_error_handler (UserInterface *ui, Display * d, XErrorEvent * e)
{
  ui->use_mitshm = 0;
  return 0;
}
#endif

static void
setup_winbuf (UserInterface *ui)
{
  XVisualInfo *matches;
  XVisualInfo plate;
  gint count;
  guint depth;
  XGCValues values;
  Visual *vis;

  vis = DefaultVisualOfScreen (DefaultScreenOfDisplay (ui->dpy));
  plate.visualid = XVisualIDFromVisual (vis);
  matches = XGetVisualInfo (ui->dpy, VisualIDMask, &plate, &count);
  depth = matches[0].depth;

#ifdef USE_MITSHM
  ui->use_mitshm = 1;
  ui->shm_info.shmid = shmget (IPC_PRIVATE, win_height * win_width * depth,
      IPC_CREAT | 0777);
  if (ui->shm_info.shmid < 0) {
    g_fprintf (stderr, "shmget failed, looks like I'll have to use XPutImage\n");
    ui->use_mitshm = 0;
  } else {
    ui->shm_info.shmaddr = (gchar *) shmat (ui->shm_info.shmid, 0, 0);

    if (!ui->shm_info.shmaddr) {
      g_fprintf (stderr, "shmat failed, looks like I'll have to use XPutImage\n");
      shmctl (ui->shm_info.shmid, IPC_RMID, 0);
      use_mitshm = 0;
    } else {
      XErrorHandler error_handler = XSetErrorHandler (shm_error_handler);

      ui->win_bufi = XShmCreateImage (ui->dpy, vis, depth, ZPixmap, ui->shm_info.shmaddr,
          &ui->shm_info, ui->win_width, ui->win_height);
      ui->shm_info.readOnly = False;
      XShmAttach (ui->dpy, &ui->shm_info);
      win_buf = XShmCreatePixmap (ui->dpy, ui->x_win, ui->shm_info.shmaddr, &ui->shm_info,
          ui->win_width, ui->win_height, depth);
      XSync (ui->dpy, False);
      (void) XSetErrorHandler (error_handler);
      if (!use_mitshm) {
        p_fprintf (stderr,
            "XShmAttach failed, looks like I'll have to use XPutImage\n");
        XFreePixmap (ui->dpy, ui->win_buf);
        XDestroyImage (ui->win_bufi);
        shmdt (ui->shm_info.shmaddr);
        shmctl (ui->shm_info.shmid, IPC_RMID, 0);
      }
    }
  }

  if (!ui->use_mitshm) {
#endif /* USE_MITSHM */
    ui->win_buf = XCreatePixmap (ui->dpy, ui->x_win, ui->win_width, ui->win_height, depth);
#ifdef USE_MITSHM
  } else {
    g_printf ("Using MIT Shared Memory Pixmaps. Good.\n", major, minor);
  }
#endif

  values.font = ui->text;
  values.foreground = WhitePixel (ui->dpy, DefaultScreen (ui->dpy));

  ui->buf_gc = XCreateGC (ui->dpy, ui->win_buf, GCFont | GCForeground, &values);
}

void
create_image (UserInterface *ui, gchar **data, XImage ** image)
{
  XpmAttributes a;
  gint err;

  a.valuemask = XpmCloseness | XpmAllocCloseColors;
  a.closeness = 40000;          /* the xpm manual suggests 40000 */
  a.alloc_close_colors = 1;     /* allocate the colours chosen */
  err = XpmCreateImageFromData (ui->dpy, data, image, NULL, &a);
  if (err != 0) {
    g_fprintf (stderr, "Cannot create image from xpm: %s\n",
        XpmGetErrorString (err));
    exit (1);
  }
}

static void
put_tile (UserInterface *ui, XImage *image, gint x, gint y)
{
  XPutImage (ui->dpy, ui->win_buf, ui->gc, image, 0, 0, x, y, TILE_SIZE, TILE_SIZE);
}

static void put_winbuf (UserInterface *ui)
{
#ifdef USE_MITSHM
  if (use_mitshm)
    XShmPutImage (ui->dpy, ui->x_win, ui->gc, ui->win_bufi, 0, 0, 0, 0, ui->win_width, ui->win_height,
        False);
  else
#endif
    XCopyArea (ui->dpy, ui->win_buf, ui->x_win, ui->gc, 0, 0, ui->win_width, ui->win_height, 0, 0);

  XSync (ui->dpy, 0);
}
