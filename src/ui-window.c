/* Copyright (C) 2008 Bradley Smith <brad@brad-smith.co.uk>
 *
 * GNU Robots, ui-window.c.
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

#include "ui-window.h"
#include "ui-window-private.h"

#include "ui-cmdwin.h"
#include "ui-arena.h"

#include "main.h"
#include "map.h"
#include "configs.h"

#include <gtk/gtkmain.h>

#define UI_WINDOW_GET_PRIVATE(obj) \
    G_TYPE_INSTANCE_GET_PRIVATE(obj, UI_TYPE_WINDOW, UIWindowPrivate)

G_DEFINE_TYPE(UIWindow, ui_window, GTK_TYPE_WINDOW)

static void on_ui_window_destroy(GtkWidget *widget, gpointer data);

GtkWidget *ui_window_new()
{
	return GTK_WIDGET(g_object_new(UI_TYPE_WINDOW, NULL));
}

static void on_ui_window_destroy(GtkWidget *widget, gpointer data)
{
	gtk_widget_hide(widget);
	gtk_main_quit();
	exit_nicely();
}

static void ui_window_init(UIWindow *window)
{
	window->priv = UI_WINDOW_GET_PRIVATE(window);

	g_signal_connect(G_OBJECT(window), "destroy",
		G_CALLBACK(on_ui_window_destroy), NULL);

	gtk_window_set_title(GTK_WINDOW(window), "GNU Robots");
	gtk_window_set_resizable(GTK_WINDOW(window), FALSE);
}

void ui_window_postinit(UIWindow *window, Map* map)
{
	GtkWidget *vbox;

	vbox = gtk_vbox_new(FALSE, 2);
	window->priv->cmdwin = ui_cmdwin_new();
	window->priv->arena = ui_arena_new();
	ui_arena_set_map(UI_ARENA(window->priv->arena), map);

	/* TODO: Add menu first etc */
	gtk_box_pack_start(GTK_BOX(vbox), window->priv->arena, TRUE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX(vbox), window->priv->cmdwin, TRUE, TRUE, 0);

	gtk_container_add(GTK_CONTAINER(window), vbox);

	gtk_widget_show(window->priv->cmdwin);
	gtk_widget_show(window->priv->arena);
	gtk_widget_show(vbox);
	gtk_widget_show(GTK_WIDGET(window));

	ui_arena_postinit(UI_ARENA(window->priv->arena));
}

static void ui_window_class_init(UIWindowClass *klass)
{
	g_type_class_add_private(G_OBJECT_CLASS(klass),
		sizeof(UIWindowPrivate));
}
