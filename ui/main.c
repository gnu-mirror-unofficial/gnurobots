/* Copyright */

#include <gtk/gtkmain.h>

#include "ui-window.h"

int main(int argc, char* argv[])
{
  GtkWidget* window;

  gtk_init(&argc, &argv);

  window = ui_window_new();

  gtk_main();

  return 0;
}
