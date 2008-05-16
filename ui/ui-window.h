/* Copyright */

#ifndef __UI_WINDOW_H__
#define __UI_WINDOW_H__

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

GtkWidget *ui_window_new(void);

G_END_DECLS

#endif /* __UI_WINDOW_H__ */
