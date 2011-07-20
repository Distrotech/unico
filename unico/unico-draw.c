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

#include <gtk/gtk.h>
#include <cairo.h>

#include "math.h"
#include "unico-cairo-support.h"
#include "unico-draw.h"
#include "unico-support.h"
#include "unico-types.h"

#define UNICO_RECT_SET(rect, _x, _y, _w, _h) (rect).x      = (_x); \
                                             (rect).y      = (_y); \
                                             (rect).width  = (_w); \
                                             (rect).height = (_h);

static void
unico_draw_arrow (GtkThemingEngine *engine,
                  cairo_t          *cr,
                  gdouble           angle,
                  gdouble           x,
                  gdouble           y,
                  gdouble           size)
{
  GtkStateFlags state;
  GdkRGBA color;
  gdouble size_reduction = 2;

  state = gtk_theming_engine_get_state (engine);
  gtk_theming_engine_get_color (engine, state, &color);

  cairo_save (cr);

  /* use floor function to adjust those doubles. */
  y = floor (y);
  x = floor (x);
  size = floor (size);

  size -= size_reduction;

  cairo_translate (cr, size_reduction / 2, size_reduction / 2);
  cairo_translate (cr, x + size / 2.0 + 0.5, y + size / 2.0 + 0.5);
  cairo_rotate (cr, angle - G_PI_2);
  cairo_translate (cr, (gint) (size / 4.0), 0);

  /* FIXME This +1 / -1 is done to fix blurred diagonal lines.
   * I know it's not nice at all, but it fix a visual bug. */
  cairo_move_to (cr, -size / 2.0, -size / 2.0);
  cairo_rel_line_to (cr, size / 2.0 + 1, size / 2.0);
  cairo_rel_line_to (cr, -size / 2.0 - 1, size / 2.0);
  cairo_close_path (cr);

  cairo_set_source_rgba (cr, color.red, color.green, color.blue, color.alpha * 0.75);
  cairo_fill_preserve (cr);

  gdk_cairo_set_source_rgba (cr, &color);
  cairo_stroke (cr);

  cairo_restore (cr);
}

static void
unico_draw_cell (DRAW_ARGS,
                 GtkRegionFlags flags)
{
  GtkJunctionSides junction;
  guint hidden_side;

  junction = GTK_JUNCTION_RIGHT | GTK_JUNCTION_LEFT;

  hidden_side = SIDE_RIGHT;

  if ((flags & GTK_REGION_FIRST) != 0)
    junction &= ~(GTK_JUNCTION_CORNER_TOPLEFT | GTK_JUNCTION_CORNER_BOTTOMLEFT);
  if ((flags & GTK_REGION_LAST) != 0)
    {
      junction &= ~(GTK_JUNCTION_CORNER_TOPRIGHT | GTK_JUNCTION_CORNER_BOTTOMRIGHT);
      hidden_side = 0;
    }

  unico_cairo_draw_background (engine, cr,
                               x, y, width, height,
                               hidden_side, junction);

  unico_cairo_draw_frame (engine, cr,
                          x, y, width, height,
                          hidden_side, junction);
}

