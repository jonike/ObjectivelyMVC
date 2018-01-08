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

#include <ObjectivelyMVC/Button.h>

#define _Class _Button

#pragma mark - Object

/**
 * @see Object::dealloc(Object *)
 */
static void dealloc(Object *self) {

	Button *this = (Button *) self;

	release(this->title);

	super(Object, self, dealloc);
}

#pragma mark - View

/**
 * @see View::awakeWithDictionary(View *, const Dictionary *)
 */
static void awakeWithDictionary(View *self, const Dictionary *dictionary) {

	super(View, self, awakeWithDictionary, dictionary);

	Button *this = (Button *) self;

	const Inlet inlets[] = MakeInlets(
		MakeInlet("title", InletTypeView, &this->title, NULL)
	);

	$(self, bind, inlets, dictionary);
}

/**
 * @see View::init(View *)
 */
static View *init(View *self) {
	return (View *) $((Button *) self, initWithFrame, NULL);
}

#pragma mark - Control

/**
 * @see Control::captureEvent(Control *, const SDL_Event *)
 */
static _Bool captureEvent(Control *self, const SDL_Event *event) {

	if (event->type == SDL_MOUSEBUTTONDOWN) {
		self->state |= ControlStateHighlighted;
		return true;
	}

	if (event->type == SDL_MOUSEBUTTONUP) {
		self->state &= ~ControlStateHighlighted;
		return true;
	}

	return super(Control, self, captureEvent, event);
}

#pragma mark - Button

/**
 * @fn Button *Button::initWithFrame(Button *self, const SDL_Rect *frame)
 * @memberof Button
 */
static Button *initWithFrame(Button *self, const SDL_Rect *frame) {

	self = (Button *) super(Control, self, initWithFrame, frame);
	if (self) {

		self->title = $(alloc(Text), initWithText, NULL, NULL);
		assert(self->title);

		$((View *) self, addSubview, (View *) self->title);
	}

	return self;
}

/**
 * @fn Button *Button::initWithFrame(Button *self, const char *title)
 * @memberof Button
 */
static Button *initWithTitle(Button *self, const char *title) {

	self = $(self, initWithFrame, NULL);
	if (self) {
		$(self->title, setText, title);
	}
	return self;
}

#pragma mark - Class lifecycle

/**
 * @see Class::initialize(Class *)
 */
static void initialize(Class *clazz) {

	((ObjectInterface *) clazz->def->interface)->dealloc = dealloc;

	((ViewInterface *) clazz->def->interface)->awakeWithDictionary = awakeWithDictionary;
	((ViewInterface *) clazz->def->interface)->init = init;

	((ControlInterface *) clazz->def->interface)->captureEvent = captureEvent;

	((ButtonInterface *) clazz->def->interface)->initWithFrame = initWithFrame;
	((ButtonInterface *) clazz->def->interface)->initWithTitle = initWithTitle;
}

/**
 * @fn Class *Button::_Button(void)
 * @memberof Button
 */
Class *_Button(void) {
	static Class clazz;
	static Once once;

	do_once(&once, {
		clazz.name = "Button";
		clazz.superclass = _Control();
		clazz.instanceSize = sizeof(Button);
		clazz.interfaceOffset = offsetof(Button, interface);
		clazz.interfaceSize = sizeof(ButtonInterface);
		clazz.initialize = initialize;
	});

	return &clazz;
}

#undef _Class
