/* The Unico Theming Engine for Gtk+.
 * Copyright (C) 2009 Canonical Ltd
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

#ifndef _RAICO_BLUR_H
#define _RAICO_BLUR_H

#include <glib.h>
#include <cairo.h>

typedef enum _raico_blur_quality_t
{
  RAICO_BLUR_QUALITY_LOW = 0, /* low quality, but fast, maybe interactive */
  RAICO_BLUR_QUALITY_MEDIUM,  /* compromise between speed and quality */
  RAICO_BLUR_QUALITY_HIGH     /* quality before speed */
} raico_blur_quality_t;

typedef struct _raico_blur_private_t raico_blur_private_t;

typedef struct _raico_blur_t
{
  raico_blur_private_t* priv;
} raico_blur_t;

raico_blur_t*
raico_blur_create (void);

raico_blur_quality_t
raico_blur_get_quality (raico_blur_t* blur);

void
raico_blur_set_quality (raico_blur_t*        blur,
                        raico_blur_quality_t quality);

guint
raico_blur_get_radius (raico_blur_t* blur);

void
raico_blur_set_radius (raico_blur_t* blur,
                       guint         radius);

void
raico_blur_apply (raico_blur_t*    blur,
                  cairo_surface_t* surface);

void
raico_blur_destroy (raico_blur_t* blur);

#endif /* _RAICO_BLUR_H */