static void
unico_draw_check (DRAW_ARGS)
{
  GtkStateFlags state;
  gboolean in_menu;
  gboolean draw_bullet, inconsistent;

  state = gtk_theming_engine_get_state (engine);

  in_menu = gtk_theming_engine_has_class (engine, GTK_STYLE_CLASS_MENUITEM);

  inconsistent = (state & GTK_STATE_FLAG_INCONSISTENT) != 0;
  draw_bullet = (state & GTK_STATE_FLAG_ACTIVE) != 0;
  draw_bullet |= inconsistent;

  if (!in_menu)
    {
      unico_cairo_draw_background (engine, cr,
                                   x, y, width, height,
                                   0, gtk_theming_engine_get_junction_sides (engine));
      unico_cairo_draw_frame (engine, cr,
                              x, y, width, height,
                              0, gtk_theming_engine_get_junction_sides (engine));
    }

  if (draw_bullet)
    {
      GdkRGBA *bullet_color;

      gtk_theming_engine_get (engine, state,
                              "-unico-bullet-color", &bullet_color,
                              NULL);

      if (inconsistent)
        {
          cairo_save (cr);

          cairo_set_line_width (cr, 2.0);
          cairo_move_to (cr, 3, (double) height / 2.0);
          cairo_line_to (cr, width - 3, (double) height / 2.0);

          cairo_restore (cr);
        }
      else
        {
          cairo_translate (cr, x, y);

          if (in_menu)
            {
              cairo_scale (cr, (double) width / 18.0, (double) height / 18.0);
              cairo_translate (cr, 2.0, 3.0);
            }
          else
            {
              GdkRGBA *bullet_outline_color;

              gtk_theming_engine_get (engine, state,
                                      "-unico-bullet-outline-color", &bullet_outline_color,
                                      NULL);

              cairo_scale (cr, (double) width / 18.0, (double) height / 18.0);

              cairo_move_to (cr, 5.0, 5.65);
              cairo_line_to (cr, 8.95, 9.57);
              cairo_line_to (cr, 16.0, 2.54);
              cairo_line_to (cr, 16.0, 8.36);
              cairo_line_to (cr, 10.6, 15.1);
              cairo_line_to (cr, 7.6, 15.1);
              cairo_line_to (cr, 2.95, 10.48);
              cairo_line_to (cr, 2.95, 7.65);
              cairo_close_path (cr);

              gdk_cairo_set_source_rgba (cr, bullet_outline_color);
              cairo_fill (cr);

              cairo_translate (cr, 4.0, 2.0);

              gdk_rgba_free (bullet_outline_color);
            }

          cairo_move_to (cr, 0.0, 6.0);
          cairo_line_to (cr, 0.0, 8.0);
          cairo_line_to (cr, 4.0, 12.0);
          cairo_line_to (cr, 6.0, 12.0);
          cairo_line_to (cr, 15.0, 1.0);
          cairo_line_to (cr, 15.0, 0.0);
          cairo_line_to (cr, 14.0, 0.0);
          cairo_line_to (cr, 5.0, 9.0);
          cairo_line_to (cr, 1.0, 5.0);
          cairo_close_path (cr);
        }

      gdk_cairo_set_source_rgba (cr, bullet_color);
      cairo_fill (cr);

      gdk_rgba_free (bullet_color);
    }
}

static void
unico_draw_column_header_background (DRAW_ARGS,
                                     GtkRegionFlags flags)
{
  GtkJunctionSides junction;

  junction = GTK_JUNCTION_RIGHT | GTK_JUNCTION_LEFT;

  unico_cairo_draw_background (engine, cr,
                               x, y, width, height,
                               0, junction);
}

static void
unico_draw_column_header_frame (DRAW_ARGS, 
                                GtkRegionFlags flags)
{
  GtkJunctionSides junction;
  guint hidden_side;

  junction = GTK_JUNCTION_RIGHT | GTK_JUNCTION_LEFT;

  hidden_side = SIDE_RIGHT | SIDE_TOP;

  if ((flags & GTK_REGION_FIRST) != 0)
    {
      hidden_side |= SIDE_LEFT;
    }
  if ((flags & GTK_REGION_LAST) != 0)
    {
      hidden_side = SIDE_RIGHT | SIDE_TOP;
    }

  unico_cairo_draw_frame (engine, cr,
                          x, y, width, height,
                          hidden_side, junction);
}

static void
unico_draw_combo_button_background (DRAW_ARGS)
{
  /* Playground for junctions. */
  unico_cairo_draw_background (engine, cr, x, y, width, height, 0, gtk_theming_engine_get_junction_sides (engine));
}

