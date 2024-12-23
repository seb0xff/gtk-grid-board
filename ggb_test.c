#include "ggb.h"
#include <gtk/gtk.h>

typedef struct _Data {
  GtkApplication *app;
  GtkWindow *window;
} Data;

static void click_cb(GtkGestureClick *gesture, guint n_press, gdouble x,
                     gdouble y, gpointer user_data) {
  GgbGrid *grid =
      GGB_GRID(gtk_event_controller_get_widget(GTK_EVENT_CONTROLLER(gesture)));
  // g_print("Click at %f, %f\n", x, y);
  int xi, yi;
  ggb_grid_position_to_index(grid, x, y, &xi, &yi);
  ggb_grid_toggle_at(grid, xi, yi, TRUE);
}

static void activate_cb(GtkApplication *app, gpointer user_data) {
  Data *data = (Data *)user_data;
  data->window = (GtkWindow *)gtk_application_window_new(app);
  gtk_window_set_title(GTK_WINDOW(data->window), "Test Window");
  gtk_window_set_default_size(GTK_WINDOW(data->window), 800, 800);

  GgbGrid *grid = ggb_grid_new();
  gtk_widget_set_hexpand(GTK_WIDGET(grid), FALSE);
  gtk_widget_set_vexpand(GTK_WIDGET(grid), FALSE);
  ggb_grid_set_size(grid, 50, 50);
  ggb_grid_set_accent_guideline_repeat(grid, 10);
  // ggb_grid_set_draw_guidelines(grid, FALSE);
  ggb_grid_set_guideline_width(grid, 0.15);
  ggb_grid_set_cell_spacing(grid, 2);
  ggb_grid_set_cell_radius(grid, 3);

  ggb_grid_toggle_at(grid, 10, 10, TRUE);
  ggb_grid_toggle_at(grid, 10, 11, TRUE);
  ggb_grid_toggle_at(grid, 11, 10, TRUE);
  ggb_grid_toggle_at(grid, 11, 11, TRUE);

  GtkGesture *evk = gtk_gesture_click_new();
  g_signal_connect(evk, "pressed", G_CALLBACK(click_cb), NULL);
  gtk_widget_add_controller(GTK_WIDGET(grid), GTK_EVENT_CONTROLLER(evk));
  gtk_window_set_child(data->window, GTK_WIDGET(grid));
  gtk_window_present(GTK_WINDOW(data->window));
}

int main(int argc, char *argv[]) {
  gtk_init();
  g_setenv("GTK_DEBUG", "interactive", TRUE);
  GtkSettings *settings = gtk_settings_get_default();
  g_object_set(settings, "gtk-application-prefer-dark-theme", TRUE, NULL);

  Data data;
  data.app =
      gtk_application_new("gridboard.test.app", G_APPLICATION_DEFAULT_FLAGS);
  g_signal_connect(data.app, "activate", G_CALLBACK(activate_cb), &data);
  g_application_run(G_APPLICATION(data.app), 0, NULL);
  g_object_unref(data.app);
  return 0;
}