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

#pragma once

#include <Objectively/Object.h>

#include <ObjectivelyMVC/TableHeaderCellView.h>

/**
 * @file
 * @brief Columns for TableViews.
 */

#define DEFAULT_TABLE_COLUMN_WIDTH 100

typedef struct TableColumn TableColumn;
typedef struct TableColumnInterface TableColumnInterface;

/**
 * @brief Columns provide alignment, spacing and sorting hints for TableView instances.
 * @extends Object
 * @ingroup Tables
 */
struct TableColumn {

	/**
	 * @brief The superclass.
	 */
	Object object;

	/**
	 * @brief The interface.
	 * @protected
	 */
	TableColumnInterface *interface;

	/**
	 * @brief The header cell.
	 */
	TableHeaderCellView *headerCell;

	/**
	 * @brief The identifier.
	 */
	char *identifier;

	/**
	 * @brief The sort order.
	 */
	Order order;
};

/**
 * @brief The TableColumn interface.
 */
struct TableColumnInterface {

	/**
	 * @brief The superclass interface.
	 */
	ObjectInterface objectInterface;

	/**
	 * @fn TableColumn *TableColumn::initWithIdentifier(TableColumn *self, const char *identifier)
	 * @brief Initializes this TableColumn with the given identifier.
	 * @param self The TableColumn.
	 * @param identifier The column identifier.
	 * @return The initialized TableColumn, or `NULL` on error.
	 * @memberof TableColumn
	 */
	TableColumn *(*initWithIdentifier)(TableColumn *self, const char *identifier);
};

/**
 * @fn Class *TableColumn::_TableColumn(void)
 * @brief The TableColumn archetype.
 * @return The TableColumn Class.
 * @memberof TableColumn
 */
OBJECTIVELYMVC_EXPORT Class *_TableColumn(void);