static void
unico_draw_combo_button_frame (DRAW_ARGS)
{
  /* Playground for junctions. */
  unico_cairo_draw_frame (engine, cr, x, y, width, height, 0, gtk_theming_engine_get_junction_sides (engine));
}

static void
unico_draw_common_background (DRAW_ARGS)
{
  unico_cairo_draw_background (engine, cr,
                               x, y, width, height,
                               0, gtk_theming_engine_get_junction_sides (engine));
}

static void
unico_draw_common_frame (DRAW_ARGS)
{
  unico_cairo_draw_frame (engine, cr,
                          x, y, width, height,
                          0, gtk_theming_engine_get_junction_sides (engine));
}

static void
unico_draw_expander (DRAW_ARGS)
{
  GtkStateFlags state;
  GdkRGBA color;
  gdouble size;
  gdouble angle = G_PI_2;

  state = gtk_theming_engine_get_state (engine);
  gtk_theming_engine_get_color (engine, state, &color);

  cairo_save (cr);

  size = floor (MIN (width, height));

  x += width / 2 - size / 2;
  y += height / 2 - size / 2;

  if ((state & GTK_STATE_FLAG_ACTIVE) == 0)
    angle = 0;

  cairo_translate (cr, x + size / 2.0 + 0.5, y + size / 2.0 + 0.5);
  cairo_rotate (cr, angle);
  cairo_translate (cr, (gint) (size / 4.0), 0);

  /* FIXME This +1 / -1 is done to fix blurred diagonal lines.
   * I know it's not nice at all, but it fix a visual bug. */
  cairo_move_to (cr, -size / 2.0, -size / 2.0);
  cairo_rel_line_to (cr, size / 2.0 + 1, size / 2.0);
  cairo_rel_line_to (cr, -size / 2.0 - 1, size / 2.0);
  cairo_close_path (cr);

  cairo_set_source_rgba (cr, color.red, color.green, color.blue, color.alpha * 0.75);
  cairo_fill_preserve (cr);

  gdk_cairo_set_source_rgba (cr, &color);
  cairo_stroke (cr);

  cairo_restore (cr);
}

static void
unico_draw_focus (DRAW_ARGS)
{
  GdkRGBA *fill_color, *border_color, *outer_stroke_color;
  GtkStateFlags state;
  gint line_width;
  gint radius;

  state = gtk_theming_engine_get_state (engine);
  gtk_theming_engine_get (engine, state,
                          "-unico-focus-border-color", &border_color,
                          "-unico-focus-border-radius", &radius,
                          "-unico-focus-fill-color", &fill_color,
                          "-unico-focus-outer-stroke-color", &outer_stroke_color,
                          NULL);

  gtk_theming_engine_get_style (engine,
                                "focus-line-width", &line_width,
                                NULL);  

  cairo_save (cr);
  cairo_set_line_width (cr, line_width);

  unico_cairo_round_rect (cr, x, y,
                              width, height,
                              radius, SIDE_ALL, GTK_JUNCTION_NONE);
  gdk_cairo_set_source_rgba (cr, fill_color);
  cairo_fill (cr);

  unico_cairo_round_rect_inner (cr, x - line_width, y - line_width,
                                    width + line_width * 2, height + line_width * 2,
                                    radius + 1, SIDE_ALL, GTK_JUNCTION_NONE);
  gdk_cairo_set_source_rgba (cr, outer_stroke_color);
  cairo_stroke (cr);

  unico_cairo_round_rect_inner (cr, x, y,
                                    width, height,
                                    radius, SIDE_ALL, GTK_JUNCTION_NONE);
  gdk_cairo_set_source_rgba (cr, border_color);
  cairo_stroke (cr);

  cairo_restore (cr);

  gdk_rgba_free (border_color);
  gdk_rgba_free (fill_color);
  gdk_rgba_free (outer_stroke_color);
}

