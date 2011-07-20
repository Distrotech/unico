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
#include "unico-css-support.h"
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

static gboolean
render_from_assets_common (GtkThemingEngine *engine,
                           cairo_t          *cr,
                           gdouble           x,
                           gdouble           y,
                           gdouble           width,
                           gdouble           height)
{
  GtkStateFlags flags;
  GValue value = { 0, };
  cairo_pattern_t *asset = NULL;
  cairo_surface_t *surface = NULL;
  gboolean retval = FALSE;

  flags = gtk_theming_engine_get_state (engine);

  gtk_theming_engine_get_property (engine, "background-image", flags, &value);

  if (!G_VALUE_HOLDS_BOXED (&value))
    return FALSE;

  asset = g_value_dup_boxed (&value);
  g_value_unset (&value);

  if (asset != NULL)
    cairo_pattern_get_surface (asset, &surface);

  if (surface != NULL)
    {
      cairo_save (cr);

      cairo_scale (cr, width / cairo_image_surface_get_width (surface),
                       height / cairo_image_surface_get_height (surface));
      cairo_set_source_surface (cr, surface, x, y);

      cairo_paint (cr);

      cairo_restore (cr);

      retval = TRUE;
    }

  if (asset != NULL)
    cairo_pattern_destroy (asset);

  return retval;
}

static void
trim_allocation_for_scale (GtkThemingEngine *engine,
                           gdouble          *x,
                           gdouble          *y,
                           gdouble          *width,
                           gdouble          *height)
{
  const GtkWidgetPath *path;

  path = gtk_theming_engine_get_path (engine);

  if (gtk_widget_path_is_type (path, GTK_TYPE_SCALE) &&
      (gtk_theming_engine_has_class (engine, GTK_STYLE_CLASS_TROUGH) ||
       gtk_theming_engine_has_class (engine, GTK_STYLE_CLASS_PROGRESSBAR)))
    {
      if (!gtk_theming_engine_has_class (engine, GTK_STYLE_CLASS_VERTICAL))
        {
          *y += *height / 2.0 - 2.0;
          *height = 5;
        }
      else
        {
          *x += *width / 2.0 - 2.0;
          *width = 5;
        }
    }
}

