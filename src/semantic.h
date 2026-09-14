#pragma once
#include "ast.h"
#include "catalog.h"

void checkStatement(const Stmt& stmt, Catalog& catalog);
