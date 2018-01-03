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

#include <Objectively/JSONSerialization.h>

#include <ObjectivelyMVC/Log.h>
#include <ObjectivelyMVC/Theme.h>
#include <ObjectivelyMVC/View.h>

#define _Class _Theme

#pragma mark - Object

/**
 * @see Object::dealloc(Object *)
 */
static void dealloc(Object *self) {

	Theme *this = (Theme *) self;

	release(this->stylesheets);

	super(Object, self, dealloc);
}

#pragma mark - Theme

/**
 * @fn void Theme::addStylesheet(Theme *self, Stylesheet *stylesheet)
 * @memberof Theme
 */
static void addStylesheet(Theme *self, Stylesheet *stylesheet) {
	$((MutableArray *) self->stylesheets, addObject, stylesheet);
}

/**
 * @brief Reducer for computeStyle.
 */
static ident computeStyle_reduce(const ident obj, ident accumulator, ident data) {

	const Stylesheet *stylesheet = obj;

	const View *view = data;

	for (size_t i = 0; i < stylesheet->selectors->count; i++) {

		Selector *selector = $(stylesheet->selectors, objectAtIndex, i);
		if ($(selector, matchesView, view)) {

			const Style *style = $(stylesheet->styles, objectForKey, selector);

			if (MVC_LogEnabled(SDL_LOG_PRIORITY_VERBOSE)) {

				String *this = $((Object *) selector, description);
				String *that = $((Object *) view, description);

				MVC_LogVerbose("%s -> %s\n", this->chars, that->chars);

				release(this);
				release(that);
			}

			if ($(style, attributeValue, "debug")) {
				SDL_TriggerBreakpoint();
			}

			$((Style *) accumulator, addSelector, selector);
			$((Style *) accumulator, addAttributes, style->attributes);
		}
	}

	return accumulator;
}

/**
 * @fn Style *Theme::computeStyle(const Theme *self, View *view)
 * @memberof Theme
 */
static Style *computeStyle(const Theme *self, const View *view) {

	assert(view);

	Style *style = $(alloc(Style), initWithAttributes, NULL);
	assert(style);

	return $(self->stylesheets, reduce, computeStyle_reduce, style, (ident) view);
}

static Theme *_defaultTheme;

/**
 * @fn Theme *Theme::defaultTheme(void)
 * @memberof Theme
 */
static Theme *defaultTheme(void) {
	static Once once;

	do_once(&once, {
		_defaultTheme = $(alloc(Theme), init);
		assert(_defaultTheme);
	});

	return _defaultTheme;
}

/**
 * @fn Theme *Theme::init(Theme *self)
 * @memberof Theme
 */
static Theme *init(Theme *self) {

	self = (Theme *) super(Object, self, init);
	if (self) {

		self->stylesheets = (Array *) $$(MutableArray, arrayWithCapacity, 8);
		assert(self->stylesheets);

		$(self, addStylesheet, $$(Stylesheet, defaultStylesheet));
	}

	return self;
}

/**
 * @fn void Theme::removeStylesheet(Theme *self, Stylesheet *stylesheet)
 * @memberof Theme
 */
static void removeStylesheet(Theme *self, Stylesheet *stylesheet) {
	$((MutableArray *) self->stylesheets, removeObject, stylesheet);
}

#pragma mark - Class lifecycle

/**
 * @see Class::initialize(Class *)
 */
static void initialize(Class *clazz) {

	((ObjectInterface *) clazz->def->interface)->dealloc = dealloc;

	((ThemeInterface *) clazz->def->interface)->addStylesheet = addStylesheet;
	((ThemeInterface *) clazz->def->interface)->computeStyle = computeStyle;
	((ThemeInterface *) clazz->def->interface)->defaultTheme = defaultTheme;
	((ThemeInterface *) clazz->def->interface)->init = init;
	((ThemeInterface *) clazz->def->interface)->removeStylesheet = removeStylesheet;
}

/**
 * @see Class::destroy(Class *)
 */
static void destroy(Class *clazz) {
	release(_defaultTheme);
}

/**
 * @fn Class *Theme::_Theme(void)
 * @memberof Theme
 */
Class *_Theme(void) {
	static Class clazz;
	static Once once;

	do_once(&once, {
		clazz.name = "Theme";
		clazz.superclass = _Object();
		clazz.instanceSize = sizeof(Theme);
		clazz.interfaceOffset = offsetof(Theme, interface);
		clazz.interfaceSize = sizeof(ThemeInterface);
		clazz.initialize = initialize;
		clazz.destroy = destroy;
	});

	return &clazz;
}

#undef _Class
