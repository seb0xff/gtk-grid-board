#include "ggb.h"
#include <string.h>

typedef struct _GgbGridPrivate {
  GtkWidget *overlay;
  GtkWidget *guidelines_area;
  GtkWidget *cells_area;
  gboolean **cells;
  double cell_width;
  double cell_height;
  // settable props
  int rows_num;
  int cols_num;
  gboolean draw_guidelines;
  double guideline_width;
  int accent_guideline_repeat;
  double cell_radius;
  double cell_spacing;
} GgbGridPrivate;

enum {
  PROP_0,
  PROP_ROWS_NUM,
  PROP_COLS_NUM,
  PROP_DRAW_GUIDELINES,
  PROP_GUIDELINE_WIDTH,
  PROP_ACCENT_GUIDELINE_REPEAT,
  PROP_CELL_RADIUS,
  PROP_CELL_SPACING,
  NUM_PROPERTIES
};

static GParamSpec *obj_props[NUM_PROPERTIES] = {
    NULL,
};

static void ggb_grid_measure(GtkWidget *widget, GtkOrientation orientation,
                             int for_size, int *minimum, int *natural,
                             int *minimum_baseline, int *natural_baseline);
static void ggb_grid_size_allocate(GtkWidget *widget, int width, int height,
                                   int baseline);
static void ggb_grid_snapshot(GtkWidget *widget, GtkSnapshot *snapshot);
static void ggb_grid_draw_guidelines(GtkDrawingArea *area, cairo_t *cr,
                                     int width, int height, void *data);
static void ggb_grid_draw_cells(GtkDrawingArea *area, cairo_t *cr, int width,
                                int height, void *data);
static void ggb_grid_draw_rect(cairo_t *cr, int x_offset, int y_offset,
                               GgbGrid *self);
static void rounded_rectangle(cairo_t *cr, int x, int y, int w, int h, int r);
static void ggb_grid_dispose(GObject *object);

G_DEFINE_TYPE_WITH_PRIVATE(GgbGrid, ggb_grid, GTK_TYPE_WIDGET);

static void ggb_grid_set_property(GObject *object, guint property_id,
                                  const GValue *value, GParamSpec *pspec) {
  GgbGrid *self = GGB_GRID(object);

  switch (property_id) {
  case PROP_ROWS_NUM:
    ggb_grid_set_rows_num(self, g_value_get_int(value));
    break;
  case PROP_COLS_NUM:
    ggb_grid_set_cols_num(self, g_value_get_int(value));
    break;
  case PROP_DRAW_GUIDELINES:
    ggb_grid_set_draw_guidelines(self, g_value_get_boolean(value));
    break;
  case PROP_GUIDELINE_WIDTH:
    ggb_grid_set_guideline_width(self, g_value_get_double(value));
    break;
  case PROP_ACCENT_GUIDELINE_REPEAT:
    ggb_grid_set_accent_guideline_repeat(self, g_value_get_int(value));
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
  }
}

static void ggb_grid_get_property(GObject *object, guint property_id,
                                  GValue *value, GParamSpec *pspec) {
  GgbGrid *self = GGB_GRID(object);

  switch (property_id) {
  case PROP_ROWS_NUM:
    g_value_set_int(value, ggb_grid_get_rows_num(self));
    break;
  case PROP_COLS_NUM:
    g_value_set_int(value, ggb_grid_get_cols_num(self));
    break;
  case PROP_DRAW_GUIDELINES:
    g_value_set_boolean(value, ggb_grid_get_draw_guidelines(self));
    break;
  case PROP_GUIDELINE_WIDTH:
    g_value_set_double(value, ggb_grid_get_guideline_width(self));
    break;
  case PROP_ACCENT_GUIDELINE_REPEAT:
    g_value_set_int(value, ggb_grid_get_accent_guideline_repeat(self));
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
  }
}

