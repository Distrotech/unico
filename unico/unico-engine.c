/* The Unico Theming Engine for Gtk+.
 * Copyright (C) 2011 Canonical Ltd
 *
 * This  library is free  software; you can  redistribute it and/or
 * modify it  under  the terms  of the  GNU Lesser  General  Public
 * License  as published  by the Free  Software  Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed  in the hope that it will be useful,
 * but  WITHOUT ANY WARRANTY; without even  the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License  along  with  this library;  if not,  write to  the Free
 * Software Foundation, Inc., 51  Franklin St, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 *
 * Authored by Andrea Cimitan <andrea.cimitan@canonical.com>
 *
 */

#include <cairo.h>
#include <cairo-gobject.h>
#include <gtk/gtk.h>

#include "unico.h"
#include "unico-cairo-support.h"
#include "unico-draw.h"
#include "unico-engine.h"
#include "unico-support.h"
#include "unico-types.h"

#define UNICO_NAMESPACE "unico"

#define UNICO_CAIRO_INIT \
        cairo_set_line_width (cr, 1.0); \
        cairo_set_line_cap (cr, CAIRO_LINE_CAP_SQUARE); \
        cairo_set_line_join (cr, CAIRO_LINE_JOIN_MITER);

G_DEFINE_DYNAMIC_TYPE (UnicoEngine, unico_engine, GTK_TYPE_THEMING_ENGINE)

static void
unico_engine_render_activity (GtkThemingEngine *engine,
                              cairo_t          *cr,
                              gdouble           x,
                              gdouble           y,
                              gdouble           width,
                              gdouble           height)
{
  UnicoStyleFunctions *style_functions;
  const GtkWidgetPath *path;

  UNICO_CAIRO_INIT

  unico_lookup_functions (UNICO_ENGINE (engine), &style_functions);
  path = gtk_theming_engine_get_path (engine);

  if (gtk_widget_path_is_type (path, GTK_TYPE_SCALE))
    unico_trim_scale_allocation (engine, &x, &y, &width, &height);

  if (gtk_theming_engine_has_class (engine, GTK_STYLE_CLASS_SPINNER))
    GTK_THEMING_ENGINE_CLASS (unico_engine_parent_class)->render_activity (engine, cr, x, y, width, height);
  else
    style_functions->draw_activity (engine, cr, x, y, width, height);
}

static void
unico_engine_render_arrow (GtkThemingEngine *engine,
                           cairo_t          *cr,
                           gdouble           angle,
                           gdouble           x,
                           gdouble           y,
                           gdouble           size)
{
  UnicoStyleFunctions *style_functions;

  UNICO_CAIRO_INIT

  unico_lookup_functions (UNICO_ENGINE (engine), &style_functions);

  style_functions->draw_arrow (engine, cr, angle, x, y, size);
}

static void
unico_engine_render_background (GtkThemingEngine *engine,
                                cairo_t          *cr,
                                gdouble           x,
                                gdouble           y,
                                gdouble           width,
                                gdouble           height)
{
  UnicoStyleFunctions *style_functions;
  const GtkWidgetPath *path;
  GtkRegionFlags flags;

  UNICO_CAIRO_INIT

  unico_lookup_functions (UNICO_ENGINE (engine), &style_functions);
  path = gtk_theming_engine_get_path (engine);

  if (gtk_widget_path_is_type (path, GTK_TYPE_SCALE) &&
      gtk_theming_engine_has_class (engine, GTK_STYLE_CLASS_TROUGH))
    unico_trim_scale_allocation (engine, &x, &y, &width, &height);

  if (gtk_theming_engine_has_class (engine, GTK_STYLE_CLASS_BUTTON) &&
      gtk_theming_engine_has_class (engine, GTK_STYLE_CLASS_SPINBUTTON))
    style_functions->draw_spinbutton_background (engine, cr, x, y, width, height);
  else if (!gtk_widget_path_is_type (path, GTK_TYPE_ICON_VIEW) &&
           gtk_theming_engine_has_class (engine, GTK_STYLE_CLASS_VIEW) &&
           gtk_theming_engine_has_region (engine, GTK_STYLE_REGION_COLUMN, &flags))
    style_functions->draw_cell_background (engine, cr, x, y, width, height, flags);
  else
    style_functions->draw_common_background (engine, cr, x, y, width, height);
}