static void
unico_draw_frame_gap (DRAW_ARGS,
                      GtkPositionType gap_side,
                      gdouble         xy0_gap,
                      gdouble         xy1_gap)
{
  GtkJunctionSides junction;
  gint border_width, radius;
  gdouble x0, y0, x1, y1, xc, yc, wc, hc;
  gdouble line_width;

  xc = yc = wc = hc = 0;

  junction = gtk_theming_engine_get_junction_sides (engine);

  unico_get_line_width (engine, &line_width);
  unico_get_border_radius (engine, &radius);

  border_width = cairo_get_line_width (cr);

  if (unico_has_outer_stroke (engine))
    border_width = 2 * cairo_get_line_width (cr);

  cairo_save (cr);

  switch (gap_side)
  {
    case GTK_POS_TOP:
      xc = x + xy0_gap + border_width;
      yc = y;
      wc = MAX (xy1_gap - xy0_gap - 2 * border_width, 0);
      hc = border_width + cairo_get_line_width (cr);

      if (xy0_gap < radius)
        junction |= GTK_JUNCTION_CORNER_TOPLEFT;

      if (xy1_gap > width - radius)
        junction |= GTK_JUNCTION_CORNER_TOPRIGHT;

      break;
    default:
    case GTK_POS_BOTTOM:
      xc = x + xy0_gap + border_width;
      yc = y + height - border_width;
      wc = MAX (xy1_gap - xy0_gap - 2 * border_width, 0);
      hc = border_width + cairo_get_line_width (cr);

      if (xy0_gap < radius)
        junction |= GTK_JUNCTION_CORNER_BOTTOMLEFT;

      if (xy1_gap > width - radius)
        junction |= GTK_JUNCTION_CORNER_BOTTOMRIGHT;

      break;
    case GTK_POS_LEFT:
      xc = x;
      yc = y + xy0_gap + border_width;
      wc = border_width + cairo_get_line_width (cr);
      hc = MAX (xy1_gap - xy0_gap - 2 * border_width, 0);

      if (xy0_gap < radius)
        junction |= GTK_JUNCTION_CORNER_TOPLEFT;

      if (xy1_gap > height - radius)
        junction |= GTK_JUNCTION_CORNER_BOTTOMLEFT;

      break;
    case GTK_POS_RIGHT:
      xc = x + width - border_width;
      yc = y + xy0_gap + border_width;
      wc = border_width + cairo_get_line_width (cr);
      hc = MAX (xy1_gap - xy0_gap - 2 * border_width, 0);

      if (xy0_gap < radius)
        junction |= GTK_JUNCTION_CORNER_TOPRIGHT;

      if (xy1_gap > height - radius)
        junction |= GTK_JUNCTION_CORNER_BOTTOMRIGHT;

      break;
  }

  cairo_clip_extents (cr, &x0, &y0, &x1, &y1);
  cairo_rectangle (cr, x0, y0, x1 - x0, yc - y0);
  cairo_rectangle (cr, x0, yc, xc - x0, hc);
  cairo_rectangle (cr, xc + wc, yc, x1 - (xc + wc), hc);
  cairo_rectangle (cr, x0, yc + hc, x1 - x0, y1 - (yc + hc));
  cairo_clip (cr);

  cairo_translate (cr, x, y);

  /* FIXME Maybe we need to add a check for the GtkBorderStyle,
   * old GTK_SHADOW_IN corresponds to GTK_BORDER_STYLE_INSET. */

  unico_cairo_draw_frame (engine, cr, 0, 0, width, height, 0, junction);

  cairo_restore (cr);
}

