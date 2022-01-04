#ifndef SLIDING_WINDOW_H
#define SLIDING_WINDOW_H

#include "strbuf.h"

struct sliding_view {
	struct line_buffer *file;
	off_t off;
	size_t width;
	off_t max_off;	/* -1 means unlimited */
	struct strbuf buf;
};

#define SLIDING_VIEW_INIT(input, len)	{ (input), 0, 0, (len), STRBUF_INIT }

int move_window(struct sliding_view *view, off_t off, size_t width);

#endif
