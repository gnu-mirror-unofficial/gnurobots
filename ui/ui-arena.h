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

GtkWidget *ui_arena_new(void);

G_END_DECLS

#endif
