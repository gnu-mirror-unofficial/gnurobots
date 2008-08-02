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

struct _UIArenaPrivate
{
	int dummy;
};

#define UI_ARENA_GET_PRIVATE(obj) \
    G_TYPE_INSTANCE_GET_PRIVATE(obj, UI_TYPE_ARENA, UIArenaPrivate)

G_DEFINE_TYPE(UIArena, ui_arena, GTK_TYPE_DRAWING_AREA)

GtkWidget *ui_arena_new(void)
{
	return GTK_WIDGET(g_object_new(UI_TYPE_ARENA, NULL));
}

static void ui_arena_init(UIArena *arena)
{
	gtk_widget_set_size_request(GTK_WIDGET(arena), 40*16,18*16);
}

static void ui_arena_class_init(UIArenaClass *klass)
{
	g_type_class_add_private(G_OBJECT_CLASS(klass),
		sizeof(UIArenaPrivate));
}