static void
unico_engine_render_check (GtkThemingEngine *engine,
                           cairo_t          *cr,
                           gdouble           x,
                           gdouble           y,
                           gdouble           width,
                           gdouble           height)
{
  UnicoStyleFunctions *style_functions;

  UNICO_CAIRO_INIT

  unico_lookup_functions (UNICO_ENGINE (engine), &style_functions);

  if (!gtk_theming_engine_has_class (engine, GTK_STYLE_CLASS_MENUITEM))
    {
      if (unico_cairo_draw_from_texture (engine, cr, x, y, width, height))
        return;
    }

  style_functions->draw_check (engine, cr, x, y, width, height);
}

static void
unico_engine_render_expander (GtkThemingEngine *engine,
                              cairo_t          *cr,
                              gdouble           x,
                              gdouble           y,
                              gdouble           width,
                              gdouble           height)
{
  UnicoStyleFunctions *style_functions;

  UNICO_CAIRO_INIT

  unico_lookup_functions (UNICO_ENGINE (engine), &style_functions);

  style_functions->draw_expander (engine, cr, x, y, width, height);
}

static void
unico_engine_render_extension (GtkThemingEngine *engine,
                               cairo_t          *cr,
                               gdouble           x,
                               gdouble           y,
                               gdouble           width,
                               gdouble           height,
                               GtkPositionType   gap_side)
{
  UnicoStyleFunctions *style_functions;

  UNICO_CAIRO_INIT

  unico_lookup_functions (UNICO_ENGINE (engine), &style_functions);

  style_functions->draw_extension (engine, cr, x, y, width, height, gap_side);
}

static void
unico_engine_render_focus (GtkThemingEngine *engine,
                           cairo_t          *cr,
                           gdouble           x,
                           gdouble           y,
                           gdouble           width,
                           gdouble           height)
{
  UnicoStyleFunctions *style_functions;

  UNICO_CAIRO_INIT

  unico_lookup_functions (UNICO_ENGINE (engine), &style_functions);

  style_functions->draw_focus (engine, cr, x, y, width, height);
}

static void
unico_engine_render_frame (GtkThemingEngine *engine,
                           cairo_t          *cr,
                           gdouble           x,
                           gdouble           y,
                           gdouble           width,
                           gdouble           height)
{
  UnicoStyleFunctions *style_functions;
  const GtkWidgetPath *path;
  GtkRegionFlags flags;

  UNICO_CAIRO_INIT

  unico_lookup_functions (UNICO_ENGINE (engine), &style_functions);
  path = gtk_theming_engine_get_path (engine);

  if (gtk_widget_path_is_type (path, GTK_TYPE_SCALE) &&
      gtk_theming_engine_has_class (engine, GTK_STYLE_CLASS_TROUGH))
    unico_trim_scale_allocation (engine, &x, &y, &width, &height);

  if (gtk_theming_engine_has_class (engine, GTK_STYLE_CLASS_SEPARATOR))
    style_functions->draw_separator (engine, cr, x, y, width, height);
  else if (gtk_theming_engine_has_class (engine, GTK_STYLE_CLASS_BUTTON) &&
           gtk_theming_engine_has_class (engine, GTK_STYLE_CLASS_SPINBUTTON))
    style_functions->draw_spinbutton_frame (engine, cr, x, y, width, height);
  else if (!gtk_widget_path_is_type (path, GTK_TYPE_ICON_VIEW) &&
           gtk_theming_engine_has_class (engine, GTK_STYLE_CLASS_VIEW) &&
           gtk_theming_engine_has_region (engine, GTK_STYLE_REGION_COLUMN, &flags))
    style_functions->draw_cell_frame (engine, cr, x, y, width, height, flags);
  else
    style_functions->draw_common_frame (engine, cr, x, y, width, height);
}

