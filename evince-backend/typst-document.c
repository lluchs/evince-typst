/* this file is based on xps-document.c of evince, a gnome document viewer
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

#include <glib/gi18n-lib.h>
#include <evince-document.h>

#include "typst-document.h"
#include "cairo.h"
#include "typst-api.h"

struct _TypstDocument {
	EvDocument    object;

	GFile        *file;
	void         *doc;
};

struct _TypstDocumentClass {
	EvDocumentClass parent_class;
};

EV_BACKEND_REGISTER_WITH_CODE (TypstDocument, typst_document,
	       {
	       })

/* TypstDocument */
static void
typst_document_init (TypstDocument *document)
{
}

static void
typst_document_dispose (GObject *object)
{
	TypstDocument *typst = TYPST_DOCUMENT (object);

  if (typst->file) {
		g_object_unref (typst->file);
		typst->file = NULL;
	}

	if (typst->doc) {
		typst_free(typst->doc);
		typst->doc = NULL;
	}

	G_OBJECT_CLASS (typst_document_parent_class)->dispose (object);
}

/* EvDocumentIface */
static gboolean
typst_document_load (EvDocument *document,
		   const char *uri,
		   GError    **error)
{
	TypstDocument *typst = TYPST_DOCUMENT (document);

	typst->file = g_file_new_for_uri (uri);
	char *path = g_file_get_path (typst->file);
	if (!path)
		return FALSE;
	typst->doc = typst_load(path);
	g_free(path);

	if (!typst->doc)
		return FALSE;

	return TRUE;
}

static gint
typst_document_get_n_pages (EvDocument *document)
{
	TypstDocument *typst = TYPST_DOCUMENT (document);

	return typst_get_n_pages (typst->doc);
}

static EvPage *
typst_document_get_page (EvDocument *document,
		       gint        index)
{
	EvPage      *page;

	// Nothing to do - the typst document already has all pages.
	page = ev_page_new (index);

	return page;
}

static void
typst_document_get_page_size (EvDocument *document,
			    EvPage     *page,
			    double     *width,
			    double     *height)
{
	TypstDocument *typst = TYPST_DOCUMENT (document);

	typst_get_page_size_pt (typst->doc, page->index, width, height);
}

static EvDocumentInfo *
typst_document_get_info (EvDocument *document)
{
	TypstDocument *typst = TYPST_DOCUMENT (document);
	EvDocumentInfo *info;
	char *tmp;

	info = ev_document_info_new ();
	info->fields_mask |=
		EV_DOCUMENT_INFO_N_PAGES |
		EV_DOCUMENT_INFO_PAPER_SIZE;
	// TODO: title etc.

  info->n_pages = typst_get_n_pages (typst->doc);
	if (info->n_pages > 0) {
		typst_get_page_size_mm (typst->doc, 0, &(info->paper_width), &(info->paper_height));
	}

	tmp = typst_get_title(typst->doc);
	if (tmp) {
		info->title = g_strdup(tmp);
		info->fields_mask |= EV_DOCUMENT_INFO_TITLE;
		typst_free_string(tmp);
	}

	return info;
}

static gboolean
typst_document_get_backend_info (EvDocument            *document,
			       EvDocumentBackendInfo *info)
{
	info->name = "typst";
	info->version = "0.10.0";

	return TRUE;
}

static cairo_surface_t *
typst_document_render (EvDocument      *document,
		     EvRenderContext *rc)
{
	gdouble          page_width, page_height;
	gint             width, height;
	cairo_surface_t *surface;

	TypstDocument *typst = TYPST_DOCUMENT (document);
	size_t page = rc->page->index;

	typst_get_page_size_pt (typst->doc, page, &page_width, &page_height);
	ev_render_context_compute_transformed_size (rc, page_width, page_height,
                                                    &width, &height);

	surface = cairo_image_surface_create (CAIRO_FORMAT_ARGB32,
					      width, height);

	cairo_surface_flush (surface);
	typst_render (typst->doc, page, rc->rotation, width, height, cairo_image_surface_get_data (surface));
	cairo_surface_mark_dirty (surface);

	return surface;
}

static void
typst_document_class_init (TypstDocumentClass *klass)
{
	GObjectClass    *object_class = G_OBJECT_CLASS (klass);
	EvDocumentClass *ev_document_class = EV_DOCUMENT_CLASS (klass);

	object_class->dispose = typst_document_dispose;

	ev_document_class->load = typst_document_load;
	ev_document_class->get_n_pages = typst_document_get_n_pages;
	ev_document_class->get_page = typst_document_get_page;
	ev_document_class->get_page_size = typst_document_get_page_size;
	ev_document_class->get_info = typst_document_get_info;
	ev_document_class->get_backend_info = typst_document_get_backend_info;
	ev_document_class->render = typst_document_render;
}

