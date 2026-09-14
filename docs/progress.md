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
- Full pipeline verified end-to-end: lexer -> parser -> semantic analyzer, run via CLI
  against real .sql test files

## Current status
- Lexer: complete
- Parser: complete for full Core grammar
- Semantic Analyzer: complete
- Execution Engine: not started

## Next steps
- Build in-memory storage (table name -> rows of data)
- Implement execution engine: CREATE TABLE / INSERT populate storage, SELECT filters
  (WHERE), projects (column list), and sorts (ORDER BY) rows, then prints results
- Expand tests/ folder with a full set of valid and error-case .sql files
- Prepare report and live demo for review