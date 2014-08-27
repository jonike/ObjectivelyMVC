/*
 * Simple DirectMedia Layer - MVC
 * Copyright (C) 2014 Jay Dolan <jay@jaydolan.com>
 *
 * @author jdolan
 */

#ifndef _MVC_label_h
#define _MVC_label_h

#include <SDL2/SDL_ttf.h>

#include "MVC_view.h"

Interface(MVC_Label, MVC_View)

	void (*render)(MVC_Label *self);
	void (*getSize)(MVC_Label *self, int *width, int *height);

	TTF_Font *font;
	SDL_Color color;
	char *text;

	SDL_Texture *texture;

End

Constructor(MVC_Label, TTF_Font *font, SDL_Color color, const char *text);

#endif