/* Copyright (C) 2008 Bradley Smith <brad@brad-smith.co.uk>
 *
 * GNU Robots, ui-arena.h
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

#ifndef __UI_ARENA_H__
#define __UI_ARENA_H__

#include <gtk/gtk.h>

#include "map.h"

G_BEGIN_DECLS

#define UI_TYPE_ARENA \
    ui_arena_get_type()
#define UI_ARENA(obj) \
    G_TYPE_CHECK_INSTANCE_CAST(obj, UI_TYPE_ARENA, UIArena)
#define UI_ARENA_CLASS(klass) \
    G_TYPE_CHECK_CLASS_CAST(klass, UI_ARENA_TYPE, UIArenaClass)
#define IS_UI_ARENA(obj) \
    G_TYPE_CHECK_INSTANCE_TYPE(obj, UI_TYPE_ARENA)
#define IS_UI_ARENA_CLASS(klass) \
    G_TYPE_CHECK_CLASS_TYPE(klass, UI_TYPE_ARENA)

#define dist(f_x, f_y, t_x, t_y) (abs((f_x)-(t_x))+abs((f_y)-(t_y)))

typedef struct _UIArena UIArena;
typedef struct _UIArenaClass UIArenaClass;
typedef struct _UIArenaPrivate UIArenaPrivate;

struct _UIArena
{
  GtkDrawingArea widget;
  UIArenaPrivate* priv;
};

struct _UIArenaClass
{
  GtkDrawingAreaClass parent_class;
};

GType ui_arena_get_type(void) G_GNUC_CONST;

GtkWidget *ui_arena_new();

void ui_arena_set_map(UIArena *arena, Map *map);
void ui_arena_postinit(UIArena *arena);
void ui_arena_draw(UIArena *arena);
void ui_arena_update_status(UIArena *arena, const gchar *s, glong energy,
	glong score, glong shields);

void ui_arena_add_thing(UIArena *arena, gint x, gint y, gint thing);
void ui_arena_move_robot(UIArena *arena, gint from_x, gint from_y,
	gint to_x, gint to_y, gint cdir, glong energy, glong score,
	glong shields);
void ui_arena_robot_smell(UIArena *arena, gint x, gint y, gint cdir,
	glong energy, glong score, glong shields);
void ui_arena_robot_zap(UIArena *arena, gint x, gint y, gint cdir,
	gint x_to, gint y_to, glong energy, glong score, glong shields);
void ui_arena_robot_feel(UIArena *arena, gint x, gint y, gint cdir,
	gint x_to, gint y_to, glong energy, glong score, glong shields);
void ui_arena_robot_grab(UIArena *arena, gint x, gint y, gint cdir,
	gint x_to, gint y_to, glong energy, glong score, glong shields);
void ui_arena_robot_look(UIArena *arena, gint x, gint y, gint cdir,
	gint x_to, gint y_to, glong energy, glong score, glong shields);

G_END_DECLS

#endif
