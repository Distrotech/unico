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

#include "unico-cairo-support.h"
#include "unico-support.h"
#include "unico-types.h"
#include "raico-blur.h"

void
unico_cairo_draw_background (GtkThemingEngine *engine,
                             cairo_t          *cr,
                             gint              x,
                             gint              y,
                             gint              width,
                             gint              height,
                             guint             hidden_side,
                             GtkJunctionSides  junction)
{
  gdouble line_width;
  gdouble offset;
  gint radius;

  unico_get_line_width (engine, &line_width);
  unico_get_border_radius (engine, &radius);

  offset = 0;
  if (unico_has_outer_stroke (engine))
    offset = line_width;

  unico_cairo_draw_background_rect (engine, cr,
                                    x + offset, y + offset,
                                    width - offset * 2, height - offset * 2,
                                    radius, hidden_side, junction);

  unico_cairo_draw_glow (engine, cr,
                         x + offset, y + offset,
                         width - offset * 2, height - offset * 2,
                         radius, hidden_side, junction);
}

void
unico_cairo_draw_background_from_path (GtkThemingEngine *engine,
                                       cairo_t          *cr,
                                       gdouble           x,
                                       gdouble           y,
                                       gdouble           width,
                                       gdouble           height)
{
  GdkRGBA bg_color;
  GtkBorder border;
  GtkBorderStyle border_style;
  GtkStateFlags flags;
  cairo_matrix_t identity;
  cairo_pattern_t *bg_pat;
  gboolean running;
  gdouble progress, alpha = 1;
  gdouble mat_w, mat_h;
  gint radius, border_width;

  cairo_matrix_init_identity (&identity);

  /* FIXME really?
   * Original gtk theming engine code uses unmodified width and height
   * for pattern scaling, but here it is already modified
   * by unico_draw_background_rect. */
  mat_w = width;
  mat_h = height;

  flags = gtk_theming_engine_get_state (engine);
  gtk_theming_engine_get_background_color (engine, flags, &bg_color);
  gtk_theming_engine_get_border (engine, flags, &border);
  gtk_theming_engine_get (engine, flags,
                          "background-image", &bg_pat,
                          "border-radius", &radius,
                          "border-style", &border_style,
                          NULL);

  border_width = MIN (MIN (border.top, border.bottom),
                      MIN (border.left, border.right));

  cairo_save (cr);

  cairo_set_line_width (cr, border_width);

  cairo_translate (cr, x, y);

  running = gtk_theming_engine_state_is_running (engine, GTK_STATE_PRELIGHT, &progress);

  if (gtk_theming_engine_has_class (engine, "background"))
    {
      cairo_set_source_rgba (cr, 1.0, 1.0, 1.0, 0.0); /* transparent */
      cairo_set_operator (cr, CAIRO_OPERATOR_SOURCE);
      cairo_paint (cr);
    }

  if (running)
    {
      cairo_pattern_t *other_pat;
      GtkStateFlags other_flags;
      GdkRGBA other_bg;
      cairo_pattern_t *new_pat = NULL;

      if (flags & GTK_STATE_FLAG_PRELIGHT)
        {
          other_flags = flags & ~(GTK_STATE_FLAG_PRELIGHT);

          /* useful math function to use for a pulse loop animation could be
           * progress = 1 - MAX (1 - fabs ((progress - 0.5)*2), 0);
           * but we need to handle the not-running case (once animation finished),
           * otherwise the last frame will be the PRELIGHT at full opacity. */
          progress = 1 - progress;
        }
      else
        other_flags = flags | GTK_STATE_FLAG_PRELIGHT;

      gtk_theming_engine_get_background_color (engine, other_flags, &other_bg);
      gtk_theming_engine_get (engine, other_flags,
                              "background-image", &other_pat,
                              NULL);

      if (bg_pat && other_pat)
        {
          cairo_pattern_type_t type, other_type;
          gint n0, n1;

          cairo_pattern_get_color_stop_count (bg_pat, &n0);
          cairo_pattern_get_color_stop_count (other_pat, &n1);
          type = cairo_pattern_get_type (bg_pat);
          other_type = cairo_pattern_get_type (other_pat);

          if (type == other_type && n0 == n1)
            {
              gdouble offset0, red0, green0, blue0, alpha0;
              gdouble offset1, red1, green1, blue1, alpha1;
              gdouble x00, x01, y00, y01, x10, x11, y10, y11;
              gdouble r00, r01, r10, r11;
              gint i;

              if (type == CAIRO_PATTERN_TYPE_LINEAR)
                {
                  cairo_pattern_get_linear_points (bg_pat, &x00, &y00, &x01, &y01);
                  cairo_pattern_get_linear_points (other_pat, &x10, &y10, &x11, &y11);

                  new_pat = cairo_pattern_create_linear (x00 + (x10 - x00) * progress,
                                                         y00 + (y10 - y00) * progress,
                                                         x01 + (x11 - x01) * progress,
                                                         y01 + (y11 - y01) * progress);
                }
              else
                {
                  cairo_pattern_get_radial_circles (bg_pat, &x00, &y00, &r00, &x01, &y01, &r01);
                  cairo_pattern_get_radial_circles (other_pat, &x10, &y10, &r10, &x11, &y11, &r11);

                  new_pat = cairo_pattern_create_radial (x00 + (x10 - x00) * progress,
                                                         y00 + (y10 - y00) * progress,
                                                         r00 + (r10 - r00) * progress,
                                                         x01 + (x11 - x01) * progress,
                                                         y01 + (y11 - y01) * progress,
                                                         r01 + (r11 - r01) * progress);
                }

              cairo_pattern_set_filter (new_pat, CAIRO_FILTER_FAST);
              i = 0;

              /* Blend both gradients into one */
              while (i < n0 && i < n1)
                {
                  cairo_pattern_get_color_stop_rgba (bg_pat, i,
                                                     &offset0,
                                                     &red0, &green0, &blue0,
                                                     &alpha0);
                  cairo_pattern_get_color_stop_rgba (other_pat, i,
                                                     &offset1,
                                                     &red1, &green1, &blue1,
                                                     &alpha1);
                  cairo_pattern_add_color_stop_rgba (new_pat,
                                                     offset0 + ((offset1 - offset0) * progress),
                                                     red0 + ((red1 - red0) * progress),
                                                     green0 + ((green1 - green0) * progress),
                                                     blue0 + ((blue1 - blue0) * progress),
                                                     alpha0 + ((alpha1 - alpha0) * progress));
                  i++;
                }
            }
          else
            {
              /* Different pattern types, or different color
               * stop counts, alpha blend both patterns.
               */
              unico_cairo_style_pattern_set_matrix (other_pat, width, height);
              cairo_set_source (cr, other_pat);
              cairo_fill_preserve (cr);

              cairo_pattern_set_matrix (other_pat, &identity);

              /* Set alpha for posterior drawing
               * of the target pattern
               */
              alpha = 1 - progress;
            }
        }
      else if (bg_pat || other_pat)
        {
          const GdkRGBA *c;
          cairo_pattern_t *p;
          gdouble x0, y0, x1, y1, r0, r1;
          gint n, i;

          /* Blend a pattern with a color */
          if (bg_pat)
            {
              p = bg_pat;
              c = &other_bg;
              progress = 1 - progress;
            }
          else
            {
              p = other_pat;
              c = &bg_color;
            }

          if (cairo_pattern_get_type (p) == CAIRO_PATTERN_TYPE_LINEAR)
            {
              cairo_pattern_get_linear_points (p, &x0, &y0, &x1, &y1);
              new_pat = cairo_pattern_create_linear (x0, y0, x1, y1);
            }
          else
            {
              cairo_pattern_get_radial_circles (p, &x0, &y0, &r0, &x1, &y1, &r1);
              new_pat = cairo_pattern_create_radial (x0, y0, r0, x1, y1, r1);
            }

          cairo_pattern_get_color_stop_count (p, &n);

          for (i = 0; i < n; i++)
            {
              gdouble red1, green1, blue1, alpha1;
              gdouble offset;

              cairo_pattern_get_color_stop_rgba (p, i,
                                                 &offset,
                                                 &red1, &green1, &blue1,
                                                 &alpha1);
              cairo_pattern_add_color_stop_rgba (new_pat, offset,
                                                 c->red + ((red1 - c->red) * progress),
                                                 c->green + ((green1 - c->green) * progress),
                                                 c->blue + ((blue1 - c->blue) * progress),
                                                 c->alpha + ((alpha1 - c->alpha) * progress));
            }
        }
      else
        {
          /* Merge just colors */
          new_pat = cairo_pattern_create_rgba (CLAMP (bg_color.red + ((other_bg.red - bg_color.red) * progress), 
                                                      0, 1),
                                               CLAMP (bg_color.green + ((other_bg.green - bg_color.green) * progress),
                                                      0, 1),
                                               CLAMP (bg_color.blue + ((other_bg.blue - bg_color.blue) * progress),
                                                      0, 1),
                                               CLAMP (bg_color.alpha + ((other_bg.alpha - bg_color.alpha) * progress),
                                                      0, 1));
        }

      if (new_pat)
        {
          /* Replace pattern to use */
          cairo_pattern_destroy (bg_pat);
          bg_pat = new_pat;
        }

      if (other_pat)
        cairo_pattern_destroy (other_pat);
    }

  if (bg_pat)
    {
      unico_cairo_style_pattern_set_matrix (bg_pat, mat_w, mat_h);
      cairo_set_source (cr, bg_pat);
    }
  else
    gdk_cairo_set_source_rgba (cr, &bg_color);

  if (alpha == 1)
    {
      if (border_width > 1 &&
          border_style != GTK_BORDER_STYLE_NONE)
        {
          /* stroke with the same source, so the background
           * has exactly the shape than the frame, this
           * is important so gtk_render_background() and
           * gtk_render_frame() fit perfectly with round
           * borders.
           */
          cairo_fill_preserve (cr);
          cairo_stroke (cr);
        }
      else
        cairo_fill (cr);
    }
  else
    {
      cairo_save (cr);

      cairo_clip_preserve (cr);
      cairo_paint_with_alpha (cr, alpha);

      cairo_restore (cr);
    }

  if (bg_pat)
    {
      cairo_pattern_set_matrix (bg_pat, &identity);
      cairo_pattern_destroy (bg_pat);
    }

  cairo_restore (cr);
}