static void ggb_grid_class_init(GgbGridClass *class) {
  GObjectClass *gobject_class = G_OBJECT_CLASS(class);

  gobject_class->set_property = ggb_grid_set_property;
  gobject_class->get_property = ggb_grid_get_property;
  gobject_class->dispose = ggb_grid_dispose;

  // GtkWidgetClass *widget_class = GTK_WIDGET_CLASS(class);
  // widget_class->snapshot = ggb_grid_snapshot;
  GTK_WIDGET_CLASS(gobject_class)->snapshot = ggb_grid_snapshot;

  GTK_WIDGET_CLASS(gobject_class)->measure = ggb_grid_measure;
  GTK_WIDGET_CLASS(gobject_class)->size_allocate = ggb_grid_size_allocate;

  obj_props[PROP_ROWS_NUM] = g_param_spec_int("rows-num", NULL, NULL, 0,
                                              G_MAXINT, 0, G_PARAM_READWRITE);

  obj_props[PROP_COLS_NUM] = g_param_spec_int("cols-num", NULL, NULL, 0,
                                              G_MAXINT, 0, G_PARAM_READWRITE);

  obj_props[PROP_DRAW_GUIDELINES] = g_param_spec_boolean(
      "draw-guidelines", NULL, NULL, FALSE, G_PARAM_READWRITE);

  obj_props[PROP_GUIDELINE_WIDTH] = g_param_spec_double(
      "guideline-width", NULL, NULL, 0.0, G_MAXDOUBLE, 0.0, G_PARAM_READWRITE);

  obj_props[PROP_ACCENT_GUIDELINE_REPEAT] =
      g_param_spec_int("guideline-accent-spacing", NULL, NULL, 0, G_MAXINT, 0,
                       G_PARAM_READWRITE);

  obj_props[PROP_CELL_RADIUS] = g_param_spec_double(
      "cell-radius", NULL, NULL, 0.0, G_MAXDOUBLE, 0.0, G_PARAM_READWRITE);

  obj_props[PROP_CELL_SPACING] = g_param_spec_double(
      "cell-spacing", NULL, NULL, 0.0, G_MAXDOUBLE, 0.0, G_PARAM_READWRITE);

  g_object_class_install_properties(gobject_class, NUM_PROPERTIES, obj_props);
}

static void ggb_grid_init(GgbGrid *self) {
  GgbGridPrivate *priv = ggb_grid_get_instance_private(self);
  memset(priv, 0, sizeof(GgbGridPrivate));
  ggb_grid_set_size(self, 0, 0);
  ggb_grid_set_draw_guidelines(self, TRUE);
  ggb_grid_set_guideline_width(self, 0.3);
  ggb_grid_set_accent_guideline_repeat(self, 0);
  ggb_grid_set_cell_radius(self, 0.0);
  ggb_grid_set_cell_spacing(self, 1.0);

  static gboolean css_loaded = FALSE;
  if (!css_loaded) {
    GtkCssProvider *css_provider = gtk_css_provider_new();
    gtk_css_provider_load_from_string(
        css_provider, ".grid-board { color: rgb(150, 150, 150); }");
    gtk_style_context_add_provider_for_display(
        gdk_display_get_default(), GTK_STYLE_PROVIDER(css_provider),
        GTK_STYLE_PROVIDER_PRIORITY_THEME);
    g_object_unref(css_provider);
    css_loaded = TRUE;
  }

  gtk_widget_add_css_class(GTK_WIDGET(self), "grid-board");
  priv->overlay = gtk_overlay_new();
  gtk_widget_set_parent(priv->overlay, GTK_WIDGET(self));

  priv->guidelines_area = gtk_drawing_area_new();
  gtk_widget_set_name(priv->guidelines_area, "guidelines");
  gtk_drawing_area_set_draw_func((GtkDrawingArea *)priv->guidelines_area,
                                 ggb_grid_draw_guidelines, self, NULL);
  gtk_overlay_add_overlay(GTK_OVERLAY(priv->overlay), priv->guidelines_area);

  priv->cells_area = gtk_drawing_area_new();
  gtk_widget_set_name(priv->cells_area, "cells");
  gtk_drawing_area_set_draw_func((GtkDrawingArea *)priv->cells_area,
                                 ggb_grid_draw_cells, self, NULL);
  gtk_overlay_add_overlay(GTK_OVERLAY(priv->overlay), priv->cells_area);
}

