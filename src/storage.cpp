// storage.cpp - Implementation of MiniSQL in-memory storage engine.
#include "storage.h"

void Storage::createTable(const std::string& name, const std::vector<std::string>& columns) {
    if (tableExists(name)) {
        throw std::runtime_error("Storage error: table '" + name + "' already exists");
    }
    Table t;
    t.name = name;
    t.columns = columns;
    tables[name] = t;
}

void Storage::insertRow(const std::string& name, const std::vector<std::string>& values) {
    if (!tableExists(name)) {
        throw std::runtime_error("Storage error: table '" + name + "' does not exist");
    }
    Table& t = tables[name];
    if (values.size() != t.columns.size()) {
        throw std::runtime_error("Storage error: value count mismatch for table '" + name + "'");
    }
    t.rows.push_back(values);
}

const Table& Storage::getTable(const std::string& name) const {
    auto it = tables.find(name);
    if (it == tables.end()) {
        throw std::runtime_error("Storage error: table '" + name + "' not found");
    }
    return it->second;
}

Table& Storage::getTable(const std::string& name) {
    auto it = tables.find(name);
    if (it == tables.end()) {
        throw std::runtime_error("Storage error: table '" + name + "' not found");
    }
    return it->second;
}

bool Storage::tableExists(const std::string& name) const {
    return tables.find(name) != tables.end();
}
