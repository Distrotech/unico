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
#include <gtk/gtk.h>

#include "math.h"
#include "unico-cairo-support.h"
#include "unico-draw.h"
#include "unico-support.h"
#include "unico-types.h"

/* draw a texture placed on the centroid */
static gboolean
draw_centroid_texture (GtkThemingEngine *engine,
                       cairo_t          *cr,
                       gdouble           x,
                       gdouble           y,
                       gdouble           width,
                       gdouble           height)
{
  GtkStateFlags state;
  GValue value = { 0, };
  cairo_pattern_t *texture = NULL;
  cairo_surface_t *surface = NULL;
  gboolean retval = FALSE;

  state = gtk_theming_engine_get_state (engine);

  gtk_theming_engine_get_property (engine, "-unico-centroid-texture", state, &value);

  if (!G_VALUE_HOLDS_BOXED (&value))
    return FALSE;

  texture = g_value_dup_boxed (&value);
  g_value_unset (&value);

  if (texture != NULL)
    cairo_pattern_get_surface (texture, &surface);

  if (surface != NULL)
    {
      cairo_save (cr);

      cairo_set_source_surface (cr, surface, (gint) (x + width / 2 - cairo_image_surface_get_width (surface) / 2),
                                             (gint) (y + height / 2 - cairo_image_surface_get_height (surface) / 2));
      cairo_paint (cr);

      cairo_restore (cr);

      retval = TRUE;
    }

  if (texture != NULL)
    cairo_pattern_destroy (texture);

  return retval;
}

static void
unico_draw_activity (DRAW_ARGS)
{
  /* playground for effects */
  unico_cairo_draw_background (engine, cr,
                               x, y, width, height,
                               0, gtk_theming_engine_get_junction_sides (engine));
  unico_cairo_draw_frame (engine, cr,
                          x, y, width, height,
                          0, gtk_theming_engine_get_junction_sides (engine));
}

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

  /* use floor function to adjust doubles */
  y = floor (y);
  x = floor (x);
  size = floor (size);

  size -= size_reduction;

  cairo_translate (cr, size_reduction / 2, size_reduction / 2);
  cairo_translate (cr, x + size / 2.0 + 0.5, y + size / 2.0 + 0.5);
  cairo_rotate (cr, angle - G_PI_2);
  cairo_translate (cr, (gint) (size / 4.0), 0);

  /* FIXME this + 1/- 1 is done to fix blurred diagonal lines.
   * I know it's not nice at all, but it fix a visual bug */
  cairo_move_to (cr, - size / 2.0, - size / 2.0);
  cairo_rel_line_to (cr, size / 2.0 + 1, size / 2.0);
  cairo_rel_line_to (cr, - size / 2.0 - 1, size / 2.0);
  cairo_close_path (cr);

  cairo_set_source_rgba (cr, color.red, color.green, color.blue, color.alpha * 0.75);
  cairo_fill_preserve (cr);

  gdk_cairo_set_source_rgba (cr, &color);
  cairo_stroke (cr);

  cairo_restore (cr);
}

static void
unico_draw_cell_background (DRAW_ARGS,
                            GtkRegionFlags flags)
{
  GtkJunctionSides junction;
  guint hidden_side;

  junction = GTK_JUNCTION_RIGHT | GTK_JUNCTION_LEFT;

  hidden_side = SIDE_RIGHT | SIDE_LEFT;

  if ((flags & GTK_REGION_FIRST) != 0)
    {
      junction &= ~(GTK_JUNCTION_CORNER_TOPLEFT | GTK_JUNCTION_CORNER_BOTTOMLEFT);
      hidden_side &= ~(SIDE_LEFT);
    }
  if ((flags & GTK_REGION_LAST) != 0)
    {
      junction &= ~(GTK_JUNCTION_CORNER_TOPRIGHT | GTK_JUNCTION_CORNER_BOTTOMRIGHT);
      hidden_side &= ~(SIDE_RIGHT);
    }

  unico_cairo_draw_background (engine, cr,
                               x, y, width, height,
                               hidden_side, junction);
}