static void ggb_grid_dispose(GObject *object) {
  GgbGrid *self = GGB_GRID(object);
  GgbGridPrivate *priv = ggb_grid_get_instance_private(self);

  g_clear_pointer(&priv->overlay, gtk_widget_unparent);
  priv->guidelines_area = NULL;
  priv->cells_area = NULL;

  for (int i = 0; i < priv->cols_num; i++) {
    g_free(priv->cells[i]);
  }
  g_free(priv->cells);
  priv->cells = NULL;

  G_OBJECT_CLASS(ggb_grid_parent_class)->dispose(object);
}

GgbGrid *ggb_grid_new(void) {
  GgbGrid *self = g_object_new(ggb_grid_get_type(), NULL);
  return self;
}

GgbGrid *ggb_grid_new_with_size(int cols_num, int rows_num) {
  GgbGrid *self = g_object_new(ggb_grid_get_type(), NULL);
  ggb_grid_set_size(self, rows_num, cols_num);
  return self;
}

static void ggb_grid_measure(GtkWidget *widget, GtkOrientation orientation,
                             int for_size, int *minimum, int *natural,
                             int *minimum_baseline, int *natural_baseline) {
  GgbGrid *self = GGB_GRID(widget);
  GgbGridPrivate *priv = ggb_grid_get_instance_private(GGB_GRID(self));
  gtk_widget_measure(priv->overlay, orientation, for_size, minimum, natural,
                     minimum_baseline, natural_baseline);
}

static void ggb_grid_size_allocate(GtkWidget *widget, int width, int height,
                                   int baseline) {
  GgbGrid *self = GGB_GRID(widget);
  GgbGridPrivate *priv = ggb_grid_get_instance_private(GGB_GRID(self));
  gtk_widget_size_allocate(priv->overlay, &(GtkAllocation){0, 0, width, height},
                           baseline);
  priv->cell_width = width / (double)priv->cols_num;
  priv->cell_height = height / (double)priv->rows_num;
}

static void ggb_grid_snapshot(GtkWidget *widget, GtkSnapshot *snapshot) {
  GgbGrid *self = GGB_GRID(widget);
  GgbGridPrivate *priv = ggb_grid_get_instance_private(self);
  gtk_widget_snapshot_child(widget, priv->overlay, snapshot);
  gtk_widget_queue_draw(priv->cells_area);
  gtk_widget_queue_draw(priv->guidelines_area);
  gtk_widget_queue_draw(priv->overlay);
  gtk_widget_queue_draw(GTK_WIDGET(self));
}

static void ggb_grid_draw_guidelines(GtkDrawingArea *area, cairo_t *cr,
                                     int width, int height, void *data) {
  GdkRGBA color;
  GgbGridPrivate *priv = ggb_grid_get_instance_private(GGB_GRID(data));
  if (!priv->draw_guidelines) {
    return;
  }
  gtk_widget_get_color(GTK_WIDGET(area), &color);
  cairo_set_source_rgba(cr, color.red, color.green, color.blue, color.alpha);

  for (double x = 0; x < width; x += priv->cell_width) {
    if (priv->accent_guideline_repeat &&
        fmod(x, priv->cell_width * priv->accent_guideline_repeat) == 0) {
      cairo_set_line_width(cr, priv->guideline_width * 2);
    } else {
      cairo_set_line_width(cr, priv->guideline_width);
    }
    cairo_move_to(cr, x, 0);
    cairo_line_to(cr, x, height);
    cairo_stroke(cr);
  }

  for (double y = 0; y < height; y += priv->cell_height) {
    if (priv->accent_guideline_repeat &&
        fmod(y, priv->cell_height * priv->accent_guideline_repeat) == 0) {
      cairo_set_line_width(cr, priv->guideline_width * 2);
    } else {
      cairo_set_line_width(cr, priv->guideline_width);
    }
    cairo_move_to(cr, 0, y);
    cairo_line_to(cr, width, y);
    cairo_stroke(cr);
  }
}

