/* Copyright (C) 2008 Bradley Smith <brad@brad-smith.co.uk>
 *
 * GNU Robots, ui-window.h.
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

#ifndef __UI_WINDOW_H__
#define __UI_WINDOW_H__

#include "map.h"

#include <gtk/gtkwindow.h>

G_BEGIN_DECLS

#define UI_TYPE_WINDOW \
    ui_window_get_type()
#define UI_WINDOW(obj) \
    G_TYPE_CHECK_INSTANCE_CAST(obj, UI_TYPE_WINDOW, UIWindow)
#define UI_WINDOW_CLASS(klass) \
    G_TYPE_CHECK_CLASS_CAST(klass, UI_WINDOW_TYPE, UIWindowClass)
#define IS_UI_WINDOW(obj) \
    G_TYPE_CHECK_INSTANCE_TYPE(obj, UI_TYPE_WINDOW)
#define IS_UI_WINDOW_CLASS(klass) \
    G_TYPE_CHECK_CLASS_TYPE(klass, UI_TYPE_WINDOW)

typedef struct _UIWindow UIWindow;
typedef struct _UIWindowClass UIWindowClass;
typedef struct _UIWindowPrivate UIWindowPrivate;

struct _UIWindow
{
  GtkWindow widget;
  UIWindowPrivate* priv;
};

struct _UIWindowClass
{
  GtkWindowClass parent_class;
};

GType ui_window_get_type(void) G_GNUC_CONST;

GtkWidget *ui_window_new();
void ui_window_postinit(UIWindow *window, Map* map);

G_END_DECLS

#endif /* __UI_WINDOW_H__ */
