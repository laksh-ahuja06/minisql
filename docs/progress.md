# Progress Log — Mini SQL Compiler and Execution Engine

## Completed
- Repo initialized (folders: src, tests, data, docs)
- Makefile and build.sh set up for compiling all .cpp files automatically (wildcard-based,
  no manual file list updates needed as project grows)
- token.h defined — TokenType enum and Token struct for the supported SQL subset
- Lexer implemented (lexer.h / lexer.cpp)
  - Handles keywords, identifiers, numbers, quoted strings, comparison operators, punctuation
  - Skips whitespace, newlines, and `--` single-line comments
  - Tested against sample queries — confirmed correct tokenization
- ast.h defined — node structures for CreateTableStmt, InsertStmt, SelectStmt, and Expr
  (Expr uses a tagged-union style to represent literals, column references, and binary
  operations, supporting nested WHERE conditions with AND/OR)
- Parser implemented (parser.h / parser.cpp) as a recursive-descent parser
  - Supports: CREATE TABLE, INSERT INTO, SELECT (column list or *), FROM (single table),
    WHERE (comparisons combined with AND/OR, correct precedence), ORDER BY
  - Throws clear errors with line numbers on invalid syntax instead of crashing
  - Tested against multiple queries, including compound WHERE clauses and deliberately
    malformed queries to confirm error reporting works correctly
- CLI support added to main.cpp — program now accepts a .sql file path as an argument,
  falls back to a default query when run with no arguments
- Fixed build issues (missing include causing cascading compile errors; Makefile updated
  to auto-detect all .cpp files via wildcard instead of manual file listing)
- Schema catalog implemented (catalog.h / catalog.cpp) — tracks table names and their
  column lists as statements are processed
- Semantic Analyzer implemented (semantic.h / semantic.cpp)
  - CREATE TABLE: registers schema into catalog, errors if table already exists
  - INSERT: verifies table exists, verifies value count matches column count
  - SELECT: verifies table exists, verifies selected columns exist (unless *), recursively
    validates every column reference inside WHERE, verifies ORDER BY column exists
  - Tested against valid queries and multiple deliberate error cases (unknown table,
    unknown column in SELECT/WHERE/ORDER BY, INSERT value count mismatch, duplicate
    table creation) — all caught with clear error messages
- Storage Engine implemented (storage.h / storage.cpp)
  - Manages in-memory table structures and data row collections (`Table` struct with schema and rows)
- Execution Engine implemented (executor.h / executor.cpp)
  - Executes CREATE TABLE to allocate memory tables
  - Executes INSERT INTO to store row data
  - Executes SELECT with column projection (or `*`), `WHERE` clause expression evaluation (supporting numeric and string comparison operators `=, !=, <, >, <=, >=` with `AND`/`OR` logic), and `ORDER BY` sorting
  - Formats query results into clean, aligned ASCII tables with row counts
- Full end-to-end pipeline verified: Lexer -> Parser -> Semantic Analyzer -> Storage & Execution Engine, tested via CLI against `.sql` test scripts in `tests/`

## Current status
- Lexer: complete
- Parser: complete for full Core grammar
- Semantic Analyzer: complete
- Storage Engine: complete
- Execution Engine: complete

## Next steps
- Prepare documentation and demonstration reports
- (Optional) Implement disk/file persistence in `data/` or additional SQL features (e.g. DELETE, UPDATE, joins)