static void
unico_engine_render_activity (GtkThemingEngine *engine,
                              cairo_t          *cr,
                              gdouble           x,
                              gdouble           y,
                              gdouble           width,
                              gdouble           height)
{
  UnicoStyleFunctions *style_functions;

  UNICO_CAIRO_INIT

  unico_lookup_functions (UNICO_ENGINE (engine), &style_functions);

  if (gtk_theming_engine_has_class (engine, GTK_STYLE_CLASS_PROGRESSBAR))
    style_functions->draw_progressbar_activity (engine, cr, x, y, width, height);
  else
    GTK_THEMING_ENGINE_CLASS (unico_engine_parent_class)->render_activity (engine, cr, x, y, width, height);
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

  trim_allocation_for_scale (engine, &x, &y, &width, &height);

  if (gtk_theming_engine_has_class (engine, GTK_STYLE_CLASS_BUTTON) &&
      gtk_widget_path_has_parent (path, GTK_TYPE_COMBO_BOX_TEXT))
    style_functions->draw_combo_button_background (engine, cr, x, y, width, height);
  else if (gtk_theming_engine_has_class (engine, GTK_STYLE_CLASS_BUTTON) &&
           gtk_theming_engine_has_class (engine, GTK_STYLE_CLASS_SPINBUTTON))
    style_functions->draw_spinbutton_background (engine, cr, x, y, width, height);
  else if (gtk_widget_path_is_type (path, GTK_TYPE_ICON_VIEW))
      style_functions->draw_common (engine, cr, x, y, width, height);
  else if (gtk_theming_engine_has_class (engine, GTK_STYLE_CLASS_VIEW) &&
           gtk_theming_engine_has_region (engine, GTK_STYLE_REGION_COLUMN, &flags))
      style_functions->draw_cell (engine, cr, x, y, width, height, flags);
  else if (gtk_theming_engine_has_class (engine, GTK_STYLE_CLASS_DOCK))
    GTK_THEMING_ENGINE_CLASS (unico_engine_parent_class)->render_background (engine, cr, x, y, width, height);
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
      if (render_from_assets_common (engine, cr, x, y, width, height))
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

  if (gtk_theming_engine_has_region (engine, GTK_STYLE_REGION_TAB, NULL))
    style_functions->draw_tab (engine, cr, x, y, width, height, gap_side);
  else
    GTK_THEMING_ENGINE_CLASS (unico_engine_parent_class)->render_extension (engine, cr, x, y, width, height, gap_side);
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

  UNICO_CAIRO_INIT

  unico_lookup_functions (UNICO_ENGINE (engine), &style_functions);
  path = gtk_theming_engine_get_path (engine);

  trim_allocation_for_scale (engine, &x, &y, &width, &height);

  if (gtk_theming_engine_has_class (engine, GTK_STYLE_CLASS_SEPARATOR))
    style_functions->draw_separator (engine, cr, x, y, width, height);
  else if (gtk_theming_engine_has_class (engine, GTK_STYLE_CLASS_BUTTON) &&
           gtk_widget_path_has_parent (path, GTK_TYPE_COMBO_BOX_TEXT))
    style_functions->draw_combo_button_frame (engine, cr, x, y, width, height);
  else if (gtk_theming_engine_has_class (engine, GTK_STYLE_CLASS_BUTTON) &&
           gtk_theming_engine_has_class (engine, GTK_STYLE_CLASS_SPINBUTTON))
    style_functions->draw_spinbutton_frame (engine, cr, x, y, width, height);
  else if (gtk_theming_engine_has_class (engine, GTK_STYLE_CLASS_FRAME) &&
           gtk_widget_path_is_type (path, GTK_TYPE_SCROLLED_WINDOW))
    style_functions->draw_scrolled_window_frame (engine, cr, x, y, width, height);
  else if (gtk_theming_engine_has_class (engine, GTK_STYLE_CLASS_DOCK))
    GTK_THEMING_ENGINE_CLASS (unico_engine_parent_class)->render_frame (engine, cr, x, y, width, height);
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

  if (gtk_theming_engine_has_class (engine, "notebook"))
    style_functions->draw_notebook (engine, cr, x, y, width, height, gap_side, xy0_gap, xy1_gap);
  else if (gtk_theming_engine_has_class (engine, "frame"))
    style_functions->draw_frame_gap (engine, cr, x, y, width, height, gap_side, xy0_gap, xy1_gap);
  else
    GTK_THEMING_ENGINE_CLASS (unico_engine_parent_class)->render_frame_gap (engine, cr,
                                                                            x, y, width, height,
                                                                            gap_side, xy0_gap, xy1_gap);
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
  else if (gtk_theming_engine_has_class (engine, GTK_STYLE_CLASS_PANE_SEPARATOR))
    style_functions->draw_pane_separator (engine, cr, x, y, width, height);
  else
    GTK_THEMING_ENGINE_CLASS (unico_engine_parent_class)->render_handle (engine, cr, x, y, width, height);
}

static GdkPixbuf*
pixbuf_set_transparency (GdkPixbuf *pixbuf,
                         gdouble    alpha_percent)
{
  GdkPixbuf *target;
  guchar *data, *current;
  guint x, y, rowstride, height, width;

  g_return_val_if_fail (pixbuf != NULL, NULL);
  g_return_val_if_fail (GDK_IS_PIXBUF (pixbuf), NULL);

  /* Returns a copy of pixbuf with it's non-completely-transparent pixels to
     have an alpha level "alpha_percent" of their original value. */

  target = gdk_pixbuf_add_alpha (pixbuf, FALSE, 0, 0, 0);

  if (alpha_percent == 1.0)
    return target;

  width = gdk_pixbuf_get_width (target);
  height = gdk_pixbuf_get_height (target);
  rowstride = gdk_pixbuf_get_rowstride (target);
  data = gdk_pixbuf_get_pixels (target);

  for (y = 0; y < height; y++)
    {
      for (x = 0; x < width; x++)
        {
          /* The "4" is the number of chars per pixel, in this case, RGBA,
             the 3 means "skip to the alpha" */
          current = data + (y * rowstride) + (x * 4) + 3;
          *(current) = (guchar) (*(current) * alpha_percent);
        }
    }

  return target;
}

static GdkPixbuf*
pixbuf_scale_or_ref (GdkPixbuf *src,
                     gint       width,
                     gint       height)
{
  if (width == gdk_pixbuf_get_width (src) &&
      height == gdk_pixbuf_get_height (src))
    return g_object_ref (src);
  else
    return gdk_pixbuf_scale_simple (src,
                                    width, height,
                                    GDK_INTERP_BILINEAR);
}