static void
unico_draw_grip (DRAW_ARGS)
{
  GdkRGBA border_color;
  GdkRGBA *inner_stroke_color;
  GtkStateFlags flags;
  gint lx, ly;

  flags = gtk_theming_engine_get_state (engine);
  gtk_theming_engine_get_border_color (engine, flags, &border_color);
  gtk_theming_engine_get (engine, flags,
                          "-unico-inner-stroke-color", &inner_stroke_color,
                          NULL);

  for (ly = 0; ly < 4; ly++) /* vertically, four rows of dots */
    {
      for (lx = 0; lx <= ly; lx++) /* horizontally */
        {
          int ny = (3.5 - ly) * 3;
          int nx = lx * 3;

          gdk_cairo_set_source_rgba (cr, inner_stroke_color);
          cairo_rectangle (cr, x + width - nx - 1, y + height - ny - 1, 2, 2);
          cairo_fill (cr);

          gdk_cairo_set_source_rgba (cr, &border_color);
          cairo_rectangle (cr, x + width - nx - 1, y + height - ny - 1, 1, 1);
          cairo_fill (cr);
        }
    }

  gdk_rgba_free (inner_stroke_color);
}

static void
unico_draw_icon_view (DRAW_ARGS)
{
  unico_cairo_draw_background (engine, cr,
                               x, y, width, height,
                               0, gtk_theming_engine_get_junction_sides (engine));
  unico_cairo_draw_frame (engine, cr,
                          x, y, width, height,
                          0, gtk_theming_engine_get_junction_sides (engine));
}

static void
unico_draw_menubaritem_background (DRAW_ARGS)
{
  unico_cairo_draw_background (engine, cr,
                               x, y, width, height,
                               0, GTK_JUNCTION_BOTTOM);
}

static void
unico_draw_menubaritem_frame (DRAW_ARGS)
{
  unico_cairo_draw_frame (engine, cr,
                          x, y, width, height,
                          0, GTK_JUNCTION_BOTTOM);
}

static void
unico_draw_notebook (DRAW_ARGS,
                     GtkPositionType gap_side,
                     gdouble         xy0_gap,
                     gdouble         xy1_gap)
{
  unico_cairo_draw_background (engine, cr,
                               x, y, width, height,
                               0, gtk_theming_engine_get_junction_sides (engine));
  unico_draw_frame_gap (engine, cr, x, y, width, height, gap_side, xy0_gap, xy1_gap);
}

static void
unico_draw_pane_separator (DRAW_ARGS)
{
  GdkRGBA border_color;
  GdkRGBA *inner_stroke_color;
  GtkStateFlags flags;
  gdouble line_width;
  gint bar_height;
  gint bar_width = 3;
  gint i, bar_y = 1;
  gint num_bars = 3, bar_spacing = 3;

  unico_get_line_width (engine, &line_width);

  bar_height = num_bars * bar_spacing * line_width;

  flags = gtk_theming_engine_get_state (engine);
  gtk_theming_engine_get_border_color (engine, flags, &border_color);
  gtk_theming_engine_get (engine, flags,
                          "-unico-inner-stroke-color", &inner_stroke_color,
                          NULL);

  /* FIXME add HORIZONTAL case with GTK_STYLE_CLASS_HORIZONTAL */
  cairo_translate (cr, x + width / 2 - bar_width / 2, y + height / 2 - bar_height / 2 + 0.5);

  /* FIXME translations could be done better */
  for (i = 0; i < num_bars; i++)
    {
      cairo_move_to (cr, -0.5, bar_y);
      cairo_line_to (cr, bar_width - 0.5, bar_y);
      gdk_cairo_set_source_rgba (cr, &border_color);
      cairo_stroke (cr);

      cairo_move_to (cr, -0.5, bar_y + line_width);
      cairo_line_to (cr, bar_width - 0.5, bar_y + line_width);
      gdk_cairo_set_source_rgba (cr, inner_stroke_color);
      cairo_stroke (cr);

      bar_y += bar_spacing;
    }

  gdk_rgba_free (inner_stroke_color);
}

static void
unico_draw_progressbar_fill_background (DRAW_ARGS)
{
  /* Playground for effects. */
  unico_cairo_draw_background (engine, cr,
                               x, y, width, height,
                               0, gtk_theming_engine_get_junction_sides (engine));
}