void
unico_cairo_draw_background_rect (GtkThemingEngine *engine,
                                  cairo_t          *cr,
                                  gdouble           x,
                                  gdouble           y,
                                  gdouble           width,
                                  gdouble           height,
                                  gint              radius,
                                  guint             hidden_side,
                                  GtkJunctionSides  junction)
{
  GtkBorder border;
  GtkBorderStyle border_style;
  GtkStateFlags flags;
  gint border_width;

  flags = gtk_theming_engine_get_state (engine);
  gtk_theming_engine_get_border (engine, flags, &border);
  gtk_theming_engine_get (engine, flags,
                          "border-style", &border_style,
                          NULL);

  border_width = MIN (MIN (border.top, border.bottom),
                      MIN (border.left, border.right));

  /* FIXME I don't like this */
  if (border_width > 1 &&
      border_style == GTK_BORDER_STYLE_NONE)
    {
      x += (gdouble) border_width / 2;
      y += (gdouble) border_width / 2;
      width -= border_width;
      height -= border_width;
    }
  else
    {
      x += border.left;
      y += border.top;
      width -= border.left + border.right;
      height -= border.top + border.bottom;
    }

  if (width <= 0 || height <= 0)
    return;

  unico_cairo_round_rect (cr, x, y, width, height, radius, SIDE_ALL & ~(hidden_side), junction);
  unico_cairo_draw_background_from_path (engine, cr, x, y, width, height);
}