static void ggb_grid_draw_cells(GtkDrawingArea *area, cairo_t *cr, int width,
                                int height, void *data) {
  GgbGrid *self = GGB_GRID(data);
  GgbGridPrivate *priv = ggb_grid_get_instance_private(self);
  for (int x = 0; x < priv->cols_num; x++) {
    for (int y = 0; y < priv->rows_num; y++) {
      if (priv->cells[x][y]) {
        ggb_grid_draw_rect(cr, x, y, self);
      }
    }
  }
}

static void ggb_grid_draw_rect(cairo_t *cr, int x_offset, int y_offset,
                               GgbGrid *self) {
  GdkRGBA color;
  GgbGridPrivate *priv = ggb_grid_get_instance_private(self);
  gtk_widget_get_color((GtkWidget *)self, &color);
  cairo_set_source_rgba(cr, color.red, color.green, color.blue, color.alpha);
  double space = priv->cell_spacing / 2;
  double r = priv->cell_radius;
  double x = x_offset * priv->cell_width + space;
  double y = y_offset * priv->cell_height + space;
  double w = priv->cell_width - space * 2;
  double h = priv->cell_height - space * 2;
  // cairo_rectangle(cr, x, y, w, h);
  rounded_rectangle(cr, x, y, w, h, r);
  cairo_fill(cr);
  cairo_stroke(cr);
}

static void rounded_rectangle(cairo_t *cr, int x, int y, int w, int h, int r) {
  cairo_new_sub_path(cr);
  cairo_arc(cr, x + r, y + r, r, M_PI, 3 * M_PI / 2);
  cairo_arc(cr, x + w - r, y + r, r, 3 * M_PI / 2, 2 * M_PI);
  cairo_arc(cr, x + w - r, y + h - r, r, 0, M_PI / 2);
  cairo_arc(cr, x + r, y + h - r, r, M_PI / 2, M_PI);
  cairo_close_path(cr);
}

void ggb_grid_set_size(GgbGrid *self, int cols_num, int rows_num) {
  GgbGridPrivate *priv = ggb_grid_get_instance_private(self);

  if (rows_num == priv->rows_num && cols_num == priv->cols_num) {
    return;
  }

  gboolean **new_cells = g_new(gboolean *, cols_num);
  for (int i = 0; i < cols_num; i++) {
    new_cells[i] = g_new(gboolean, rows_num);
    if (i < priv->cols_num) {
      int min_rows = MIN(priv->rows_num, rows_num);
      memcpy(new_cells[i], priv->cells[i], min_rows * sizeof(gboolean));
      if (rows_num > priv->rows_num) {
        memset(new_cells[i] + priv->rows_num, FALSE,
               (rows_num - priv->rows_num) * sizeof(gboolean));
      }
    } else {
      memset(new_cells[i], FALSE, rows_num * sizeof(gboolean));
    }
  }

  for (int i = 0; i < priv->rows_num; i++) {
    g_free(priv->cells[i]);
  }
  g_free(priv->cells);

  priv->cells = new_cells;
  priv->cols_num = cols_num;
  priv->rows_num = rows_num;

  ggb_grid_redraw(self);
}

void ggb_grid_set_rows_num(GgbGrid *self, int rows_num) {
  GgbGridPrivate *priv = ggb_grid_get_instance_private(self);
  ggb_grid_set_size(self, priv->cols_num, rows_num);
}

void ggb_grid_set_cols_num(GgbGrid *self, int cols_num) {
  GgbGridPrivate *priv = ggb_grid_get_instance_private(self);
  ggb_grid_set_size(self, cols_num, priv->rows_num);
}

int ggb_grid_get_rows_num(GgbGrid *self) {
  GgbGridPrivate *priv = ggb_grid_get_instance_private(self);
  return priv->rows_num;
}

int ggb_grid_get_cols_num(GgbGrid *self) {
  GgbGridPrivate *priv = ggb_grid_get_instance_private(self);
  return priv->cols_num;
}

