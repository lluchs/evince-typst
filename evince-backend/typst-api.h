// Functions exported by the rust library.

#pragma once

#include <stdlib.h>
#include <inttypes.h>

void *typst_load(char *path);
void typst_free(void *doc);
size_t typst_get_n_pages(void *doc);
void typst_get_page_size_pt(void *doc, size_t page, double *width, double *height);
void typst_get_page_size_mm(void *doc, size_t page, double *width, double *height);
void typst_free_string(char *str);
char *typst_get_title(void *doc);
void typst_render(void *doc, size_t page, size_t width, size_t height, uint8_t *surface);