void
unico_cairo_draw_border_from_path (GtkThemingEngine *engine,
                                   cairo_t          *cr,
                                   gdouble           x,
                                   gdouble           y,
                                   gdouble           width,
                                   gdouble           height)
{
  GdkRGBA border_color;
  GtkBorderStyle border_style;
  GtkStateFlags flags;
  cairo_matrix_t identity;
  cairo_pattern_t *border_pat;
  gboolean running;
  gint radius;
  gdouble line_width, progress;

  cairo_matrix_init_identity (&identity);

  flags = gtk_theming_engine_get_state (engine);
  gtk_theming_engine_get_border_color (engine, flags, &border_color);
  gtk_theming_engine_get (engine, flags,
                          "border-style", &border_style,
                          "border-radius", &radius,
                          "-unico-border-gradient", &border_pat,
                          NULL);

  unico_get_line_width (engine, &line_width);

  cairo_save (cr);
  cairo_set_line_width (cr, line_width);
  cairo_translate (cr, x, y);

  running = gtk_theming_engine_state_is_running (engine, GTK_STATE_PRELIGHT, &progress);

  if (running)
    {
      cairo_pattern_t *other_pat;
      GtkStateFlags other_flags;
      GdkRGBA other_color;
      cairo_pattern_t *new_pat = NULL;

      if (flags & GTK_STATE_FLAG_PRELIGHT)
        {
          other_flags = flags & ~(GTK_STATE_FLAG_PRELIGHT);
          progress = 1 - progress;
        }
      else
        other_flags = flags | GTK_STATE_FLAG_PRELIGHT;

      gtk_theming_engine_get_border_color (engine, other_flags, &other_color);
      gtk_theming_engine_get (engine, other_flags,
                              "-unico-border-gradient", &other_pat,
                              NULL);

      if (border_pat && other_pat)
        {
          cairo_pattern_type_t type, other_type;
          gint n0, n1;

          cairo_pattern_get_color_stop_count (border_pat, &n0);
          cairo_pattern_get_color_stop_count (other_pat, &n1);
          type = cairo_pattern_get_type (border_pat);
          other_type = cairo_pattern_get_type (other_pat);

          if (type == other_type && n0 == n1)
            {
              gdouble offset0, red0, green0, blue0, alpha0;
              gdouble offset1, red1, green1, blue1, alpha1;
              gdouble x00, x01, y00, y01, x10, x11, y10, y11;
              gdouble r00, r01, r10, r11;
              gint i;

              if (type == CAIRO_PATTERN_TYPE_LINEAR)
                {
                  cairo_pattern_get_linear_points (border_pat, &x00, &y00, &x01, &y01);
                  cairo_pattern_get_linear_points (other_pat, &x10, &y10, &x11, &y11);

                  new_pat = cairo_pattern_create_linear (x00 + (x10 - x00) * progress,
                                                         y00 + (y10 - y00) * progress,
                                                         x01 + (x11 - x01) * progress,
                                                         y01 + (y11 - y01) * progress);
                }
              else
                {
                  cairo_pattern_get_radial_circles (border_pat, &x00, &y00, &r00, &x01, &y01, &r01);
                  cairo_pattern_get_radial_circles (other_pat, &x10, &y10, &r10, &x11, &y11, &r11);

                  new_pat = cairo_pattern_create_radial (x00 + (x10 - x00) * progress,
                                                         y00 + (y10 - y00) * progress,
                                                         r00 + (r10 - r00) * progress,
                                                         x01 + (x11 - x01) * progress,
                                                         y01 + (y11 - y01) * progress,
                                                         r01 + (r11 - r01) * progress);
                }

              cairo_pattern_set_filter (new_pat, CAIRO_FILTER_FAST);
              i = 0;

              /* Blend both gradients into one */
              while (i < n0 && i < n1)
                {
                  cairo_pattern_get_color_stop_rgba (border_pat, i,
                                                     &offset0,
                                                     &red0, &green0, &blue0,
                                                     &alpha0);
                  cairo_pattern_get_color_stop_rgba (other_pat, i,
                                                     &offset1,
                                                     &red1, &green1, &blue1,
                                                     &alpha1);
                  cairo_pattern_add_color_stop_rgba (new_pat,
                                                     offset0 + ((offset1 - offset0) * progress),
                                                     red0 + ((red1 - red0) * progress),
                                                     green0 + ((green1 - green0) * progress),
                                                     blue0 + ((blue1 - blue0) * progress),
                                                     alpha0 + ((alpha1 - alpha0) * progress));
                  i++;
                }
            }
        }
      else if (border_pat || other_pat)
        {
          const GdkRGBA *c;
          cairo_pattern_t *p;
          gdouble x0, y0, x1, y1, r0, r1;
          gint n, i;

          /* Blend a pattern with a color */
          if (border_pat)
            {
              p = border_pat;
              c = &other_color;
              progress = 1 - progress;
            }
          else
            {
              p = other_pat;
              c = &border_color;
            }

          if (cairo_pattern_get_type (p) == CAIRO_PATTERN_TYPE_LINEAR)
            {
              cairo_pattern_get_linear_points (p, &x0, &y0, &x1, &y1);
              new_pat = cairo_pattern_create_linear (x0, y0, x1, y1);
            }
          else
            {
              cairo_pattern_get_radial_circles (p, &x0, &y0, &r0, &x1, &y1, &r1);
              new_pat = cairo_pattern_create_radial (x0, y0, r0, x1, y1, r1);
            }

          cairo_pattern_get_color_stop_count (p, &n);

          for (i = 0; i < n; i++)
            {
              gdouble red1, green1, blue1, alpha1;
              gdouble offset;

              cairo_pattern_get_color_stop_rgba (p, i,
                                                 &offset,
                                                 &red1, &green1, &blue1,
                                                 &alpha1);
              cairo_pattern_add_color_stop_rgba (new_pat, offset,
                                                 c->red + ((red1 - c->red) * progress),
                                                 c->green + ((green1 - c->green) * progress),
                                                 c->blue + ((blue1 - c->blue) * progress),
                                                 c->alpha + ((alpha1 - c->alpha) * progress));
            }
        }
      else
        {
          border_color.red = CLAMP (border_color.red + ((other_color.red - border_color.red) * progress), 0, 1);
          border_color.green = CLAMP (border_color.green + ((other_color.green - border_color.green) * progress), 0, 1);
          border_color.blue = CLAMP (border_color.blue + ((other_color.blue - border_color.blue) * progress), 0, 1);
          border_color.alpha = CLAMP (border_color.alpha + ((other_color.alpha - border_color.alpha) * progress), 0, 1);
        }

      if (new_pat)
        {
          /* Replace pattern to use */
          cairo_pattern_destroy (border_pat);
          border_pat = new_pat;
        }

      if (other_pat)
        cairo_pattern_destroy (other_pat);
    }

  if (border_pat)
    {
      unico_cairo_style_pattern_set_matrix (border_pat, width - line_width, height - line_width);
      cairo_set_source (cr, border_pat);
    }
  else
    gdk_cairo_set_source_rgba (cr, &border_color);

  switch (border_style)
  {
    default:
    case GTK_BORDER_STYLE_NONE:
      break;
    /* FIXME Need to implement those two styles. */
    case GTK_BORDER_STYLE_INSET:
    case GTK_BORDER_STYLE_OUTSET:
    case GTK_BORDER_STYLE_SOLID:
      cairo_stroke (cr);
      break;
  }

  if (border_pat)
    {
      cairo_pattern_set_matrix (border_pat, &identity);
      cairo_pattern_destroy (border_pat);
    }

  cairo_restore (cr);
}

