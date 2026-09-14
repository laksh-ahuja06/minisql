#pragma once
#include <string>
#include <vector>
#include <map>

class Catalog {
public:
    void registerTable(const std::string& tableName, const std::vector<std::string>& columns);
    bool tableExists(const std::string& tableName) const;
    bool columnExists(const std::string& tableName, const std::string& columnName) const;
    const std::vector<std::string>& getColumns(const std::string& tableName) const;

private:
    std::map<std::string, std::vector<std::string>> tables;
};
