// semantic.h - Semantic analysis and schema validation interface.
#pragma once
#include "ast.h"
#include "catalog.h"

/**
 * @brief Validates a parsed SQL AST statement against the schema catalog.
 * 
 * Performs semantic validation checks:
 * - CREATE TABLE: Registers table schema in catalog; errors if table already exists.
 * - INSERT: Verifies table existence and matching column/value counts.
 * - SELECT: Verifies table existence, projects existing columns, validates WHERE & ORDER BY column refs.
 * 
 * @param stmt The AST statement to validate.
 * @param catalog Mutable schema catalog tracking registered tables.
 * @throws std::runtime_error on semantic error.
 */
void checkStatement(const Stmt& stmt, Catalog& catalog);
