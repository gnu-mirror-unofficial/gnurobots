/* Copyright (C) 2008 Bradley Smith <brad@brad-smith.co.uk>
 *
 * GNU Robots UI Test, ui-window.c.
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

#include <gtk/gtkmain.h>

#define UI_WINDOW_GET_PRIVATE(obj) \
    G_TYPE_INSTANCE_GET_PRIVATE(obj, UI_TYPE_WINDOW, UIWindowPrivate)

G_DEFINE_TYPE(UIWindow, ui_window, GTK_TYPE_WINDOW)

static void on_ui_window_destroy(GtkWidget *widget, gpointer data);

GtkWidget *ui_window_new(void)
{
  return GTK_WIDGET(g_object_new(UI_TYPE_WINDOW, NULL));
}

static void on_ui_window_destroy(GtkWidget *widget, gpointer data)
{
  gtk_widget_hide(widget);
  gtk_main_quit();
}

static void ui_window_init(UIWindow* window)
{
  g_signal_connect(G_OBJECT(window), "destroy",
          G_CALLBACK(on_ui_window_destroy), NULL);

  gtk_window_set_title(GTK_WINDOW(window), "UI Test");
  gtk_widget_show(GTK_WIDGET(window));
}

static void ui_window_class_init(UIWindowClass* klass)
{
  g_type_class_add_private(G_OBJECT_CLASS(klass), sizeof(UIWindowPrivate));
}
