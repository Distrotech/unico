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
 * Authored by Mirco "MacSlow" Mueller <mirco.mueller@canonical.com>
 *
 */

#include "raico-blur.h"
#include "exponential-blur.h"

struct _raico_blur_private_t
{
  raico_blur_quality_t quality; /* low, medium, high */
  guint                radius;  /* blur-radius */
};

raico_blur_t*
raico_blur_create (void)
{
  raico_blur_t*         blur = NULL;
  raico_blur_private_t* priv = NULL;

  blur = g_new0 (raico_blur_t, 1);
  if (!blur)
    {
      g_debug ("raico_blur_create(): could not allocate blur struct");
      return NULL;
    }

  priv = g_new0 (raico_blur_private_t, 1);
  if (!priv)
    {
      g_debug ("raico_blur_create(): could not allocate priv struct");
      g_free ((gpointer) blur);
      return NULL;
    }

  priv->quality = RAICO_BLUR_QUALITY_LOW;

  priv->radius  = 0;

  blur->priv = priv;

  return blur;
}

raico_blur_quality_t
raico_blur_get_quality (raico_blur_t* blur)
{
  g_assert (blur != NULL);

  return blur->priv->quality;
}

void
raico_blur_set_quality (raico_blur_t*        blur,
                        raico_blur_quality_t quality)
{
  if (!blur)
    {
      g_debug ("raico_blur_set_quality(): NULL blur-pointer passed");
      return;
    }

  blur->priv->quality = quality;
}

guint
raico_blur_get_radius (raico_blur_t* blur)
{
  g_assert (blur != NULL);

  return blur->priv->radius;
}

void
raico_blur_set_radius (raico_blur_t* blur,
                       guint         radius)
{
  if (!blur)
    {
      g_debug ("raico_blur_set_radius(): NULL blur-pointer passed");
      return;
    }

  blur->priv->radius = radius;
}

void
raico_blur_apply (raico_blur_t*    blur,
                  cairo_surface_t* surface)
{
  cairo_format_t format;

  /* sanity checks */
  if (!blur)
    {
      g_debug ("raico_blur_apply(): NULL blur-pointer passed");
      return;
    }

  if (!surface)
    {
      g_debug ("raico_blur_apply(): NULL surface-pointer passed");
      return;
    }

  if (cairo_surface_status (surface) != CAIRO_STATUS_SUCCESS)
    {
      g_debug ("raico_blur_apply(): invalid surface status");
      return;
    }

  if (cairo_surface_get_type (surface) != CAIRO_SURFACE_TYPE_IMAGE)
    {
      g_debug ("raico_blur_apply(): non-image surface passed");
      return;
    }

  format = cairo_image_surface_get_format (surface);
  if (format != CAIRO_FORMAT_A8 &&
      format != CAIRO_FORMAT_RGB24 &&
      format != CAIRO_FORMAT_ARGB32)
    {
      g_debug ("raico_blur_apply(): unsupported image-format");
      return;
    }

  /* stupid, but you never know */
  if (blur->priv->radius == 0)
    return;

  /* now do the real work */
  switch (blur->priv->quality)
  {
    default:
    case RAICO_BLUR_QUALITY_HIGH:
    case RAICO_BLUR_QUALITY_MEDIUM:
      /* Not implemented yet */
      //surface_gaussian_blur (surface, blur->priv->radius);
    case RAICO_BLUR_QUALITY_LOW:
      surface_exponential_blur (surface, blur->priv->radius);
      break;
  }
}

void
raico_blur_destroy (raico_blur_t* blur)
{
  if (!blur)
    {
      g_debug ("raico_blur_destroy(): invalid blur-pointer passed");
      return;
    }

  g_free ((gpointer) blur->priv);
  g_free ((gpointer) blur);
}
