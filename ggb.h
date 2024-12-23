#pragma once
#include <gtk/gtk.h>

#ifdef __cplusplus
extern "C" {
#endif

#define GGB_TYPE_SQUARE (ggb_grid_get_type())
#define GGB_GRID(obj)                                                          \
  (G_TYPE_CHECK_INSTANCE_CAST((obj), GGB_TYPE_SQUARE, GgbGrid))
G_DECLARE_DERIVABLE_TYPE(GgbGrid, ggb_grid, GB, SQUARE, GtkWidget)

struct _GgbGridClass {
  GtkWidgetClass parent_class;
};

GgbGrid *ggb_grid_new();
GgbGrid *ggb_grid_new_with_size(int cols_num, int rows_num);

void ggb_grid_set_size(GgbGrid *self, int cols_num, int rows_num);
void ggb_grid_set_cols_num(GgbGrid *self, int rows_num);
void ggb_grid_set_rows_num(GgbGrid *self, int rows_num);
int ggb_grid_get_cols_num(GgbGrid *self);
int ggb_grid_get_rows_num(GgbGrid *self);

void ggb_grid_set_draw_guidelines(GgbGrid *self, gboolean draw_guidelines);
gboolean ggb_grid_get_draw_guidelines(GgbGrid *self);

void ggb_grid_set_guideline_width(GgbGrid *self, double width);
double ggb_grid_get_guideline_width(GgbGrid *self);

void ggb_grid_set_accent_guideline_repeat(GgbGrid *self, int spacing);
int ggb_grid_get_accent_guideline_repeat(GgbGrid *self);

void ggb_grid_set_cell_radius(GgbGrid *self, double radius);
double ggb_grid_get_cell_radius(GgbGrid *self);

void ggb_grid_set_cell_spacing(GgbGrid *self, double spacing);
double ggb_grid_get_cell_spacing(GgbGrid *self);

void ggb_grid_toggle_at(GgbGrid *self, int x, int y, gboolean redraw);
void ggb_grid_set_at(GgbGrid *self, int x, int y, gboolean value,
                     gboolean redraw);
gboolean ggb_grid_get_at(GgbGrid *self, int x, int y);

void ggb_grid_toggle_all(GgbGrid *self);
void ggb_grid_clear_all(GgbGrid *self);

void ggb_grid_redraw(GgbGrid *self);

#ifdef __cplusplus
} // extern "C"
#endif