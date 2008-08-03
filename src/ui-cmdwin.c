/* Copyright (C) 2008 Bradley Smith <brad@brad-smith.co.uk>
 *
 * GNU Robots, ui-cmdwin.c
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

#include "ui-cmdwin.h"

#include <stdlib.h>
#include <pty.h>

#include <vte/vte.h>
#include <readline.h>
#include <history.h>

struct _UICmdWinPrivate
{
	GtkWidget *vte;
};

#define UI_CMDWIN_GET_PRIVATE(obj) \
    G_TYPE_INSTANCE_GET_PRIVATE(obj, UI_TYPE_CMDWIN, UICmdWinPrivate)

G_DEFINE_TYPE(UICmdWin, ui_cmdwin, GTK_TYPE_VBOX)

GtkWidget *ui_cmdwin_new(void)
{
	return GTK_WIDGET(g_object_new(UI_TYPE_CMDWIN, NULL));
}

static void ui_cmdwin_init(UICmdWin *cmdwin)
{
	int master_pty, slave_pty;
	FILE *in, *out;

	cmdwin->priv = UI_CMDWIN_GET_PRIVATE(cmdwin);

	cmdwin->priv->vte = vte_terminal_new();

	gtk_box_pack_start(GTK_BOX(cmdwin), cmdwin->priv->vte, TRUE, TRUE, 0);

	gtk_widget_set_size_request(GTK_WIDGET(cmdwin), -1, 200);

	g_assert(openpty(&master_pty, &slave_pty, NULL, NULL, NULL) >= 0);
	vte_terminal_set_pty(VTE_TERMINAL(cmdwin->priv->vte), master_pty);
	in = fdopen(slave_pty, "r");
	out = fdopen(slave_pty, "w");

	rl_instream = in;
	rl_outstream = out;

	gtk_widget_show(cmdwin->priv->vte);
}

static void ui_cmdwin_class_init(UICmdWinClass *klass)
{
	g_type_class_add_private(G_OBJECT_CLASS(klass),
		sizeof(UICmdWinPrivate));
}

void ui_cmdwin_get_string(UICmdWin *cmdwin, gchar *prompt, gchar *buf,
	gint len)
{
	char *line = (char *)NULL;

	line = readline(prompt);

	if (line && *line)
	{
		add_history(line);
		g_strlcpy(buf, line, len);
	}

	free(line);
}