static void
unico_engine_render_frame_gap (GtkThemingEngine *engine,
                               cairo_t          *cr,
                               gdouble           x,
                               gdouble           y,
                               gdouble           width,
                               gdouble           height,
                               GtkPositionType   gap_side,
                               gdouble           xy0_gap,
                               gdouble           xy1_gap)
{
  UnicoStyleFunctions *style_functions;

  UNICO_CAIRO_INIT

  unico_lookup_functions (UNICO_ENGINE (engine), &style_functions);

  if (gtk_theming_engine_has_class (engine, GTK_STYLE_CLASS_NOTEBOOK))
    style_functions->draw_notebook (engine, cr, x, y, width, height, gap_side, xy0_gap, xy1_gap);
  else
    style_functions->draw_frame_gap (engine, cr, x, y, width, height, gap_side, xy0_gap, xy1_gap);
}

static void
unico_engine_render_handle (GtkThemingEngine *engine,
                            cairo_t          *cr,
                            gdouble           x,
                            gdouble           y,
                            gdouble           width,
                            gdouble           height)
{
  UnicoStyleFunctions *style_functions;

  UNICO_CAIRO_INIT

  unico_lookup_functions (UNICO_ENGINE (engine), &style_functions);

  if (gtk_theming_engine_has_class (engine, GTK_STYLE_CLASS_GRIP))
    style_functions->draw_grip (engine, cr, x, y, width, height);
  else
    style_functions->draw_handle (engine, cr, x, y, width, height);
}

static void
unico_engine_render_line (GtkThemingEngine *engine,
                          cairo_t          *cr,
                          gdouble           x0,
                          gdouble           y0,
                          gdouble           x1,
                          gdouble           y1)
{
  UnicoStyleFunctions *style_functions;

  UNICO_CAIRO_INIT

  unico_lookup_functions (UNICO_ENGINE (engine), &style_functions);

  style_functions->draw_line (engine, cr, x0, y0, x1, y1);
}

static void
unico_engine_render_option (GtkThemingEngine *engine,
                            cairo_t          *cr,
                            gdouble           x,
                            gdouble           y,
                            gdouble           width,
                            gdouble           height)
{
  UnicoStyleFunctions *style_functions;

  UNICO_CAIRO_INIT

  unico_lookup_functions (UNICO_ENGINE (engine), &style_functions);

  if (!gtk_theming_engine_has_class (engine, GTK_STYLE_CLASS_MENUITEM))
    {
      if (unico_cairo_draw_from_texture (engine, cr, x, y, width, height))
        return;
    }

  style_functions->draw_radio (engine, cr, x, y, width, height);
}

static void
unico_engine_render_slider (GtkThemingEngine *engine,
                            cairo_t          *cr,
                            gdouble           x,
                            gdouble           y,
                            gdouble           width,
                            gdouble           height,
                            GtkOrientation    orientation)
{
  UnicoStyleFunctions *style_functions;

  UNICO_CAIRO_INIT

  unico_lookup_functions (UNICO_ENGINE (engine), &style_functions);

  if (gtk_theming_engine_has_class (engine, GTK_STYLE_CLASS_SCALE))
    {
      if (unico_cairo_draw_from_texture (engine, cr, x, y, width, height))
        return;
    }

  style_functions->draw_slider (engine, cr, x, y, width, height, orientation);
}

void
unico_engine_register_types (GTypeModule *module)
{
  unico_engine_register_type (module);
}

static void
unico_engine_init (UnicoEngine *engine)
{
  unico_register_style_default (&engine->style_functions[UNICO_STYLE_DEFAULT]);
}

