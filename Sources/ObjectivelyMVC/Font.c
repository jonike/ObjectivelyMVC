/*
 * ObjectivelyMVC: MVC framework for OpenGL and SDL2 in c.
 * Copyright (C) 2014 Jay Dolan <jay@jaydolan.com>
 *
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 * claim that you wrote the original software. If you use this software
 * in a product, an acknowledgment in the product documentation would be
 * appreciated but is not required.
 *
 * 2. Altered source versions must be plainly marked as such, and must not be
 * misrepresented as being the original software.
 *
 * 3. This notice may not be removed or altered from any source distribution.
 */

#include <assert.h>
#include <string.h>

#include <Objectively/Hash.h>
#include <Objectively/MutableArray.h>
#include <Objectively/String.h>

#include <ObjectivelyMVC/Font.h>
#include <ObjectivelyMVC/Log.h>
#include <ObjectivelyMVC/View.h>
#include <ObjectivelyMVC/Window.h>

#include <coda.ttf.h>

const EnumName FontStyleNames[] = MakeEnumNames(
	MakeEnumAlias(FontStyleRegular, regular),
	MakeEnumAlias(FontStyleBold, bold),
	MakeEnumAlias(FontStyleItalic, italic),
	MakeEnumAlias(FontStyleUnderline, underline),
	MakeEnumAlias(FontStyleStrikeThrough, strikethrough)
);

#define _Class _Font

#pragma mark - Object

/**
 * @see Object::dealloc(Object *)
 */
static void dealloc(Object *self) {

	Font *this = (Font *) self;

	free(this->family);

	release(this->data);
	
	TTF_CloseFont(this->font);

	super(Object, self, dealloc);
}

/**
 * @see Object::hash(const Object *)
 */
static int hash(const Object *self) {

	Font *this = (Font *) self;

	int hash = HASH_SEED;
	hash = HashForCString(hash, this->family);
	hash = HashForInteger(hash, this->size);
	hash = HashForInteger(hash, this->style);
	hash = HashForInteger(hash, this->renderSize);

	return hash;
}

/**
 * @see Object::isEqual(const Object *, const Object *)
 */
static _Bool isEqual(const Object *self, const Object *other) {

	if (super(Object, self, isEqual, other)) {
		return true;
	}

	if (other && $(other, isKindOfClass, _Font())) {

		const Font *this = (Font *) self;
		const Font *that = (Font *) other;

		if (!strcmp(this->family, that->family) &&
				this->size == that->size &&
				this->style == that->style &&
				this->renderSize == that->renderSize) {
			return true;
		}
	}

	return false;
}

#pragma mark - Font

static MutableDictionary *_cache;
static MutableArray *_fonts;

/**
 * @fn void Font::cacheFont(Data *font, const char *family)
 * @member Font
 */
static void cacheFont(Data *data, const char *family) {
	$(_cache, setObjectForKeyPath, data, family);
}

/**
 * @fn Font *Font::cachedFont(const char *family, int size, int style)
 * @memberof Font
 */
static Font *cachedFont(const char *family, int size, int style) {

	if (family == NULL) {
		family = DEFAULT_FONT_FAMILY;
	}
	if (size < 1) {
		size = DEFAULT_FONT_SIZE;
	}
	if (style < FontStyleRegular) {
		style = DEFAULT_FONT_STYLE;
	}

	const int renderSize = size * MVC_WindowScale(NULL, NULL, NULL);

	const Array *fonts = (Array *) _fonts;
	for (size_t i = 0; i < fonts->count; i++) {

		Font *font = $(fonts, objectAtIndex, i);

		if (!strcmp(font->family, family) &&
				font->size == size &&
				font->style == style &&
				font->renderSize == renderSize) {
			return font;
		}
	}

	Data *data = $((Dictionary *) _cache, objectForKeyPath, family);
	if (data) {
		Font *font = $(alloc(Font), initWithData, data, family, size, style);
		assert(font);

		$(_fonts, addObject, font);
		release(font);

		return font;
	}

	MVC_LogWarn("%s-%d-%d not found\n", family, size, style);
	return $$(Font, defaultFont);
}

/**
 * @fn void Font::clearCache(void)
 * @memberof Font
 */
static void clearCache(void) {
	$(_cache, removeAllObjects);
}

/**
 * @fn Font *Font::defaultFont(void)
omg  * @memberof Font
 */
