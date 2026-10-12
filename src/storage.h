// storage.h - In-memory storage engine declaration for MiniSQL table and row data.
#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include <stdexcept>

/**
 * @brief Represents a table's schema and stored data rows.
 */
struct Table {
    std::string name;                          ///< Table name
    std::vector<std::string> columns;          ///< Column names in schema order
    std::vector<std::vector<std::string>> rows;///< Rows of data (each row vector matches column order)
};

/**
 * @brief In-memory storage engine managing database tables and rows.
 */
class Storage {
public:
    /**
     * @brief Creates a new table in storage with given schema columns.
     * @param name Table name.
     * @param columns Vector of column names.
     */
    void createTable(const std::string& name, const std::vector<std::string>& columns);

    /**
     * @brief Inserts a row of values into an existing table.
     * @param name Target table name.
     * @param values Vector of row value strings matching column count.
     */
    void insertRow(const std::string& name, const std::vector<std::string>& values);

    /**
     * @brief Retrieves a const reference to a stored table.
     * @param name Table name.
     * @return Const reference to Table struct.
     */
    const Table& getTable(const std::string& name) const;

    /**
     * @brief Retrieves a mutable reference to a stored table.
     * @param name Table name.
     * @return Mutable reference to Table struct.
     */
    Table& getTable(const std::string& name);

    /**
     * @brief Checks if a table exists in storage.
     * @param name Table name.
     * @return true if table exists, false otherwise.
     */
    bool tableExists(const std::string& name) const;

private:
    std::unordered_map<std::string, Table> tables; ///< Storage map: tableName -> Table
};