void
unico_cairo_draw_border_rect (GtkThemingEngine *engine,
                              cairo_t          *cr,
                              gdouble           x,
                              gdouble           y,
                              gdouble           width,
                              gdouble           height,
                              gint              radius,
                              guint             hidden_side,
                              GtkJunctionSides  junction)
{
  gdouble line_width;

  unico_get_line_width (engine, &line_width);

  cairo_save (cr);

  cairo_set_line_width (cr, line_width);

  unico_cairo_round_rect_inner (cr, x, y, width, height, radius, SIDE_ALL & ~(hidden_side), junction);
  unico_cairo_draw_border_from_path (engine, cr, x, y, width, height);

  cairo_restore (cr);
}

void
unico_cairo_draw_inner_stroke_from_path (GtkThemingEngine *engine,
                                         cairo_t          *cr,
                                         gdouble           x,
                                         gdouble           y,
                                         gdouble           width,
                                         gdouble           height)
{
  GdkRGBA *inner_stroke_color;
  GtkStateFlags flags;
  cairo_pattern_t *inner_stroke_pat;
  gdouble line_width;

  flags = gtk_theming_engine_get_state (engine);
  gtk_theming_engine_get (engine, flags,
                          "-unico-inner-stroke-color", &inner_stroke_color,
                          "-unico-inner-stroke-gradient", &inner_stroke_pat,
                          NULL);

  cairo_save (cr);

  cairo_translate (cr, x, y);

  line_width = cairo_get_line_width (cr);

  if (inner_stroke_pat)
    {
      unico_cairo_style_pattern_set_matrix (inner_stroke_pat, width - line_width, height - line_width);
      cairo_set_source (cr, inner_stroke_pat);
    }
  else
    gdk_cairo_set_source_rgba (cr, inner_stroke_color);

  cairo_stroke (cr);

  cairo_restore (cr);

  if (inner_stroke_pat != NULL)
    cairo_pattern_destroy (inner_stroke_pat);

  gdk_rgba_free (inner_stroke_color);
}