static Font *defaultFont(void) {
	static Once once;

	do_once(&once, {
		Data *data = $(alloc(Data), initWithConstMemory, coda_ttf, coda_ttf_len);
		assert(data);

		$$(Font, cacheFont, data, DEFAULT_FONT_FAMILY);

		release(data);
	});

	return $$(Font, cachedFont, DEFAULT_FONT_FAMILY, DEFAULT_FONT_SIZE, DEFAULT_FONT_STYLE);
}

/**
 * @fn Font *Font::initWithData(Font *self, Data *data, int size, int index)
 * @memberof Font
 */
static Font *initWithData(Font *self, Data *data, const char *family, int size, int style) {

	self = (Font *) super(Object, self, init);
	if (self) {

		self->data = retain(data);
		assert(self->data);

		self->family = strdup(family);
		assert(self->family);

		self->size = size;
		assert(self->size);

		self->style = style;

		$(self, renderDeviceDidReset);
	}

	return self;
}

/**
 * @fn void Font::renderCharacters(const Font *self, const char *chars, SDL_Color color)
 * @memberof Font
 */
static SDL_Surface *renderCharacters(const Font *self, const char *chars, SDL_Color color) {

	SDL_Surface *surface = TTF_RenderUTF8_Blended(self->font, chars, color);

	if (surface == NULL) {
		MVC_LogError("%s\n", TTF_GetError());
	}

	return surface;
}

/**
 * @fn void Font::renderDeviceDidReset(Font *self)
 * @memberof Font
 */
static void renderDeviceDidReset(Font *self) {

	const int renderSize = self->size * MVC_WindowScale(NULL, NULL, NULL);
	if (renderSize != self->renderSize) {

		self->renderSize = renderSize;

		if (self->font) {
			TTF_CloseFont(self->font);
		}

		SDL_RWops *buffer = SDL_RWFromConstMem(self->data->bytes, (int) self->data->length);
		assert(buffer);

		self->font = TTF_OpenFontRW(buffer, 1, self->renderSize);
		assert(self->font);

		TTF_SetFontStyle(self->font, self->style);
	}
}

/**
 * @fn void Font::sizeCharacters(const Font *self, const char *chars, int *w, int *h)
 * @memberof Font
 */
static void sizeCharacters(const Font *self, const char *chars, int *w, int *h) {

	TTF_SizeUTF8(self->font, chars, w, h);

	const float scale = MVC_WindowScale(NULL, NULL, NULL);
	if (w) {
		*w /= scale;
	}
	if (h) {
		*h /= scale;
	}
}

#pragma mark - Class lifecycle

/**
 * @see Class::initialize(Class *)
 */
static void initialize(Class *clazz) {

	((ObjectInterface *) clazz->def->interface)->dealloc = dealloc;
	((ObjectInterface *) clazz->def->interface)->hash = hash;
	((ObjectInterface *) clazz->def->interface)->isEqual = isEqual;

	((FontInterface *) clazz->def->interface)->cachedFont = cachedFont;
	((FontInterface *) clazz->def->interface)->cacheFont = cacheFont;
	((FontInterface *) clazz->def->interface)->clearCache = clearCache;
	((FontInterface *) clazz->def->interface)->defaultFont = defaultFont;
	((FontInterface *) clazz->def->interface)->initWithData = initWithData;
	((FontInterface *) clazz->def->interface)->renderCharacters = renderCharacters;
	((FontInterface *) clazz->def->interface)->renderDeviceDidReset = renderDeviceDidReset;
	((FontInterface *) clazz->def->interface)->sizeCharacters = sizeCharacters;

	const int err = TTF_Init();
	assert(err == 0);

	_cache = $$(MutableDictionary, dictionary);
	assert(_cache);

	_fonts = $$(MutableArray, array);
	assert(_fonts);
}

/**
 * @see Class::destroy(Class *)
 */
static void destroy(Class *clazz) {

	release(_cache);
	release(_fonts);

	TTF_Quit();
}

/**
 * @fn Class *Font::_Font(void)
 * @memberof Font
 */
Class *_Font(void) {
	static Class clazz;
	static Once once;

	do_once(&once, {
		clazz.name = "Font";
		clazz.superclass = _Object();
		clazz.instanceSize = sizeof(Font);
		clazz.interfaceOffset = offsetof(Font, interface);
		clazz.interfaceSize = sizeof(FontInterface);
		clazz.initialize = initialize;
		clazz.destroy = destroy;
	});

	return &clazz;
}

#undef _Class