static void
unico_draw_progressbar_fill_frame (DRAW_ARGS)
{
  /* Playground for effects. */
  unico_cairo_draw_frame (engine, cr,
                          x, y, width, height,
                          0, gtk_theming_engine_get_junction_sides (engine));
}

static void
unico_draw_radio (DRAW_ARGS)
{
  GtkStateFlags flags;
  gboolean in_menu;
  gboolean draw_bullet, inconsistent;

  flags = gtk_theming_engine_get_state (engine);

  in_menu = gtk_theming_engine_has_class (engine, GTK_STYLE_CLASS_MENUITEM);

  inconsistent = (flags & GTK_STATE_FLAG_INCONSISTENT) != 0;
  draw_bullet = (flags & GTK_STATE_FLAG_ACTIVE) != 0;
  draw_bullet |= inconsistent;

  if (!in_menu)
    {
      unico_cairo_draw_background (engine, cr,
                                   x, y, width, height,
                                   0, gtk_theming_engine_get_junction_sides (engine));
      unico_cairo_draw_frame (engine, cr,
                              x, y, width, height,
                              0, gtk_theming_engine_get_junction_sides (engine));
    }

  if (draw_bullet)
    {
      GdkRGBA *bullet_color;

      gtk_theming_engine_get (engine, flags,
                              "-unico-bullet-color", &bullet_color,
                              NULL);

      if (inconsistent)
        {
          cairo_save (cr);

          cairo_set_line_cap(cr, CAIRO_LINE_CAP_ROUND);
          cairo_set_line_width (cr, 2.0);

          cairo_move_to(cr, 5, (double) height / 2.0);
          cairo_line_to(cr, width - 5, (double) height / 2.0);

          gdk_cairo_set_source_rgba (cr, bullet_color);
          cairo_stroke (cr);

          cairo_restore (cr);
        }
      else
        {
          if (in_menu)
            cairo_arc (cr, x + (double) width / 2.0, y + (double) height / 2.0,
                           (double) (width + height) / 4.0 - 4, 0, G_PI * 2);
          else
            {
              GdkRGBA *bullet_outline_color;

              gtk_theming_engine_get (engine, flags,
                                      "-unico-bullet-outline-color", &bullet_outline_color,
                                      NULL);

              cairo_arc (cr, x + (double) width / 2.0, y + (double) height / 2.0,
                             (double) (width + height) / 4.0 - 4, 0, G_PI * 2);
              gdk_cairo_set_source_rgba (cr, bullet_outline_color);
              cairo_fill (cr);

              cairo_arc (cr, x + (double) width / 2.0, y + (double) height / 2.0,
                             (double) (width + height) / 4.0 - 5, 0, G_PI * 2);

              gdk_rgba_free (bullet_outline_color);
            }

          gdk_cairo_set_source_rgba (cr, bullet_color);
          cairo_fill (cr);
        }

      gdk_rgba_free (bullet_color);
    }
}

static void
unico_draw_scrollbar_stepper_background (DRAW_ARGS)
{
  unico_cairo_draw_background (engine, cr,
                               x, y, width, height,
                               0, gtk_theming_engine_get_junction_sides (engine));
}

static void
unico_draw_scrollbar_stepper_frame (DRAW_ARGS)
{
  unico_cairo_draw_frame (engine, cr,
                          x, y, width, height,
                          0, gtk_theming_engine_get_junction_sides (engine));
}

static void
unico_draw_scrollbar_slider (DRAW_ARGS)
{
  unico_cairo_draw_background (engine, cr,
                               x, y, width, height,
                               0, gtk_theming_engine_get_junction_sides (engine));
  unico_cairo_draw_frame (engine, cr,
                          x, y, width, height,
                          0, gtk_theming_engine_get_junction_sides (engine));
}