void
unico_cairo_draw_inner_stroke_rect (GtkThemingEngine *engine,
                                    cairo_t          *cr,
                                    gdouble           x,
                                    gdouble           y,
                                    gdouble           width,
                                    gdouble           height,
                                    gint              radius,
                                    guint             hidden_side,
                                    GtkJunctionSides  junction)
{
  gdouble line_width;

  unico_get_line_width (engine, &line_width);

  cairo_save (cr);

  cairo_set_line_width (cr, line_width);

  unico_cairo_round_rect_inner (cr, x, y, width, height, radius, SIDE_ALL & ~(hidden_side), junction);
  unico_cairo_draw_inner_stroke_from_path (engine, cr, x, y, width, height);

  cairo_restore (cr);
}

void
unico_cairo_draw_outer_stroke_from_path (GtkThemingEngine *engine,
                                         cairo_t          *cr,
                                         gdouble           x,
                                         gdouble           y,
                                         gdouble           width,
                                         gdouble           height)
{
  GdkRGBA *outer_stroke_color;
  GtkStateFlags flags;
  UnicoStrokeStyle outer_stroke_style;
  cairo_pattern_t *outer_stroke_pat;
  gdouble line_width;

  flags = gtk_theming_engine_get_state (engine);
  gtk_theming_engine_get (engine, flags,
                          "-unico-outer-stroke-color", &outer_stroke_color,
                          "-unico-outer-stroke-gradient", &outer_stroke_pat,
                          "-unico-outer-stroke-style", &outer_stroke_style,
                          NULL);

  cairo_save (cr);

  cairo_translate (cr, x, y);

  line_width = cairo_get_line_width (cr);

  if (outer_stroke_style != UNICO_STROKE_STYLE_NONE)
    {
      if (outer_stroke_pat)
        {
          unico_cairo_style_pattern_set_matrix (outer_stroke_pat, width - line_width, height - line_width);
          cairo_set_source (cr, outer_stroke_pat);
        }
      else
        gdk_cairo_set_source_rgba (cr, outer_stroke_color);

      cairo_stroke (cr);
    }

  cairo_restore (cr);

  if (outer_stroke_pat != NULL)
    cairo_pattern_destroy (outer_stroke_pat);

  gdk_rgba_free (outer_stroke_color);
}

