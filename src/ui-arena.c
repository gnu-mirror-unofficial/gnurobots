/* Copyright (C) 2008 Bradley Smith <brad@brad-smith.co.uk>
 *
 * GNU Robots, ui-arena.c
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

#include "ui-arena.h"

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

#include "map.h"
#include "configs.h"

#include <stdlib.h>

#include <glib/gprintf.h>

#define TILE_SIZE	16

enum
{
	ARG_0,
	ARG_MAP
};

struct _UIArenaPrivate
{
	Map *map;
	MapSize *map_size;

	GdkPixmap *buf;
	GdkFont *font;

	int width;
	int height;

	GdkPixmap *win_bufi;
	GdkPixmap *statusbar;
	GdkPixmap *space;
	GdkPixmap *food;
	GdkPixmap *wall;
	GdkPixmap *prize;
	GdkPixmap *baddie;
	GdkPixmap *robotDirs[4];
	GdkPixmap *robotPix;
};

#define UI_ARENA_GET_PRIVATE(obj) \
    G_TYPE_INSTANCE_GET_PRIVATE(obj, UI_TYPE_ARENA, UIArenaPrivate)

G_DEFINE_TYPE(UIArena, ui_arena, GTK_TYPE_DRAWING_AREA)

static gboolean ui_arena_expose(GtkWidget *widget, GdkEventExpose *event,
	UIArena *arena);
static gboolean ui_arena_configure(GtkWidget *widget,
	GdkEventConfigure *event, UIArena *arena);
static void create_image (UIArena *arena, gchar **data, GdkPixmap **image);
static void put_tile(UIArena *arena, GdkPixmap *image, gint x, gint y);

GtkWidget *ui_arena_new()
{
	return GTK_WIDGET(g_object_new(UI_TYPE_ARENA, NULL));
}

static void ui_arena_init(UIArena *arena)
{
	arena->priv = UI_ARENA_GET_PRIVATE(arena);

	arena->priv->map = NULL;
	arena->priv->map_size = NULL;
	arena->priv->width = 400;
	arena->priv->height = 200;
	arena->priv->font = gdk_font_from_description(
		pango_font_description_from_string("Fixed 18"));

	gtk_widget_set_size_request(GTK_WIDGET(arena), arena->priv->width,
		arena->priv->height);

	g_signal_connect(G_OBJECT(arena), "expose_event",
		G_CALLBACK(ui_arena_expose), arena);
	g_signal_connect(G_OBJECT(arena), "configure_event",
		G_CALLBACK(ui_arena_configure), arena);

	gtk_widget_set_events(GTK_WIDGET(arena),
		GDK_EXPOSURE_MASK | GDK_BUTTON_PRESS_MASK);
}

static void ui_arena_class_init(UIArenaClass *klass)
{
	g_type_class_add_private(G_OBJECT_CLASS(klass),
		sizeof(UIArenaPrivate));
}

static gboolean ui_arena_expose(GtkWidget *widget, GdkEventExpose *event,
	UIArena *arena)
{
	gdk_draw_drawable(widget->window,
		widget->style->fg_gc[GTK_WIDGET_STATE(widget)], arena->priv->buf,
		event->area.x, event->area.y, event->area.x, event->area.y,
		event->area.width, event->area.height);

	if(arena->priv->map)
	{
		gdk_draw_rectangle(widget->window, widget->style->black_gc, FALSE,
			0, 0, widget->allocation.width - 2,
			widget->allocation.height - 1);
	}

	return FALSE;
}

static gboolean ui_arena_configure(GtkWidget *widget,
	GdkEventConfigure *event, UIArena *arena)
{
	if(arena->priv->buf)
		g_object_unref(arena->priv->buf);

	arena->priv->buf = gdk_pixmap_new(widget->window,
		widget->allocation.width - 1, widget->allocation.height, -1);

	gdk_draw_rectangle(arena->priv->buf, widget->style->white_gc, TRUE,
		0, 0, widget->allocation.width, widget->allocation.height);

	return TRUE;
}

void ui_arena_set_map(UIArena *arena, Map *map)
{
	arena->priv->map = map;

	g_object_get(G_OBJECT(arena->priv->map), "size",
		&arena->priv->map_size, NULL);

	arena->priv->width = arena->priv->map_size->num_cols * TILE_SIZE;
	arena->priv->height = arena->priv->map_size->num_rows * TILE_SIZE + 32;

	gtk_widget_set_size_request(GTK_WIDGET(arena), arena->priv->width,
		arena->priv->height);
}

void ui_arena_draw(UIArena *arena)
{
	gint i, j;

	if(!arena->priv->map)
		return;

	for (j = 0; j < arena->priv->map_size->num_rows; j++)
	{
		for (i = 0; i < arena->priv->map_size->num_cols; i++)
		{
			switch(MAP_GET_OBJECT(arena->priv->map, i, j))
			{
			case '\0':
				put_tile(arena, arena->priv->wall, i * TILE_SIZE,
					j * TILE_SIZE);
				break;
			case SPACE:
				put_tile(arena, arena->priv->space, i * TILE_SIZE,
					j * TILE_SIZE);
				break;
			case FOOD:
				put_tile(arena, arena->priv->food, i * TILE_SIZE,
					j * TILE_SIZE);
				break;
			case PRIZE:
				put_tile(arena, arena->priv->prize, i * TILE_SIZE,
					j * TILE_SIZE);
				break;
			case WALL:
				put_tile(arena, arena->priv->wall, i * TILE_SIZE,
					j * TILE_SIZE);
				break;
			case BADDIE:
				put_tile(arena, arena->priv->baddie, i * TILE_SIZE,
					j * TILE_SIZE);
				break;
			case ROBOT:
				put_tile(arena, arena->priv->robotPix, i * TILE_SIZE,
					j * TILE_SIZE);
				break;
			default:
				put_tile(arena, arena->priv->wall, i * TILE_SIZE,
					j * TILE_SIZE);
				break;
			}
		}
	}

	gtk_widget_queue_draw_area(GTK_WIDGET(arena), 0, 0, arena->priv->width,
		arena->priv->height);
}

void ui_arena_postinit(UIArena *arena)
{
	create_image(arena, statusbar_xpm, &arena->priv->statusbar);
	create_image(arena, space_xpm, &arena->priv->space);
	create_image(arena, food_xpm, &arena->priv->food);
	create_image(arena, wall_xpm, &arena->priv->wall);
	create_image(arena, prize_xpm, &arena->priv->prize);
	create_image(arena, baddie_xpm, &arena->priv->baddie);
	create_image(arena, robot_north_xpm, &arena->priv->robotDirs[0]);
	create_image(arena, robot_east_xpm, &arena->priv->robotDirs[1]);
	create_image(arena, robot_south_xpm, &arena->priv->robotDirs[2]);
	create_image(arena, robot_west_xpm, &arena->priv->robotDirs[3]);
	create_image(arena, robot_xpm, &arena->priv->robotPix);
}

static void create_image (UIArena *arena, gchar **data, GdkPixmap **image)
{
	*image = gdk_pixmap_create_from_xpm_d(GDK_DRAWABLE(arena->priv->buf),
		NULL, NULL, data);
	if (*image == NULL)
		g_fprintf (stderr, "Cannot create image from xpm\n");
}

static void put_tile(UIArena *arena, GdkPixmap *image, gint x, gint y)
{
	gdk_draw_drawable(arena->priv->buf,
		GTK_WIDGET(arena)->style->fg_gc[GTK_WIDGET_STATE(arena)],
		GDK_DRAWABLE(image), 0, 0, x, y, TILE_SIZE, TILE_SIZE);
}

void ui_arena_update_status(UIArena *arena, const gchar *s, glong energy,
	glong score, glong shields)
{
	gchar status[20];
	gint x = 0;

	while(x < arena->priv->width)
	{
		gdk_draw_drawable(arena->priv->buf,
			GTK_WIDGET(arena)->style->white_gc,
			arena->priv->statusbar, 0, 0, x,
			arena->priv->map_size->num_rows * TILE_SIZE, 96, 32);
		x += 96;
	}

	gdk_draw_string(arena->priv->buf, arena->priv->font,
		GTK_WIDGET(arena)->style->white_gc, 3,
		arena->priv->map_size->num_rows * TILE_SIZE + 16, s);

	if(energy > -1)
	{
		g_sprintf(status, "Robot Energy: %3ld", energy);
		gdk_draw_string(arena->priv->buf, arena->priv->font,
			GTK_WIDGET(arena)->style->white_gc, 240,
			arena->priv->map_size->num_rows * TILE_SIZE + 12, status);
	}

	if (score > -1)
	{
		g_sprintf (status, "Robot Score: %3ld", score);
		gdk_draw_string(arena->priv->buf, arena->priv->font,
			GTK_WIDGET(arena)->style->white_gc, 240,
			arena->priv->map_size->num_rows * TILE_SIZE + 25, status);
	}

	if (shields > -1)
	{
		g_sprintf (status, "Robot Shields: %3ld", shields);
		gdk_draw_string(arena->priv->buf, arena->priv->font,
			GTK_WIDGET(arena)->style->white_gc, 480,
			arena->priv->map_size->num_rows * TILE_SIZE + 12, status);
	}

	gtk_widget_queue_draw_area(GTK_WIDGET(arena), 0, 0, arena->priv->width,
		arena->priv->height);
}

/* note that hook_delete_thing(x,y) is the same as
 * hook_add_thing(x,y,space) */