static GdkPixbuf*
unico_engine_render_icon_pixbuf (GtkThemingEngine    *engine,
                                 const GtkIconSource *source,
                                 GtkIconSize          size)
{
  GdkPixbuf *scaled;
  GdkPixbuf *stated;
  GdkPixbuf *base_pixbuf;
  GdkScreen *screen;
  GtkSettings *settings;
  GtkStateFlags flags;
  gint width = 1;
  gint height = 1;

  base_pixbuf = gtk_icon_source_get_pixbuf (source);
  screen = gtk_theming_engine_get_screen (engine);
  settings = gtk_settings_get_for_screen (screen);
  flags = gtk_theming_engine_get_state (engine);

  g_return_val_if_fail (base_pixbuf != NULL, NULL);

  if (size != (GtkIconSize) -1 && !gtk_icon_size_lookup_for_settings (settings, size, &width, &height))
    {
      g_warning (G_STRLOC ": invalid icon size '%d'", size);
      return NULL;
    }

  /* If the size was wildcarded, and we're allowed to scale, then scale; otherwise,
   * leave it alone.
   */
  if (size != (GtkIconSize)-1 && gtk_icon_source_get_size_wildcarded (source))
    scaled = pixbuf_scale_or_ref (base_pixbuf, width, height);
  else
    scaled = g_object_ref (base_pixbuf);

  /* If the state was wildcarded, then generate a state. */
  if (gtk_icon_source_get_state_wildcarded (source))
    {
      if (flags & GTK_STATE_FLAG_INSENSITIVE)
        {
          stated = pixbuf_set_transparency (scaled, 0.3);
          gdk_pixbuf_saturate_and_pixelate (stated, stated, 0.1, FALSE);

          g_object_unref (scaled);
        }
      else if (flags & GTK_STATE_FLAG_PRELIGHT)
        {
          stated = gdk_pixbuf_copy (scaled);
          gdk_pixbuf_saturate_and_pixelate (scaled, stated, 1.2, FALSE);

          g_object_unref (scaled);
        }
      else
        stated = scaled;
    }
  else
    stated = scaled;

  return stated;
}

static void
unico_engine_render_layout (GtkThemingEngine *engine,
                            cairo_t          *cr,
                            gdouble           x,
                            gdouble           y,
                            PangoLayout      *layout)
{
  GTK_THEMING_ENGINE_CLASS (unico_engine_parent_class)->render_layout (engine, cr, x, y, layout);
}

static void
unico_engine_render_line (GtkThemingEngine *engine,
                          cairo_t          *cr,
                          gdouble           x0,
                          gdouble           y0,
                          gdouble           x1,
                          gdouble           y1)
{
  GTK_THEMING_ENGINE_CLASS (unico_engine_parent_class)->render_line (engine, cr, x0, y0, x1, y1);
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
      if (render_from_assets_common (engine, cr, x, y, width, height))
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
      if (render_from_assets_common (engine, cr, x, y, width, height))
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
  engine_class->render_icon_pixbuf = unico_engine_render_icon_pixbuf;
  engine_class->render_layout      = unico_engine_render_layout;
  engine_class->render_line        = unico_engine_render_line;
  engine_class->render_option      = unico_engine_render_option;
  engine_class->render_slider      = unico_engine_render_slider;

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
                                        g_param_spec_boxed ("inner-stroke-gradient",
                                                            "Inner stroke gradient",
                                                            "Inner stroke gradient",
                                                            CAIRO_GOBJECT_TYPE_PATTERN, 0));

  gtk_theming_engine_register_property (UNICO_NAMESPACE, NULL,
                                        g_param_spec_boxed ("outer-stroke-color",
                                                            "Outer stroke color",
                                                            "Outer stroke color",
                                                            GDK_TYPE_RGBA, 0));

  gtk_theming_engine_register_property (UNICO_NAMESPACE, NULL,
                                        g_param_spec_boxed ("outer-stroke-gradient",
                                                            "Outer stroke gradient",
                                                            "Outer stroke gradient",
                                                            CAIRO_GOBJECT_TYPE_PATTERN, 0));

  gtk_theming_engine_register_property (UNICO_NAMESPACE, NULL,
                                        g_param_spec_enum ("outer-stroke-style",
                                                           "Outer stroke style",
                                                           "Outer stroke style",
                                                           UNICO_TYPE_STROKE_STYLE,
                                                           UNICO_STROKE_STYLE_NONE, 0));

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