void
unico_cairo_draw_frame (GtkThemingEngine *engine,
                        cairo_t          *cr,
                        gint              x,
                        gint              y,
                        gint              width,
                        gint              height,
                        guint             hidden_side,
                        GtkJunctionSides  junction)
{
  gdouble line_width;
  gdouble offset;
  gint radius;

  unico_get_line_width (engine, &line_width);
  unico_get_border_radius (engine, &radius);

  offset = 0;
  if (unico_has_outer_stroke (engine))
    {
      offset = line_width;

      unico_cairo_draw_outer_stroke_rect (engine, cr,
                                          x, y,
                                          width, height,
                                          radius + line_width, hidden_side, junction);
    }

  unico_cairo_draw_inner_stroke_rect (engine, cr,
                                      x + offset + line_width, y + offset + line_width,
                                      width - offset * 2 - line_width * 2, height - offset * 2 - line_width * 2,
                                      radius - line_width, hidden_side, junction);

  unico_cairo_draw_border_rect (engine, cr,
                                x + offset, y + offset,
                                width - offset * 2, height - offset * 2,
                                radius, hidden_side, junction);
}

void
unico_cairo_draw_glow (GtkThemingEngine *engine,
                       cairo_t          *cr,
                       gint              x,
                       gint              y,
                       gint              width,
                       gint              height,
                       gint              radius,
                       guint             hidden_side,
                       GtkJunctionSides  junction)
{
  GdkRGBA *glow_color;
  GtkStateFlags flags;
  cairo_t *cr_surface; 
  cairo_surface_t *surface;
  raico_blur_t* blur = NULL;
  gint bradius = 0;
  gdouble line_width;

  unico_get_line_width (engine, &line_width);

  flags = gtk_theming_engine_get_state (engine);
  gtk_theming_engine_get (engine, flags,
                          "-unico-glow-radius", &bradius,
                          "-unico-glow-color", &glow_color,
                          NULL);

  if (bradius <= 0)
    {
      gdk_rgba_free (glow_color);
      return;
    }

  /* clip the area */
  cairo_save (cr);
  unico_cairo_round_rect (cr, x, y,
                              width, height,
                              radius, SIDE_ALL & ~(hidden_side), junction);
  cairo_clip (cr);

  /* create the surface to blur */
  surface = cairo_image_surface_create (CAIRO_FORMAT_ARGB32,
                                        width + bradius * 2,
                                        height + bradius * 2);
  cr_surface = cairo_create (surface); 

  cairo_set_line_width (cr_surface, line_width * 4.0);

  /* draw a rounded rectangle on the surface to blur */
  unico_cairo_round_rect (cr_surface, bradius, bradius, width, height, radius, SIDE_ALL & ~(hidden_side), junction);
  gdk_cairo_set_source_rgba (cr_surface, glow_color);
  cairo_stroke (cr_surface);

  /* create and apply raico blur */
  blur = raico_blur_create ();
  raico_blur_set_radius (blur, bradius);
  raico_blur_apply (blur, surface);

  /* paint the blurred surface to cr */
  cairo_set_source_surface (cr, surface, x - bradius, y - bradius); 
  cairo_paint (cr);

  cairo_surface_destroy (surface); 
  cairo_destroy (cr_surface);

  gdk_rgba_free (glow_color);

  cairo_restore (cr);
}

void
unico_cairo_draw_outer_stroke_rect (GtkThemingEngine *engine,
                                    cairo_t          *cr,
                                    gdouble           x,
                                    gdouble           y,
                                    gdouble           width,
                                    gdouble           height,
                                    gint              radius,
                                    guint             hidden_side,
                                    GtkJunctionSides  junction)
{
  gdouble line_width;

  unico_get_line_width (engine, &line_width);

  cairo_save (cr);

  cairo_set_line_width (cr, line_width);

  unico_cairo_round_rect_inner (cr, x, y, width, height, radius, SIDE_ALL & ~(hidden_side), junction);
  unico_cairo_draw_outer_stroke_from_path (engine, cr, x, y, width, height);

  cairo_restore (cr);
}