void ui_arena_add_thing(UIArena *arena, gint x, gint y, gint thing)
{
	gint w_x, w_y;

	w_x = x * TILE_SIZE;
	w_y = y * TILE_SIZE;

	switch (thing)
	{
	case SPACE:
		put_tile(arena, arena->priv->space, w_x, w_y);
		break;
	case FOOD:
		put_tile(arena, arena->priv->food, w_x, w_y);
		break;
	case PRIZE:
		put_tile(arena, arena->priv->prize, w_x, w_y);
		break;
	case WALL:
		put_tile(arena, arena->priv->wall, w_x, w_y);
		break;
	case BADDIE:
		put_tile(arena, arena->priv->baddie, w_x, w_y);
		break;
	case ROBOT:
		put_tile(arena, arena->priv->robotPix, w_x, w_y);
		break;
	default:
		put_tile(arena, arena->priv->wall, w_x, w_y);
		break;
	}

	gtk_widget_queue_draw_area(GTK_WIDGET(arena), 0, 0, arena->priv->width,
		arena->priv->height);
}

void ui_arena_move_robot(UIArena *arena, gint from_x, gint from_y,
	gint to_x, gint to_y, gint cdir, glong energy, glong score,
	glong shields)
{
	const static gint movement = TILE_SIZE / 16;
	guint8 distance = dist(from_x, from_y, to_x, to_y);
	guint w_x = to_x * TILE_SIZE, w_y = to_y * TILE_SIZE, tw_x, tw_y;
	gboolean ok;

	g_assert(distance <= 1);

	ui_arena_update_status(arena, "Robot moves..", energy, score, shields);

	/* Check if robot is moving withing a single box */
	if (distance == 0)
	{
		put_tile(arena, arena->priv->space, from_x * TILE_SIZE,
			from_y * TILE_SIZE);
		put_tile(arena, arena->priv->robotDirs[cdir], to_x * TILE_SIZE,
			to_y * TILE_SIZE);

		gtk_widget_queue_draw_area(GTK_WIDGET(arena), 0, 0,
			arena->priv->width,	arena->priv->height);

		return;
	}

	from_x *= TILE_SIZE;
	from_y *= TILE_SIZE;
	tw_y = w_y;
	tw_x = w_x;
	switch (cdir)
	{
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
		g_printf("Weird unknown robot direction. I'm Confused.\n");
	}


	while (1)
	{
		put_tile(arena, arena->priv->space, from_x, from_y);
		put_tile(arena, arena->priv->robotDirs[cdir], tw_x, tw_y);

		ok = FALSE;
		if (tw_x < w_x)
		{
			tw_x += movement;
			ok = TRUE;
		}
		else if (tw_x > w_x)
		{
			tw_x -= movement;
			ok = TRUE;
		}
		if (tw_y < w_y)
		{
			tw_y += movement;
			ok = TRUE;
		}
		else if (tw_y > w_y)
		{
			tw_y -= movement;
			ok = TRUE;
		}

		gtk_widget_queue_draw_area(GTK_WIDGET(arena), 0, 0,
			arena->priv->width,	arena->priv->height);
		gdk_window_process_all_updates();

		g_usleep(USLEEP_TIME / 16);

		if (!ok)
			break;
	}

	g_usleep(USLEEP_TIME);
}

