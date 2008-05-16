/* Copyright */

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