void
unico_cairo_exchange_axis (cairo_t *cr,
                           gint    *x,
                           gint    *y,
                           gint    *width,
                           gint    *height)
{
  gint tmp;
  cairo_matrix_t matrix;

  cairo_translate (cr, *x, *y);
  cairo_matrix_init (&matrix, 0, 1, 1, 0, 0, 0);

  cairo_transform (cr, &matrix);

  /* swap width/height */
  tmp = *width;
  *x = 0;
  *y = 0;
  *width = *height;
  *height = tmp;
}
#if 0
void
unico_cairo_round_rect (cairo_t         *cr,
                        gdouble          x,
                        gdouble          y,
                        gdouble          width,
                        gdouble          height,
                        gint             radius,
                        guint            sides,
                        GtkJunctionSides junction)
{
  radius = CLAMP (radius, 0, MIN (width / 2, height / 2));

  if (sides & SIDE_RIGHT)
    {
      if (radius == 0 ||
          (junction & GTK_JUNCTION_CORNER_TOPRIGHT))
        cairo_move_to (cr, x + width, y);
      else
        {
          cairo_new_sub_path (cr);
          cairo_arc (cr, x + width - radius, y + radius, radius, - G_PI / 4, 0);
        }

      if (radius == 0 ||
          (junction & GTK_JUNCTION_CORNER_BOTTOMRIGHT))
        cairo_line_to (cr, x + width, y + height);
      else
        cairo_arc (cr, x + width - radius, y + height - radius, radius, 0, G_PI / 4);
    }

  if (sides & SIDE_BOTTOM)
    {
      if (radius != 0 &&
          ! (junction & GTK_JUNCTION_CORNER_BOTTOMRIGHT))
        {
          if ((sides & SIDE_RIGHT) == 0)
            cairo_new_sub_path (cr);

          cairo_arc (cr, x + width - radius, y + height - radius, radius, G_PI / 4, G_PI / 2);
        }
      else if ((sides & SIDE_RIGHT) == 0)
        cairo_move_to (cr, x + width, y + height);

      if (radius == 0 ||
          (junction & GTK_JUNCTION_CORNER_BOTTOMLEFT))
        cairo_line_to (cr, x, y + height);
      else
        cairo_arc (cr, x + radius, y + height - radius, radius, G_PI / 2, 3 * (G_PI / 4));
    }
  else
    cairo_move_to (cr, x, y + height);

  if (sides & SIDE_LEFT)
    {
      if (radius != 0 &&
          ! (junction & GTK_JUNCTION_CORNER_BOTTOMLEFT))
        {
          if ((sides & SIDE_BOTTOM) == 0)
            cairo_new_sub_path (cr);

          cairo_arc (cr, x + radius, y + height - radius, radius, 3 * (G_PI / 4), G_PI);
        }
      else if ((sides & SIDE_BOTTOM) == 0)
        cairo_move_to (cr, x, y + height);

      if (radius == 0 ||
          (junction & GTK_JUNCTION_CORNER_TOPLEFT))
        cairo_line_to (cr, x, y);
      else
        cairo_arc (cr, x + radius, y + radius, radius, G_PI, G_PI + G_PI / 4);
    }

  if (sides & SIDE_TOP)
    {
      if (radius != 0 &&
          ! (junction & GTK_JUNCTION_CORNER_TOPLEFT))
        {
          if ((sides & SIDE_LEFT) == 0)
            cairo_new_sub_path (cr);

          cairo_arc (cr, x + radius, y + radius, radius, 5 * (G_PI / 4), 3 * (G_PI / 2));
        }
      else if ((sides & SIDE_LEFT) == 0)
        cairo_move_to (cr, x, y);

      if (radius == 0 ||
          (junction & GTK_JUNCTION_CORNER_TOPRIGHT))
        cairo_line_to (cr, x + width, y);
      else
        cairo_arc (cr, x + width - radius, y + radius, radius, 3 * (G_PI / 2), - G_PI / 4);
    }
}
#endif
void
unico_cairo_round_rect (cairo_t         *cr,
                        gdouble          x,
                        gdouble          y,
                        gdouble          width,
                        gdouble          height,
                        gint             radius,
                        guint            sides,
                        GtkJunctionSides junction)
{
  radius = CLAMP (radius, 0, MIN (width / 2, height / 2));

  if (sides & SIDE_RIGHT)
    {
      if (radius == 0 ||
          (junction & GTK_JUNCTION_CORNER_TOPRIGHT))
        cairo_move_to (cr, x + width, y);
      else
        {
          cairo_new_sub_path (cr);
          
          cairo_arc (cr, x + width - radius, y + radius, radius, - G_PI / 4, 0);
        }

      if (radius == 0 ||
          (junction & GTK_JUNCTION_CORNER_BOTTOMRIGHT))
        cairo_line_to (cr, x + width, y + height);
      else
        cairo_arc (cr, x + width - radius, y + height - radius, radius, 0, G_PI / 4);
    }

  if (sides & SIDE_BOTTOM)
    {
      if (radius != 0 &&
          ! (junction & GTK_JUNCTION_CORNER_BOTTOMRIGHT))
        {
          if ((sides & SIDE_RIGHT) == 0)
            cairo_new_sub_path (cr);

          cairo_arc (cr, x + width - radius, y + height - radius, radius, G_PI / 4, G_PI / 2);
        }
      else if ((sides & SIDE_RIGHT) == 0)
        cairo_move_to (cr, x + width, y + height);

      if (radius == 0 ||
          (junction & GTK_JUNCTION_CORNER_BOTTOMLEFT))
        cairo_line_to (cr, x, y + height);
      else
        cairo_arc (cr, x + radius, y + height - radius, radius, G_PI / 2, 3 * (G_PI / 4));
    }
  else
    cairo_move_to (cr, x, y + height);

  if (sides & SIDE_LEFT)
    {
      if (radius != 0 &&
          ! (junction & GTK_JUNCTION_CORNER_BOTTOMLEFT))
        {
          if ((sides & SIDE_BOTTOM) == 0)
            cairo_new_sub_path (cr);

          cairo_arc (cr, x + radius, y + height - radius, radius, 3 * (G_PI / 4), G_PI);
        }
      else if ((sides & SIDE_BOTTOM) == 0)
        cairo_move_to (cr, x, y + height);

      if (radius == 0 ||
          (junction & GTK_JUNCTION_CORNER_TOPLEFT))
        cairo_line_to (cr, x, y);
      else
        cairo_arc (cr, x + radius, y + radius, radius, G_PI, G_PI + G_PI / 4);
    }

  if (sides & SIDE_TOP)
    {
      if (radius != 0 &&
          ! (junction & GTK_JUNCTION_CORNER_TOPLEFT))
        {
          if ((sides & SIDE_LEFT) == 0)
            cairo_new_sub_path (cr);

          cairo_arc (cr, x + radius, y + radius, radius, 5 * (G_PI / 4), 3 * (G_PI / 2));
        }
      else if ((sides & SIDE_LEFT) == 0)
        cairo_move_to (cr, x, y);

      if (radius == 0 ||
          (junction & GTK_JUNCTION_CORNER_TOPRIGHT))
        cairo_line_to (cr, x + width, y);
      else
        cairo_arc (cr, x + width - radius, y + radius, radius, 3 * (G_PI / 2), - G_PI / 4);
    }
}