void ggb_grid_set_draw_guidelines(GgbGrid *self, gboolean draw_guidelines) {
  GgbGridPrivate *priv = ggb_grid_get_instance_private(self);
  priv->draw_guidelines = draw_guidelines;
  ggb_grid_redraw(self);
}
gboolean ggb_grid_get_draw_guidelines(GgbGrid *self) {
  GgbGridPrivate *priv = ggb_grid_get_instance_private(self);
  return priv->draw_guidelines;
}

void ggb_grid_set_guideline_width(GgbGrid *self, double width) {
  GgbGridPrivate *priv = ggb_grid_get_instance_private(self);
  priv->guideline_width = width;
  ggb_grid_redraw(self);
}
double ggb_grid_get_guideline_width(GgbGrid *self) {
  GgbGridPrivate *priv = ggb_grid_get_instance_private(self);
  return priv->guideline_width;
}

void ggb_grid_set_accent_guideline_repeat(GgbGrid *self, int spacing) {
  GgbGridPrivate *priv = ggb_grid_get_instance_private(self);
  priv->accent_guideline_repeat = spacing;
  ggb_grid_redraw(self);
}
int ggb_grid_get_accent_guideline_repeat(GgbGrid *self) {
  GgbGridPrivate *priv = ggb_grid_get_instance_private(self);
  return priv->accent_guideline_repeat;
}

void ggb_grid_set_cell_radius(GgbGrid *self, double radius) {
  GgbGridPrivate *priv = ggb_grid_get_instance_private(self);
  priv->cell_radius = radius;
  ggb_grid_redraw(self);
}
double ggb_grid_get_cell_radius(GgbGrid *self) {
  GgbGridPrivate *priv = ggb_grid_get_instance_private(self);
  return priv->cell_radius;
}

void ggb_grid_set_cell_spacing(GgbGrid *self, double spacing) {
  GgbGridPrivate *priv = ggb_grid_get_instance_private(self);
  priv->cell_spacing = spacing;
  ggb_grid_redraw(self);
}
double ggb_grid_get_cell_spacing(GgbGrid *self) {
  GgbGridPrivate *priv = ggb_grid_get_instance_private(self);
  return priv->cell_spacing;
}

void ggb_grid_toggle_at(GgbGrid *self, int x, int y, gboolean redraw) {
  GgbGridPrivate *priv = ggb_grid_get_instance_private(self);
  priv->cells[x][y] = !priv->cells[x][y];
  if (redraw) {
    ggb_grid_redraw(self);
  }
}

void ggb_grid_set_at(GgbGrid *self, int x, int y, gboolean value,
                     gboolean redraw) {
  GgbGridPrivate *priv = ggb_grid_get_instance_private(self);
  priv->cells[x][y] = value;
  if (redraw) {
    ggb_grid_redraw(self);
  }
}

gboolean ggb_grid_get_at(GgbGrid *self, int x, int y) {
  GgbGridPrivate *priv = ggb_grid_get_instance_private(self);
  return priv->cells[x][y];
}

void ggb_grid_toggle_all(GgbGrid *self) {
  GgbGridPrivate *priv = ggb_grid_get_instance_private(self);
  for (int x = 0; x < priv->cols_num; x++) {
    for (int y = 0; y < priv->rows_num; y++) {
      ggb_grid_toggle_at(self, x, y, FALSE);
    }
  }
  ggb_grid_redraw(self);
}

void ggb_grid_clear_all(GgbGrid *self) {
  GgbGridPrivate *priv = ggb_grid_get_instance_private(self);
  for (int x = 0; x < priv->cols_num; x++) {
    for (int y = 0; y < priv->rows_num; y++) {
      ggb_grid_set_at(self, x, y, FALSE, FALSE);
    }
  }
  ggb_grid_redraw(self);
}

void ggb_grid_redraw(GgbGrid *self) {
  // GgbGridPrivate *priv = ggb_grid_get_instance_private(self);
  // gtk_widget_queue_draw(priv->cells_area);
  // gtk_widget_queue_draw(priv->overlay);
  // gtk_widget_queue_draw(priv->guidelines_area);
  gtk_widget_queue_draw(GTK_WIDGET(self));
}
