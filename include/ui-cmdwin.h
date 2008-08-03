/* Copyright (C) 2008 Bradley Smith <brad@brad-smith.co.uk>
 *
 * GNU Robots, ui-cmdwin.h
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

#ifndef __UI_CMDWIN_H__
#define __UI_CMDWIN_H__

#include <gtk/gtk.h>

G_BEGIN_DECLS

#define UI_TYPE_CMDWIN \
    ui_cmdwin_get_type()
#define UI_CMDWIN(obj) \
    G_TYPE_CHECK_INSTANCE_CAST(obj, UI_TYPE_CMDWIN, UICmdWin)
#define UI_CMDWIN_CLASS(klass) \
    G_TYPE_CHECK_CLASS_CAST(klass, UI_CMDWIN_TYPE, UICmdWinClass)
#define IS_UI_CMDWIN(obj) \
    G_TYPE_CHECK_INSTANCE_TYPE(obj, UI_TYPE_CMDWIN)
#define IS_UI_CMDWIN_CLASS(klass) \
    G_TYPE_CHECK_CLASS_TYPE(klass, UI_TYPE_CMDWIN)

typedef struct _UICmdWin UICmdWin;
typedef struct _UICmdWinClass UICmdWinClass;
typedef struct _UICmdWinPrivate UICmdWinPrivate;

struct _UICmdWin
{
  GtkVBox widget;
  UICmdWinPrivate* priv;
};

struct _UICmdWinClass
{
  GtkVBoxClass parent_class;
};

GType ui_cmdwin_get_type(void) G_GNUC_CONST;

GtkWidget *ui_cmdwin_new(void);
void ui_cmdwin_get_string(UICmdWin *cmdwin, gchar *prompt, gchar *buf,
	gint len);

G_END_DECLS

#endif
