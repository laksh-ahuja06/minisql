# MiniSQL — Compiler & Execution Engine

A lightweight, modular **SQL Compiler, Query Parser, Semantic Analyzer, and In-Memory Execution Engine** written in **C++17** supporting full **CRUD (Create, Read, Update, Delete)** operations.

MiniSQL parses, semantically validates, and executes core SQL statements (`CREATE TABLE`, `INSERT INTO`, `SELECT`, `UPDATE`, and `DELETE FROM` with compound `WHERE` logic and `ORDER BY` sorting), producing clean, aligned ASCII tabular output.

---

## 🌟 Key Features

* **Lexical Analyzer (Lexer)**: Tokenizes SQL keywords (`SELECT`, `FROM`, `WHERE`, `ORDER`, `BY`, `CREATE`, `TABLE`, `INSERT`, `INTO`, `VALUES`, `UPDATE`, `SET`, `DELETE`, `AND`, `OR`), identifiers, numbers, quoted strings, comparison operators, and skips `--` single-line comments.
* **Recursive-Descent Parser**: Builds Abstract Syntax Trees (AST) for SQL queries while maintaining operator precedence (`AND` / `OR` in `WHERE` clauses).
* **Schema Catalog & Semantic Analyzer**: Validates table and column definitions, prevents duplicate table creation, checks target columns in `UPDATE`, and ensures column/value count matching before execution.
* **In-Memory Storage Engine**: Manages dynamic schemas and table row collections in memory with fast record lookup, updates, and removals.
* **Execution Engine**: Evaluates binary logic and comparisons (`=`, `!=`, `<`, `>`, `<=`, `>=`), filters rows, sorts results by target columns, updates target fields, deletes matching records, and formats output as aligned ASCII tables.
* **Multi-Statement / Script Execution**: Processes full multi-query `.sql` files sequentially with persistent in-memory state across statements.

---

## 🏗️ Architecture Pipeline

```text
SQL File / Query String
         │
         ▼
 ┌───────────────┐
 │ Lexical Analyzer (lexer.cpp)
 └───────┬───────┘
         │ Tokens
         ▼
 ┌───────────────┐
 │ Parser / AST Builder (parser.cpp)
 └───────┬───────┘
         │ AST Nodes
         ▼
 ┌───────────────┐
 │ Semantic Analyzer (semantic.cpp) <──> Schema Catalog (catalog.cpp)
 └───────┬───────┘
         │ Validated AST
         ▼
 ┌───────────────┐
 │ Execution Engine (executor.cpp)  <──> Storage Engine (storage.cpp)
 └───────┬───────┘
         │ Result Evaluation
         ▼
 Formatted ASCII Table Output
```

---

## 💻 Supported SQL Subset (CRUD)

### 1. Create Table (Create)
```sql
CREATE TABLE employees (id, name, department, salary);
```

### 2. Insert Data (Create)
```sql
INSERT INTO employees VALUES (1, 'Alice', 'Engineering', 75000);
INSERT INTO employees VALUES (2, 'Bob', 'Marketing', 50000);
```

### 3. Query & Filter Data (Read)
```sql
-- Select all columns
SELECT * FROM employees;

-- Projected columns with compound WHERE condition and ORDER BY sorting
SELECT name, department, salary FROM employees WHERE salary >= 60000 AND department = 'Engineering' ORDER BY salary;
```

### 4. Update Data (Update)
```sql
-- Update with WHERE filter
UPDATE employees SET salary = 80000 WHERE name = 'Alice';

-- Bulk update
UPDATE employees SET department = 'Sales';
```

### 5. Delete Data (Delete)
```sql
-- Delete matching records
DELETE FROM employees WHERE department = 'Marketing' AND salary <= 50000;

-- Delete all records in table
DELETE FROM employees;
```

---

## 🚀 Quick Start

### Prerequisites
* A C++17 compatible compiler (e.g., `g++` 7.0 or higher)
* `make` or `bash` (optional)

### 1. Compile the Project

* **Using `g++` directly**:
  ```bash
  g++ -Wall -std=c++17 src/*.cpp -o sqlcompiler
  ```

* **Using `build.sh`**:
  ```bash
  chmod +x build.sh
  ./build.sh
  ```

* **Using `Makefile`**:
  ```bash
  make
  ```

### 2. Run SQL Scripts

Execute a multi-statement SQL script file:

```bash
./sqlcompiler tests/crud_test.sql
```

Or run the multi-table university tracking case study:

```bash
./sqlcompiler tests/case_study.sql
```

Or run without arguments to execute built-in fallback queries:

```bash
./sqlcompiler
```

---

## 📊 Sample Execution Output

Running `./sqlcompiler tests/crud_test.sql`:

```text
Table 'employees' created successfully.
1 row inserted into 'employees'.
1 row inserted into 'employees'.
1 row inserted into 'employees'.
1 row inserted into 'employees'.
1 row inserted into 'employees'.
+----+---------+-------------+--------+
| id | name    | department  | salary |
+----+---------+-------------+--------+
| 1  | Alice   | Engineering | 75000  |
| 2  | Bob     | Marketing   | 50000  |
| 3  | Charlie | Engineering | 68000  |
| 4  | David   | Sales       | 55000  |
| 5  | Eve     | Marketing   | 52000  |
+----+---------+-------------+--------+
(5 rows)

1 row updated in 'employees'.
1 row updated in 'employees'.
+----+---------+-------------+--------+
| id | name    | department  | salary |
+----+---------+-------------+--------+
| 1  | Alice   | Engineering | 75000  |
| 2  | Bob     | Sales       | 50000  |
| 3  | Charlie | Engineering | 72000  |
| 4  | David   | Sales       | 55000  |
| 5  | Eve     | Marketing   | 52000  |
+----+---------+-------------+--------+
(5 rows)

1 row deleted from 'employees'.
1 row deleted from 'employees'.
+---------+-------------+--------+
| name    | department  | salary |
+---------+-------------+--------+
| Eve     | Marketing   | 52000  |
| David   | Sales       | 55000  |
| Charlie | Engineering | 72000  |
+---------+-------------+--------+
(3 rows)
```

---

## 📁 Repository Structure

```text
minisql/
├── src/
│   ├── ast.h           # AST node definitions (Create, Insert, Select, Update, Delete)
│   ├── catalog.h/cpp   # Schema catalog tracking
│   ├── executor.h/cpp  # Query execution engine & ASCII renderer
│   ├── lexer.h/cpp     # Lexical analysis & tokenization
│   ├── main.cpp        # CLI entry point
│   ├── parser.h/cpp    # Recursive-descent parser
│   ├── semantic.h/cpp  # Semantic validation checks
│   ├── storage.h/cpp   # In-memory table & row storage
│   └── token.h/cpp     # Token type definitions
├── tests/
│   ├── crud_test.sql   # Full CRUD lifecycle test script
│   ├── case_study.sql  # Multi-table university tracking test case
│   ├── sample1.sql     # Analytical queries test script
│   ├── sample2.sql     # Schema test script
│   └── error.sql       # Error handling validation script
├── docs/
│   └── progress.md     # Task progress & milestone log
├── Makefile            # Build automation configuration
├── build.sh            # Shell compilation script
└── README.md           # Project documentation
```

---

## 📝 License

This project is open-source under the [MIT License](LICENSE).