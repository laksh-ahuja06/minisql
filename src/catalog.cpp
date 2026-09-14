#include "catalog.h"

void Catalog::registerTable(const std::string& tableName, const std::vector<std::string>& columns) {
    tables[tableName] = columns;
}

bool Catalog::tableExists(const std::string& tableName) const {
    return tables.count(tableName) > 0;
}

bool Catalog::columnExists(const std::string& tableName, const std::string& columnName) const {
    if (!tableExists(tableName)) return false;
    const auto& cols = tables.at(tableName);
    for (auto& c : cols) if (c == columnName) return true;
    return false;
}

const std::vector<std::string>& Catalog::getColumns(const std::string& tableName) const {
    return tables.at(tableName);
}
