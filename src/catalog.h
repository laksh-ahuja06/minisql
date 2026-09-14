// catalog.h - Schema catalog class declaration for tracking tables and column definitions.
#pragma once
#include <string>
#include <vector>
#include <map>

/**
 * @brief In-memory schema catalog that registers and tracks defined tables and their column lists.
 */
class Catalog {
public:
    /**
     * @brief Registers a new table schema in the catalog.
     * @param tableName Name of the table.
     * @param columns List of column names defined for the table.
     */
    void registerTable(const std::string& tableName, const std::vector<std::string>& columns);

    /**
     * @brief Checks if a table exists in the catalog.
     * @param tableName Table name to query.
     * @return true if the table is registered, false otherwise.
     */
    bool tableExists(const std::string& tableName) const;

    /**
     * @brief Checks if a specific column exists within a registered table.
     * @param tableName Name of the table.
     * @param columnName Name of the column.
     * @return true if both table and column exist, false otherwise.
     */
    bool columnExists(const std::string& tableName, const std::string& columnName) const;

    /**
     * @brief Retrieves the column names for a given table.
     * @param tableName Name of the table.
     * @return Reference to vector of column names.
     */
    const std::vector<std::string>& getColumns(const std::string& tableName) const;

private:
    std::map<std::string, std::vector<std::string>> tables; ///< Map storing tableName -> vector of column names
};