static void
unico_engine_class_init (UnicoEngineClass *klass)
{
  GtkThemingEngineClass *engine_class = GTK_THEMING_ENGINE_CLASS (klass);

  engine_class->render_activity    = unico_engine_render_activity;
  engine_class->render_arrow       = unico_engine_render_arrow;
  engine_class->render_background  = unico_engine_render_background;
  engine_class->render_check       = unico_engine_render_check;
  engine_class->render_expander    = unico_engine_render_expander;
  engine_class->render_extension   = unico_engine_render_extension;
  engine_class->render_focus       = unico_engine_render_focus;
  engine_class->render_frame       = unico_engine_render_frame;
  engine_class->render_frame_gap   = unico_engine_render_frame_gap;
  engine_class->render_handle      = unico_engine_render_handle;
  engine_class->render_line        = unico_engine_render_line;
  engine_class->render_option      = unico_engine_render_option;
  engine_class->render_slider      = unico_engine_render_slider;

  gtk_theming_engine_register_property (UNICO_NAMESPACE, NULL,
                                        g_param_spec_boxed ("background-texture",
                                                            "Background texture",
                                                            "Background texture",
                                                            CAIRO_GOBJECT_TYPE_PATTERN, 0));

  gtk_theming_engine_register_property (UNICO_NAMESPACE, NULL,
                                        g_param_spec_boxed ("border-gradient",
                                                            "Border gradient",
                                                            "Border gradient",
                                                            CAIRO_GOBJECT_TYPE_PATTERN, 0));

  gtk_theming_engine_register_property (UNICO_NAMESPACE, NULL,
                                        g_param_spec_boxed ("bullet-color",
                                                            "Bullet color",
                                                            "Bullet color",
                                                            GDK_TYPE_RGBA, 0));

  gtk_theming_engine_register_property (UNICO_NAMESPACE, NULL,
                                        g_param_spec_boxed ("bullet-outline-color",
                                                            "Bullet outline color",
                                                            "Bullet outline color",
                                                            GDK_TYPE_RGBA, 0));

  gtk_theming_engine_register_property (UNICO_NAMESPACE, NULL,
                                        g_param_spec_boxed ("centroid-texture",
                                                            "Centroid texture",
                                                            "Centroid texture",
                                                            CAIRO_GOBJECT_TYPE_PATTERN, 0));


  gtk_theming_engine_register_property (UNICO_NAMESPACE, NULL,
                                        g_param_spec_boxed ("focus-border-color",
                                                            "Focus border color",
                                                            "Focus border color",
                                                            GDK_TYPE_RGBA, 0));

  gtk_theming_engine_register_property (UNICO_NAMESPACE, NULL,
                                        g_param_spec_int ("focus-border-radius",
                                                          "Focus border radius",
                                                          "Focus border radius",
                                                          0, G_MAXINT, 0, 0));

  gtk_theming_engine_register_property (UNICO_NAMESPACE, NULL,
                                        g_param_spec_boxed ("focus-fill-color",
                                                            "Focus fill color",
                                                            "Focus fill color",
                                                            GDK_TYPE_RGBA, 0));

  gtk_theming_engine_register_property (UNICO_NAMESPACE, NULL,
                                        g_param_spec_boxed ("focus-outer-stroke-color",
                                                            "Focus outer stroke color",
                                                            "Focus outer stroke color",
                                                            GDK_TYPE_RGBA, 0));

  gtk_theming_engine_register_property (UNICO_NAMESPACE, NULL,
                                        g_param_spec_int ("glow-radius",
                                                          "Glow radius",
                                                          "Glow radius",
                                                          0, G_MAXINT, 0, 0));

  gtk_theming_engine_register_property (UNICO_NAMESPACE, NULL,
                                        g_param_spec_boxed ("glow-color",
                                                            "Glow color",
                                                            "Glow color",
                                                            GDK_TYPE_RGBA, 0));

  gtk_theming_engine_register_property (UNICO_NAMESPACE, NULL,
                                        g_param_spec_boxed ("inner-stroke-color",
                                                            "Inner stroke color",
                                                            "Inner stroke color",
                                                            GDK_TYPE_RGBA, 0));

  gtk_theming_engine_register_property (UNICO_NAMESPACE, NULL,
                                        g_param_spec_boxed ("inner-stroke-top-color",
                                                            "Inner stroke top color",
                                                            "Inner stroke top color",
                                                            GDK_TYPE_RGBA, 0));

  gtk_theming_engine_register_property (UNICO_NAMESPACE, NULL,
                                        g_param_spec_boxed ("inner-stroke-right-color",
                                                            "Inner stroke right color",
                                                            "Inner stroke right color",
                                                            GDK_TYPE_RGBA, 0));

  gtk_theming_engine_register_property (UNICO_NAMESPACE, NULL,
                                        g_param_spec_boxed ("inner-stroke-bottom-color",
                                                            "Inner stroke bottom color",
                                                            "Inner stroke bottom color",
                                                            GDK_TYPE_RGBA, 0));

  gtk_theming_engine_register_property (UNICO_NAMESPACE, NULL,
                                        g_param_spec_boxed ("inner-stroke-left-color",
                                                            "Inner stroke left color",
                                                            "Inner stroke left color",
                                                            GDK_TYPE_RGBA, 0));

  gtk_theming_engine_register_property (UNICO_NAMESPACE, NULL,
                                        g_param_spec_boxed ("inner-stroke-gradient",
                                                            "Inner stroke gradient",
                                                            "Inner stroke gradient",
                                                            CAIRO_GOBJECT_TYPE_PATTERN, 0));

  gtk_theming_engine_register_property (UNICO_NAMESPACE, NULL,
                                        g_param_spec_boxed ("inner-stroke-width",
                                                            "Inner stroke width",
                                                            "Inner stroke width",
                                                            GTK_TYPE_BORDER, 0));

  gtk_theming_engine_register_property (UNICO_NAMESPACE, NULL,
                                        g_param_spec_boxed ("outer-stroke-color",
                                                            "Outer stroke color",
                                                            "Outer stroke color",
                                                            GDK_TYPE_RGBA, 0));

  gtk_theming_engine_register_property (UNICO_NAMESPACE, NULL,
                                        g_param_spec_boxed ("outer-stroke-top-color",
                                                            "Outer stroke top color",
                                                            "Outer stroke top color",
                                                            GDK_TYPE_RGBA, 0));

  gtk_theming_engine_register_property (UNICO_NAMESPACE, NULL,
                                        g_param_spec_boxed ("outer-stroke-right-color",
                                                            "Outer stroke right color",
                                                            "Outer stroke right color",
                                                            GDK_TYPE_RGBA, 0));

  gtk_theming_engine_register_property (UNICO_NAMESPACE, NULL,
                                        g_param_spec_boxed ("outer-stroke-bottom-color",
                                                            "Outer stroke bottom color",
                                                            "Outer stroke bottom color",
                                                            GDK_TYPE_RGBA, 0));

  gtk_theming_engine_register_property (UNICO_NAMESPACE, NULL,
                                        g_param_spec_boxed ("outer-stroke-left-color",
                                                            "Outer stroke left color",
                                                            "Outer stroke left color",
                                                            GDK_TYPE_RGBA, 0));

  gtk_theming_engine_register_property (UNICO_NAMESPACE, NULL,
                                        g_param_spec_boxed ("outer-stroke-gradient",
                                                            "Outer stroke gradient",
                                                            "Outer stroke gradient",
                                                            CAIRO_GOBJECT_TYPE_PATTERN, 0));

  gtk_theming_engine_register_property (UNICO_NAMESPACE, NULL,
                                        g_param_spec_boxed ("outer-stroke-width",
                                                            "Outer stroke width",
                                                            "Outer stroke width",
                                                            GTK_TYPE_BORDER, 0));

  gtk_theming_engine_register_property (UNICO_NAMESPACE, NULL,
                                        g_param_spec_boxed ("text-shadow-color",
                                                            "Text shadow color",
                                                            "Text shadow color",
                                                            GDK_TYPE_RGBA, 0));
}

static void
unico_engine_class_finalize (UnicoEngineClass *klass)
{
}