static void
unico_draw_cell_frame (DRAW_ARGS,
                       GtkRegionFlags flags)
{
  GtkJunctionSides junction;
  guint hidden_side;

  junction = GTK_JUNCTION_RIGHT | GTK_JUNCTION_LEFT;

  hidden_side = SIDE_RIGHT | SIDE_LEFT;

  if ((flags & GTK_REGION_FIRST) != 0)
    {
      junction &= ~(GTK_JUNCTION_CORNER_TOPLEFT | GTK_JUNCTION_CORNER_BOTTOMLEFT);
      hidden_side &= ~(SIDE_LEFT);
    }
  if ((flags & GTK_REGION_LAST) != 0)
    {
      junction &= ~(GTK_JUNCTION_CORNER_TOPRIGHT | GTK_JUNCTION_CORNER_BOTTOMRIGHT);
      hidden_side &= ~(SIDE_RIGHT);
    }

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
          cairo_move_to (cr, 3, (gdouble) height / 2.0);
          cairo_line_to (cr, width - 3, (gdouble) height / 2.0);

          cairo_restore (cr);
        }
      else
        {
          cairo_translate (cr, x, y);

          if (in_menu)
            {
              cairo_scale (cr, (gdouble) width / 18.0, (gdouble) height / 18.0);
              cairo_translate (cr, 2.0, 3.0);
            }
          else
            {
              GdkRGBA *bullet_outline_color;

              gtk_theming_engine_get (engine, state,
                                      "-unico-bullet-outline-color", &bullet_outline_color,
                                      NULL);

              cairo_scale (cr, (gdouble) width / 18.0, (gdouble) height / 18.0);

              /* thick's outline */
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

          /* thick */
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
unico_draw_common (DRAW_ARGS)
{
  unico_cairo_draw_background (engine, cr,
                               x, y, width, height,
                               0, gtk_theming_engine_get_junction_sides (engine));
  unico_cairo_draw_frame (engine, cr,
                          x, y, width, height,
                          0, gtk_theming_engine_get_junction_sides (engine));
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

  /* use floor function to adjust doubles */
  size = floor (MIN (width, height));

  x += width / 2 - size / 2;
  y += height / 2 - size / 2;

  if ((state & GTK_STATE_FLAG_ACTIVE) == 0)
    angle = 0;

  cairo_translate (cr, x + size / 2.0 + 0.5, y + size / 2.0 + 0.5);
  cairo_rotate (cr, angle);
  cairo_translate (cr, (gint) (size / 4.0), 0);

  /* FIXME this + 1/- 1 is done to fix blurred diagonal lines.
   * I know it's not nice at all, but it fix a visual bug */
  cairo_move_to (cr, - size / 2.0, - size / 2.0);
  cairo_rel_line_to (cr, size / 2.0 + 1, size / 2.0);
  cairo_rel_line_to (cr, - size / 2.0 - 1, size / 2.0);
  cairo_close_path (cr);

  cairo_set_source_rgba (cr, color.red, color.green, color.blue, color.alpha * 0.75);
  cairo_fill_preserve (cr);

  gdk_cairo_set_source_rgba (cr, &color);
  cairo_stroke (cr);

  cairo_restore (cr);
}

static void
unico_draw_extension (DRAW_ARGS,
                      GtkPositionType gap_side)
{
  GtkBorder border;
  GtkBorder *outer_border;
  GtkJunctionSides junction = 0;
  GtkStateFlags state;
  gboolean has_outer_stroke = FALSE;
  gdouble bg_offset = 0;
  guint hidden_side = 0;

  state = gtk_theming_engine_get_state (engine); 

  gtk_theming_engine_get (engine, state,
                          "-unico-outer-stroke-width", &outer_border,
                          NULL);
  gtk_theming_engine_get_border (engine, state, &border);

  if (!unico_gtk_border_is_zero (outer_border))
    has_outer_stroke = TRUE;

  cairo_save (cr);

  /* FIXME doesn't work properly with not homogenuos border-width,
   * especially between tab and notebook.
   * I guess the issue comes from the fact draw_background
   * is looking at border dimensions while we're not,
   * or we're doing it wrong.
   * draw_background is looking at SIDE_BOTTOM and
   * sets border to 0 for this side */
  switch (gap_side)
  {
    case GTK_POS_TOP:
      junction = GTK_JUNCTION_TOP;
      hidden_side = SIDE_TOP;

      if (has_outer_stroke)
        {
          y -= outer_border->bottom;
          height += outer_border->bottom;
        }

      if ((state & GTK_STATE_FLAG_ACTIVE) != 0)
        bg_offset = border.bottom;

      cairo_translate (cr, x + width, y + height);
      cairo_rotate (cr, G_PI);
      break;
    default:
    case GTK_POS_BOTTOM:
      junction = GTK_JUNCTION_BOTTOM;
      hidden_side = SIDE_BOTTOM;

      if (has_outer_stroke)
        height += outer_border->top;

      if ((state & GTK_STATE_FLAG_ACTIVE) != 0)
        bg_offset = border.top;

      cairo_translate (cr, x, y);
      break;
    case GTK_POS_LEFT:
      junction = GTK_JUNCTION_LEFT;
      hidden_side = SIDE_LEFT;

      if (has_outer_stroke)
        {
          x -= outer_border->right;
          width += outer_border->right;
        }

      if ((state & GTK_STATE_FLAG_ACTIVE) != 0)
        bg_offset = border.right;

      cairo_translate (cr, x + width, y);
      cairo_rotate (cr, G_PI / 2);
      break;
    case GTK_POS_RIGHT:
      junction = GTK_JUNCTION_RIGHT;
      hidden_side = SIDE_RIGHT;

      if (has_outer_stroke)
        width += outer_border->left;

      if ((state & GTK_STATE_FLAG_ACTIVE) != 0)
        bg_offset = border.left;

      cairo_translate (cr, x, y + height);
      cairo_rotate (cr, - G_PI / 2);
      break;
  }

  if (gap_side == GTK_POS_TOP ||
      gap_side == GTK_POS_BOTTOM)
    unico_cairo_draw_background (engine, cr, 0, 0, width, height + bg_offset, SIDE_BOTTOM, GTK_JUNCTION_BOTTOM);
  else
    unico_cairo_draw_background (engine, cr, 0, 0, height, width + bg_offset, SIDE_BOTTOM, GTK_JUNCTION_BOTTOM);
  cairo_restore (cr);

  /* FIXME the frame on bottom bar has the wrong gradient,
   * while should be reflected */
  unico_cairo_draw_frame (engine, cr, x, y, width, height, hidden_side, junction);

  gtk_border_free (outer_border);
}

static void
unico_draw_focus (DRAW_ARGS)
{
  GdkRGBA *fill_color, *border_color, *outer_stroke_color;
  GtkStateFlags state;
  gint focus_pad, line_width;
  gint radius;

  state = gtk_theming_engine_get_state (engine);

  gtk_theming_engine_get (engine, state,
                          "-unico-focus-border-color", &border_color,
                          "-unico-focus-border-radius", &radius,
                          "-unico-focus-fill-color", &fill_color,
                          "-unico-focus-outer-stroke-color", &outer_stroke_color,
                          NULL);
  gtk_theming_engine_get_style (engine,
                                "focus-padding", &focus_pad,
                                "focus-line-width", &line_width,
                                NULL);  

  x += focus_pad;
  y += focus_pad;
  width -= focus_pad * 2;
  height -= focus_pad * 2;

  cairo_save (cr);

  cairo_set_line_width (cr, line_width);

  /* first layer, background */
  unico_cairo_round_rect (cr, x, y,
                              width, height,
                              radius, SIDE_ALL, GTK_JUNCTION_NONE);
  gdk_cairo_set_source_rgba (cr, fill_color);
  cairo_fill (cr);

  /* second layer, outer stroke */
  unico_cairo_round_rect_inner (cr, x - line_width, y - line_width,
                                    width + line_width * 2, height + line_width * 2,
                                    radius + 1, SIDE_ALL, GTK_JUNCTION_NONE);
  gdk_cairo_set_source_rgba (cr, outer_stroke_color);
  cairo_stroke (cr);

  /* third layer, border */
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
  GtkBorder border;
  GtkBorder *outer_border;
  GtkCssBorderCornerRadius *top_left_radius, *top_right_radius;
  GtkCssBorderCornerRadius *bottom_left_radius, *bottom_right_radius;
  GtkCssBorderRadius border_radius = { { 0, },  };
  GtkJunctionSides junction;
  GtkStateFlags state;
  gboolean has_outer_stroke = FALSE;
  gdouble x0, y0, x1, y1, xc, yc, wc, hc;

  xc = yc = wc = hc = 0;

  junction = gtk_theming_engine_get_junction_sides (engine);

  state = gtk_theming_engine_get_state (engine);

  gtk_theming_engine_get (engine, state,
                          /* Can't use border-radius as it's an int for
                           * backwards compat */
                          "border-top-left-radius", &top_left_radius,
                          "border-top-right-radius", &top_right_radius,
                          "border-bottom-right-radius", &bottom_right_radius,
                          "border-bottom-left-radius", &bottom_left_radius,
                          "-unico-outer-stroke-width", &outer_border,
                          NULL);
  gtk_theming_engine_get_border (engine, state, &border);

  if (!unico_gtk_border_is_zero (outer_border))
    has_outer_stroke = TRUE;

  if (top_left_radius)
    border_radius.top_left = *top_left_radius;
  g_free (top_left_radius);
  if (top_right_radius)
    border_radius.top_right = *top_right_radius;
  g_free (top_right_radius);
  if (bottom_right_radius)
    border_radius.bottom_right = *bottom_right_radius;
  g_free (bottom_right_radius);
  if (bottom_left_radius)
    border_radius.bottom_left = *bottom_left_radius;
  g_free (bottom_left_radius);

  cairo_save (cr);

  switch (gap_side)
  {
    case GTK_POS_TOP:
      xc = x + xy0_gap + border.left;
      yc = y;
      wc = MAX (xy1_gap - xy0_gap - (border.left + border.right), 0);
      hc = border.top * 2;

      if (has_outer_stroke)
        {
          xc += outer_border->left;
          wc = MAX (xy1_gap - xy0_gap - (outer_border->left + outer_border->right) - (border.left + border.right), 0);
          hc += outer_border->top;
        }

      if (xy0_gap < border_radius.top_left.horizontal)
        junction |= GTK_JUNCTION_CORNER_TOPLEFT;

      if (xy1_gap > width - border_radius.top_right.horizontal)
        junction |= GTK_JUNCTION_CORNER_TOPRIGHT;
      break;
    default:
    case GTK_POS_BOTTOM:
      xc = x + xy0_gap + border.left;
      yc = y + height - border.bottom * 2;
      wc = MAX (xy1_gap - xy0_gap - (border.left + border.right), 0);
      hc = border.bottom * 2;

      if (has_outer_stroke)
        {
          xc += outer_border->left;
          yc -= outer_border->bottom;
          wc = MAX (xy1_gap - xy0_gap - (outer_border->left + outer_border->right) - (border.left + border.right), 0);
          hc += outer_border->bottom;
        }

      if (xy0_gap < border_radius.bottom_left.horizontal)
        junction |= GTK_JUNCTION_CORNER_BOTTOMLEFT;

      if (xy1_gap > width - border_radius.bottom_right.horizontal)
        junction |= GTK_JUNCTION_CORNER_BOTTOMRIGHT;

      break;
    case GTK_POS_LEFT:
      xc = x;
      yc = y + xy0_gap + border.top;
      wc = border.left * 2;
      hc = MAX (xy1_gap - xy0_gap - (border.top + border.bottom), 0);

      if (has_outer_stroke)
        {
          yc += outer_border->top;
          wc += outer_border->left;
          hc = MAX (xy1_gap - xy0_gap - (outer_border->top + outer_border->bottom) - (border.top + border.bottom), 0);
        }

      if (xy0_gap < border_radius.top_left.vertical)
        junction |= GTK_JUNCTION_CORNER_TOPLEFT;

      if (xy1_gap > height - border_radius.bottom_left.vertical)
        junction |= GTK_JUNCTION_CORNER_BOTTOMLEFT;

      break;
    case GTK_POS_RIGHT:
      xc = x + width - border.right * 2;
      yc = y + xy0_gap + border.top;
      wc = border.right * 2;
      hc = MAX (xy1_gap - xy0_gap - (border.top + border.bottom), 0);

      if (has_outer_stroke)
        {
          xc -= outer_border->right;
          yc += outer_border->top;
          wc += outer_border->right;
          hc = MAX (xy1_gap - xy0_gap - (outer_border->top + outer_border->bottom) - (border.top + border.bottom), 0);
        }

      if (xy0_gap < border_radius.top_right.vertical)
        junction |= GTK_JUNCTION_CORNER_TOPRIGHT;

      if (xy1_gap > height - border_radius.bottom_right.vertical)
        junction |= GTK_JUNCTION_CORNER_BOTTOMRIGHT;

      break;
  }

  /* clip the gap */
  cairo_clip_extents (cr, &x0, &y0, &x1, &y1);
  cairo_rectangle (cr, x0, y0, x1 - x0, yc - y0);
  cairo_rectangle (cr, x0, yc, xc - x0, hc);
  cairo_rectangle (cr, xc + wc, yc, x1 - (xc + wc), hc);
  cairo_rectangle (cr, x0, yc + hc, x1 - x0, y1 - (yc + hc));
  cairo_clip (cr);

  /* draw the frame, gap area will not be drawn */
  unico_cairo_draw_frame (engine, cr, x, y, width, height, 0, junction);

  cairo_restore (cr);

  gtk_border_free (outer_border);
}

static void
unico_draw_grip (DRAW_ARGS)
{
  GdkRGBA border_color;
  GdkRGBA *inner_stroke_color;
  GtkStateFlags state;
  gint lx, ly;

  state = gtk_theming_engine_get_state (engine);

  gtk_theming_engine_get (engine, state,
                          "-unico-inner-stroke-color", &inner_stroke_color,
                          NULL);
  gtk_theming_engine_get_border_color (engine, state, &border_color);

  for (ly = 0; ly < 4; ly++)
    {
      /* vertically, four rows of dots */
      for (lx = 0; lx <= ly; lx++)
        {
          /* horizontally */
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
unico_draw_handle (DRAW_ARGS)
{
  gdouble line_width;
  gint i, bar_y, num_bars, bar_spacing, bar_width, bar_height;

  unico_cairo_draw_background (engine, cr,
                               x, y, width, height,
                               0, gtk_theming_engine_get_junction_sides (engine));

  if (draw_centroid_texture (engine, cr, x, y, width, height))
    return;

  unico_get_line_width (engine, &line_width);

  bar_y = 1;
  num_bars = 3;
  bar_spacing = 3;
  bar_width = 3;
  bar_height = num_bars * bar_spacing * line_width;

  cairo_save (cr);

  cairo_translate (cr, x + width / 2, y + height / 2);

  if (height > width)
    cairo_translate (cr, - bar_width / 2 - 0.5, - bar_height / 2 + 0.5);
  else
    {
      cairo_translate (cr, - bar_height / 2 + 0.5, bar_width / 2 + 0.5);
      cairo_rotate (cr, - G_PI / 2);
    }

  for (i = 0; i < num_bars; i++)
    {
      /* draw bars */
      cairo_move_to (cr, 0, bar_y);
      cairo_line_to (cr, bar_width, bar_y);
      unico_cairo_set_source_border (engine, cr, bar_width, 3);
      cairo_stroke (cr);

      cairo_move_to (cr, 0, bar_y + line_width);
      cairo_line_to (cr, bar_width, bar_y + line_width);
      unico_cairo_set_source_inner_stroke (engine, cr, bar_width, line_width);
      cairo_stroke (cr);

      bar_y += bar_spacing;
    }

  cairo_restore (cr);
}

static void
unico_draw_line (GtkThemingEngine *engine,
                 cairo_t          *cr,
                 gdouble           x0,
                 gdouble           y0,
                 gdouble           x1,
                 gdouble           y1)
{
  /* line endings */
  if (y0 == y1)
    {
      y0 += 0.5;
      y1 += 0.5;
      x0 += 0.5;
      x1 -= 0.5;
    }
  else if (x0 == x1)
    {
      x0 += 0.5;
      x1 += 0.5;
      y0 += 0.5;
      y1 -= 0.5;
    }

  cairo_move_to (cr, x0, y0);
  cairo_line_to (cr, x1, y1);
  unico_cairo_set_source_border (engine, cr, x1 - x0, y1 - y0);
  cairo_stroke (cr);
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
  unico_draw_frame_gap (engine, cr,
                        x, y, width, height,
                        gap_side, xy0_gap, xy1_gap);
}

static void
unico_draw_radio (DRAW_ARGS)
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

          cairo_set_line_cap(cr, CAIRO_LINE_CAP_ROUND);
          cairo_set_line_width (cr, 2.0);

          cairo_move_to(cr, 5, (gdouble) height / 2.0);
          cairo_line_to(cr, width - 5, (gdouble) height / 2.0);

          gdk_cairo_set_source_rgba (cr, bullet_color);
          cairo_stroke (cr);

          cairo_restore (cr);
        }
      else
        {
          if (in_menu)
            cairo_arc (cr, x + (gdouble) width / 2.0, y + (gdouble) height / 2.0,
                           (gdouble) (width + height) / 4.0 - 4, 0, G_PI * 2);
          else
            {
              GdkRGBA *bullet_outline_color;

              gtk_theming_engine_get (engine, state,
                                      "-unico-bullet-outline-color", &bullet_outline_color,
                                      NULL);

              /* bullet's outline */
              cairo_arc (cr, x + (gdouble) width / 2.0, y + (gdouble) height / 2.0,
                             (gdouble) (width + height) / 4.0 - 4, 0, G_PI * 2);
              gdk_cairo_set_source_rgba (cr, bullet_outline_color);
              cairo_fill (cr);

              cairo_arc (cr, x + (gdouble) width / 2.0, y + (gdouble) height / 2.0,
                             (gdouble) (width + height) / 4.0 - 5, 0, G_PI * 2);

              gdk_rgba_free (bullet_outline_color);
            }

          /* bullet */
          gdk_cairo_set_source_rgba (cr, bullet_color);
          cairo_fill (cr);
        }

      gdk_rgba_free (bullet_color);
    }
}

static void
unico_draw_separator (DRAW_ARGS)
{
  gdouble line_width;

  unico_get_line_width (engine, &line_width);

  if (line_width == 0)
    return;

  /* FIXME right code should be
   * if (gtk_theming_engine_has_class (engine, GTK_STYLE_CLASS_VERTICAL))
   * but doesn't work for separator tool item. */
  if (width > height)
    {
      cairo_move_to (cr, x, y + height / 2 + line_width / 2);
      cairo_line_to (cr, x + width, y + height / 2 + line_width / 2);
      unico_cairo_set_source_inner_stroke (engine, cr, width, line_width);
      cairo_stroke (cr);

      cairo_move_to (cr, x, y + height / 2 - line_width / 2);
      cairo_line_to (cr, x + width, y + height / 2 - line_width / 2);
      unico_cairo_set_source_border (engine, cr, width, line_width);
      cairo_stroke (cr);
    }
  else
    {
      cairo_move_to (cr, x + width / 2 + line_width / 2, y);
      cairo_line_to (cr, x + width / 2 + line_width / 2, y + height);
      unico_cairo_set_source_inner_stroke (engine, cr, line_width, height);
      cairo_stroke (cr);

      cairo_move_to (cr, x + width / 2 - line_width / 2, y);
      cairo_line_to (cr, x + width / 2 - line_width / 2, y + height);
      unico_cairo_set_source_border (engine, cr, line_width, height);
      cairo_stroke (cr);
    }
}

static void
unico_draw_slider (DRAW_ARGS,
                   GtkOrientation orientation)
{
  /* use orientation, if needed */
  unico_cairo_draw_background (engine, cr,
                               x, y, width, height,
                               0, gtk_theming_engine_get_junction_sides (engine));

  draw_centroid_texture (engine, cr, x, y, width, height);

  unico_cairo_draw_frame (engine, cr,
                          x, y, width, height,
                          0, gtk_theming_engine_get_junction_sides (engine));
}

static void
unico_draw_spinbutton_background (DRAW_ARGS)
{
  GtkBorder border;
  GtkJunctionSides junction;
  GtkStateFlags state;

  junction = gtk_theming_engine_get_junction_sides (engine);

  state = gtk_theming_engine_get_state (engine);

  gtk_theming_engine_get_border (engine, state, &border);

  /* FIXME this code always adds padding,
   * even when outer stroke for the spinbutton frame is none */
  if (!(junction & GTK_JUNCTION_CORNER_TOPRIGHT))
    y += border.top;

  unico_cairo_draw_background (engine, cr,
                               x, y, width, height,
                               0, junction);
}

static void
unico_draw_spinbutton_frame (DRAW_ARGS)
{
  GtkBorder border;
  GtkJunctionSides junction;
  GtkStateFlags state;

  junction = gtk_theming_engine_get_junction_sides (engine);

  state = gtk_theming_engine_get_state (engine);

  gtk_theming_engine_get_border (engine, state, &border);

  /* FIXME this code always adds padding,
   * even when outer stroke for the spinbutton frame is none */
  if (!(junction & GTK_JUNCTION_CORNER_TOPRIGHT))
    y += border.top;

  unico_cairo_draw_frame (engine, cr,
                          x, y, width, height,
                          0, junction);
}

void
unico_register_style_default (UnicoStyleFunctions *functions)
{
  g_assert (functions);

  functions->draw_activity                      = unico_draw_activity;
  functions->draw_arrow                         = unico_draw_arrow;
  functions->draw_cell_background               = unico_draw_cell_background;
  functions->draw_cell_frame                    = unico_draw_cell_frame;
  functions->draw_check                         = unico_draw_check;
  functions->draw_common                        = unico_draw_common;
  functions->draw_common_background             = unico_draw_common_background;
  functions->draw_common_frame                  = unico_draw_common_frame;
  functions->draw_expander                      = unico_draw_expander;
  functions->draw_extension                     = unico_draw_extension;
  functions->draw_focus                         = unico_draw_focus;
  functions->draw_frame_gap                     = unico_draw_frame_gap;
  functions->draw_grip                          = unico_draw_grip;
  functions->draw_handle                        = unico_draw_handle;
  functions->draw_line                          = unico_draw_line;
  functions->draw_notebook                      = unico_draw_notebook;
  functions->draw_radio                         = unico_draw_radio;
  functions->draw_separator                     = unico_draw_separator;
  functions->draw_slider                        = unico_draw_slider;
  functions->draw_spinbutton_background         = unico_draw_spinbutton_background;
  functions->draw_spinbutton_frame              = unico_draw_spinbutton_frame;
}