static void
unico_draw_scrolled_window_frame (DRAW_ARGS)
{
  unico_cairo_draw_frame (engine, cr,
                          x, y, width, height,
                          0, gtk_theming_engine_get_junction_sides (engine));
}

static void
unico_draw_separator (DRAW_ARGS)
{
  gdouble line_width;

  unico_get_line_width (engine, &line_width);

  /* FIXME right code should be
   * if (gtk_theming_engine_has_class (engine, GTK_STYLE_CLASS_VERTICAL))
   * but doesn't work for separator tool item. */
  if (width > height)
    {
      //cairo_move_to (cr, x, y + height / 2 + line_width / 2);
      //cairo_line_to (cr, x + width, y + height / 2 + line_width / 2);
      //unico_cairo_draw_inner_stroke_from_path (engine, cr, x, y + height / 2 + line_width / 2, width, line_width);

      //cairo_move_to (cr, x, y + height / 2 - line_width / 2);
      //cairo_line_to (cr, x + width, y + height / 2 - line_width / 2);
      //unico_cairo_draw_border_from_path (engine, cr, x, y + height / 2 - line_width / 2, width, line_width);
    }
  else
    {
      //cairo_move_to (cr, x + width / 2 + line_width / 2, y);
      //cairo_line_to (cr, x + width / 2 + line_width / 2, y + height);
      //unico_cairo_draw_inner_stroke_from_path (engine, cr, x + width / 2 + line_width / 2, y, line_width, height);

      //cairo_move_to (cr, x + width / 2 - line_width / 2, y);
      //cairo_line_to (cr, x + width / 2 - line_width / 2, y + height);
      //unico_cairo_draw_border_from_path (engine, cr, x + width / 2 - line_width / 2, y, line_width, height);
    }
}

static void
unico_draw_slider_button (DRAW_ARGS)
{
  unico_cairo_draw_background (engine, cr,
                               x, y, width, height,
                               0, gtk_theming_engine_get_junction_sides (engine));

  unico_cairo_draw_frame (engine, cr,
                          x, y, width, height,
                          0, gtk_theming_engine_get_junction_sides (engine));
}

static void
unico_draw_spinbutton_background (DRAW_ARGS)
{
  GtkJunctionSides junction;
  gdouble line_width;

  unico_get_line_width (engine, &line_width);

  junction = gtk_theming_engine_get_junction_sides (engine);

  if (!(junction & GTK_JUNCTION_CORNER_TOPRIGHT))
    y += line_width;

  unico_cairo_draw_background (engine, cr,
                               x, y,
                               width, height,
                               0, junction);
}

static void
unico_draw_spinbutton_frame (DRAW_ARGS)
{
  GtkJunctionSides junction;
  gdouble line_width;

  unico_get_line_width (engine, &line_width);

  junction = gtk_theming_engine_get_junction_sides (engine);

  if (!(junction & GTK_JUNCTION_CORNER_TOPRIGHT))
    y += line_width;

  unico_cairo_draw_frame (engine, cr,
                          x, y,
                          width, height,
                          0, junction);
}

static void
unico_draw_switch (DRAW_ARGS,
                   GtkOrientation orientation)
{
  unico_cairo_draw_background (engine, cr,
                               x, y, width, height,
                               0, gtk_theming_engine_get_junction_sides (engine));
  unico_cairo_draw_frame (engine, cr,
                          x, y, width, height,
                          0, gtk_theming_engine_get_junction_sides (engine));
}

