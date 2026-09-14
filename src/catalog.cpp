// catalog.cpp - Implementation of the schema catalog.
#include "catalog.h"

// Registers table schema by binding a list of column names to a table name.
void Catalog::registerTable(const std::string& tableName, const std::vector<std::string>& columns) {
    tables[tableName] = columns;
}

// Returns true if the given table name exists in the catalog.
bool Catalog::tableExists(const std::string& tableName) const {
    return tables.count(tableName) > 0;
}

// Returns true if table exists and contains the specified column name.
bool Catalog::columnExists(const std::string& tableName, const std::string& columnName) const {
    if (!tableExists(tableName)) return false;
    const auto& cols = tables.at(tableName);
    for (auto& c : cols) if (c == columnName) return true;
    return false;
}

// Retrieves the column vector for a registered table.
const std::vector<std::string>& Catalog::getColumns(const std::string& tableName) const {
    return tables.at(tableName);
}