/* hooks to animate the robot */
void ui_arena_robot_smell(UIArena *arena, gint x, gint y, gint cdir,
	glong energy, glong score, glong shields)
{
	/* If we want to change the pic, do it here */
	ui_arena_update_status(arena, "Robot sniffs...", energy, score,
		shields);

	g_usleep(USLEEP_TIME);
}

void ui_arena_robot_zap(UIArena *arena,	gint x,	gint y,	gint cdir,
	gint x_to, gint y_to, glong energy, glong score, glong shields)
{
	ui_arena_update_status(arena, "Robot fires his little gun...", energy,
		score, shields);

	g_usleep(USLEEP_TIME);
}

void ui_arena_robot_feel(UIArena *arena, gint x, gint y, gint cdir,
	gint x_to, gint y_to, glong energy, glong score, glong shields)
{
	ui_arena_update_status(arena, "Robot feels for a thing...", energy,
		score, shields);

	g_usleep(USLEEP_TIME);
}

void ui_arena_robot_grab(UIArena *arena, gint x, gint y, gint cdir,
	gint x_to, gint y_to, glong energy, glong score, glong shields)
{
	ui_arena_update_status(arena, "Robot grabs thing...", energy, score,
		shields);

	g_usleep(USLEEP_TIME);
}

void ui_arena_robot_look(UIArena *arena, gint x, gint y, gint cdir,
	gint x_to, gint y_to, glong energy, glong score, glong shields)
{
	ui_arena_update_status(arena, "Robot looks for a thing...", energy,
		score, shields);

	g_usleep(USLEEP_TIME);
}