static void
unico_draw_tab (DRAW_ARGS,
                GtkPositionType gap_side)
{
  GtkJunctionSides junction = 0;
  gdouble line_width;
  guint hidden_side = 0;
  gdouble offset = 0;

  unico_get_line_width (engine, &line_width);

  offset = 0;
  if (unico_has_outer_stroke (engine))
    offset = line_width * 2;

  cairo_save (cr);

  switch (gap_side)
  {
    case GTK_POS_TOP:
      junction = GTK_JUNCTION_TOP;
      hidden_side = SIDE_TOP;

      y -= offset;

      cairo_translate (cr, x + width, y + height);
      cairo_rotate (cr, G_PI);
      break;
    default:
    case GTK_POS_BOTTOM:
      junction = GTK_JUNCTION_BOTTOM;
      hidden_side = SIDE_BOTTOM;

      height += offset;

      cairo_translate (cr, x, y);
      break;
    case GTK_POS_LEFT:
      junction = GTK_JUNCTION_LEFT;
      hidden_side = SIDE_LEFT;

      x -= offset;

      cairo_translate (cr, x + width, y);
      cairo_rotate (cr, G_PI / 2);
      break;
    case GTK_POS_RIGHT:
      junction = GTK_JUNCTION_RIGHT;
      hidden_side = SIDE_RIGHT;

      width += offset;

      cairo_translate (cr, x, y + height);
      cairo_rotate (cr, - G_PI / 2);
      break;
  }

  if (gap_side == GTK_POS_TOP ||
      gap_side == GTK_POS_BOTTOM)
    unico_cairo_draw_background (engine, cr, 0, 0, width, height, 0, GTK_JUNCTION_BOTTOM);
  else
    unico_cairo_draw_background (engine, cr, 0, 0, height, width, 0, GTK_JUNCTION_BOTTOM);
  cairo_restore (cr);

  cairo_save (cr);

  unico_cairo_draw_frame (engine, cr, x, y, width, height, hidden_side, junction);

  cairo_restore (cr);
}

void
unico_register_style_default (UnicoStyleFunctions *functions)
{
  g_assert (functions);

  functions->draw_arrow                         = unico_draw_arrow;
  functions->draw_cell                          = unico_draw_cell;
  functions->draw_check                         = unico_draw_check;
  functions->draw_column_header_background      = unico_draw_column_header_background;
  functions->draw_column_header_frame           = unico_draw_column_header_frame;
  functions->draw_combo_button_background       = unico_draw_combo_button_background;
  functions->draw_combo_button_frame            = unico_draw_combo_button_frame;
  functions->draw_common_background             = unico_draw_common_background;
  functions->draw_common_frame                  = unico_draw_common_frame;
  functions->draw_expander                      = unico_draw_expander;
  functions->draw_focus                         = unico_draw_focus;
  functions->draw_frame_gap                     = unico_draw_frame_gap;
  functions->draw_grip                          = unico_draw_grip;
  functions->draw_icon_view                     = unico_draw_icon_view;
  functions->draw_menubaritem_background        = unico_draw_menubaritem_background;
  functions->draw_menubaritem_frame             = unico_draw_menubaritem_frame;
  functions->draw_notebook                      = unico_draw_notebook;
  functions->draw_pane_separator                = unico_draw_pane_separator;
  functions->draw_progressbar_fill_background   = unico_draw_progressbar_fill_background;
  functions->draw_progressbar_fill_frame        = unico_draw_progressbar_fill_frame;
  functions->draw_radio                         = unico_draw_radio;
  functions->draw_scrollbar_slider              = unico_draw_scrollbar_slider;
  functions->draw_scrollbar_stepper_background  = unico_draw_scrollbar_stepper_background;
  functions->draw_scrollbar_stepper_frame       = unico_draw_scrollbar_stepper_frame;
  functions->draw_scrolled_window_frame         = unico_draw_scrolled_window_frame;
  functions->draw_separator                     = unico_draw_separator;
  functions->draw_slider_button                 = unico_draw_slider_button;
  functions->draw_spinbutton_background         = unico_draw_spinbutton_background;
  functions->draw_spinbutton_frame              = unico_draw_spinbutton_frame;
  functions->draw_switch                        = unico_draw_switch;
  functions->draw_tab                           = unico_draw_tab;
}
