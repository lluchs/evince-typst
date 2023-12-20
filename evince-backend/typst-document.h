/* this file is based on xps-document.h of evince, a gnome document viewer
 *
 * Copyright (C) 2010 Carlos Garcia Campos <carlosgc@gnome.org>
 *
 * Evince is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * Evince is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */

#pragma once

#include <glib-object.h>

#include <evince-document.h>

G_BEGIN_DECLS

#define TYPST_TYPE_DOCUMENT           (typst_document_get_type())
#define TYPST_DOCUMENT(obj)           (G_TYPE_CHECK_INSTANCE_CAST ((obj), TYPST_TYPE_DOCUMENT, TypstDocument))
#define TYPST_DOCUMENT_CLASS(klass)   (G_TYPE_CHECK_CLASS_CAST ((klass), TYPST_TYPE_DOCUMENT, TYPSTDocumentClass))
#define TYPST_IS_DOCUMENT(obj)        (G_TYPE_CHECK_INSTANCE_TYPE ((obj), TYPST_TYPE_DOCUMENT))
#define TYPST_DOCUMENT_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS((obj), TYPST_TYPE_DOCUMENT, TypstDocumentClass))

typedef struct _TypstDocument      TypstDocument;
typedef struct _TypstDocumentClass TypstDocumentClass;

GType                 typst_document_get_type   (void) G_GNUC_CONST;

EV_PUBLIC
GType                 register_evince_backend (GTypeModule *module);

G_END_DECLS
