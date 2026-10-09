# MiniSQL — Compiler & Execution Engine

A lightweight, modular **SQL Compiler, Query Parser, Semantic Analyzer, and In-Memory Execution Engine** written in **C++17**.

MiniSQL parses, semantically validates, and executes core SQL statements (`CREATE TABLE`, `INSERT INTO`, and `SELECT` with complex `WHERE` logic and `ORDER BY` sorting), producing clean, aligned ASCII tabular output.

---

## 🌟 Key Features

* **Lexical Analyzer (Lexer)**: Tokenizes SQL keywords, identifiers, numbers, quoted strings, comparison operators, and skips `--` single-line comments.
* **Recursive-Descent Parser**: Builds Abstract Syntax Trees (AST) for SQL queries while maintaining operator precedence (`AND` / `OR` in `WHERE` clauses).
* **Schema Catalog & Semantic Analyzer**: Validates table and column definitions, prevents duplicate table creation, and ensures column/value count matching before execution.
* **In-Memory Storage Engine**: Manages dynamic schemas and table row collections in memory.
* **Execution Engine**: Evaluates binary logic and comparisons (`=`, `!=`, `<`, `>`, `<=`, `>=`), filters rows, sorts results by target columns, and formats output as aligned ASCII tables.
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

## 💻 Supported SQL Subset

### 1. Create Table
```sql
CREATE TABLE students (id, name, age, gpa);
```

### 2. Insert Data
```sql
INSERT INTO students VALUES (1, 'Alice', 22, 3.8);
INSERT INTO students VALUES (2, 'Bob', 19, 3.2);
```

### 3. Query & Filter Data
```sql
-- Select all columns
SELECT * FROM students;

-- Projected columns with compound WHERE condition and ORDER BY sorting
SELECT name, age, gpa FROM students WHERE age >= 20 AND gpa > 3.4 ORDER BY gpa;
SELECT name, age FROM students WHERE age < 20 OR age > 23 ORDER BY age;
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
./sqlcompiler tests/case_study.sql
```

Or run without arguments to execute built-in fallback queries:

```bash
./sqlcompiler
```

---

## 📊 Sample Execution Output

Running `./sqlcompiler tests/case_study.sql`:

```text
Table 'departments' created successfully.
Table 'courses' created successfully.
Table 'students' created successfully.
1 row inserted into 'departments'.
...
+---------+------------------------+--------+
| dept_id | dept_name              | budget |
+---------+------------------------+--------+
| 3       | Mathematics            | 300000 |
| 2       | Electrical Engineering | 450000 |
| 1       | Computer Science       | 500000 |
+---------+------------------------+--------+
(3 rows)

+-----------------+---------+------+------+
| name            | dept_id | gpa  | year |
+-----------------+---------+------+------+
| Fiona Gallagher | 2       | 3.60 | 4    |
| Diana Prince    | 3       | 3.75 | 3    |
| Alice Johnson   | 1       | 3.85 | 3    |
| Charlie Brown   | 2       | 3.92 | 4    |
+-----------------+---------+------+------+
(4 rows)
```

---

## 📁 Repository Structure

```text
minisql/
├── src/
│   ├── ast.h           # AST node definitions
│   ├── catalog.h/cpp   # Schema catalog tracking
│   ├── executor.h/cpp  # Query execution engine & ASCII renderer
│   ├── lexer.h/cpp     # Lexical analysis & tokenization
│   ├── main.cpp        # CLI entry point
│   ├── parser.h/cpp    # Recursive-descent parser
│   ├── semantic.h/cpp  # Semantic validation checks
│   ├── storage.h/cpp   # In-memory table & row storage
│   └── token.h/cpp     # Token type definitions
├── tests/
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