void
unico_cairo_round_rect_inner (cairo_t         *cr,
                              gdouble          x,
                              gdouble          y,
                              gdouble          width,
                              gdouble          height,
                              gint             radius,
                              guint            sides,
                              GtkJunctionSides junction)
{
  gdouble line_width;

  line_width = cairo_get_line_width (cr);

  unico_cairo_round_rect (cr, x + (line_width / 2.0),
                          y + (line_width / 2.0),
                          width - line_width,
                          height - line_width,
                          radius, sides, junction);
}

void
unico_cairo_set_source_color_with_alpha (cairo_t *cr,
                                         GdkRGBA *color,
                                         gdouble  alpha)
{
  g_return_if_fail (cr && color);

  cairo_set_source_rgba (cr, color->red, color->green, color->blue, alpha);
}

void
unico_cairo_style_pattern_set_matrix (cairo_pattern_t *pat,
                                      gdouble          width,
                                      gdouble          height)
{
  cairo_matrix_t matrix;
  gint w, h;

  if (cairo_pattern_get_type (pat) == CAIRO_PATTERN_TYPE_SURFACE)
    {
      cairo_surface_t *surface;

      cairo_pattern_get_surface (pat, &surface);
      w = cairo_image_surface_get_width (surface);
      h = cairo_image_surface_get_height (surface);
    }
  else
    w = h = 1;

  cairo_matrix_init_scale (&matrix, (gdouble) w / width, (gdouble) h / height);
  cairo_pattern_set_matrix (pat, &matrix);
}
