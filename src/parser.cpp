// parser.cpp - Implementation of recursive descent parser for MiniSQL AST construction.
#include "parser.h"
#include <stdexcept>

Parser::Parser(const std::vector<Token>& toks) : tokens(toks) {}

// Stream navigation helpers
Token Parser::peek() { return tokens[pos]; }
Token Parser::advance() { return tokens[pos++]; }
bool Parser::isAtEnd() { return peek().type == TokenType::END_OF_FILE; }
bool Parser::check(TokenType type) { return peek().type == type; }

// Asserts current token matches expected type; advances if true, throws exception if false.
Token Parser::expect(TokenType type, const std::string& errMsg) {
    if (check(type)) return advance();
    throw std::runtime_error("Parse error at line " + std::to_string(peek().line) +
                              ": " + errMsg + " (got '" + peek().text + "')");
}

// Parses all statements in the token stream until END_OF_FILE is reached.
std::vector<Stmt> Parser::parseProgram() {
    std::vector<Stmt> stmts;
    while (!isAtEnd()) {
        stmts.push_back(parseStatement());
    }
    return stmts;
}

// Routes token stream to specific statement parser based on leading keyword.
Stmt Parser::parseStatement() {
    if (check(TokenType::CREATE)) return parseCreateTable();
    if (check(TokenType::INSERT)) return parseInsert();
    if (check(TokenType::SELECT)) return parseSelect();
    if (check(TokenType::DELETE)) return parseDelete();
    if (check(TokenType::UPDATE)) return parseUpdate();
    throw std::runtime_error("Parse error at line " + std::to_string(peek().line) +
                              ": expected CREATE, INSERT, SELECT, DELETE, or UPDATE (got '" + peek().text + "')");
}

// Skips an optional column type after a column name, e.g. INT, VARCHAR, VARCHAR(20), DECIMAL(10, 2).
// Types are accepted for compatibility but not enforced: all values are stored as strings.
void Parser::skipColumnType() {
    if (!check(TokenType::IDENTIFIER)) return;
    advance();
    if (check(TokenType::LPAREN)) {
        advance();
        expect(TokenType::NUMBER, "expected type size");
        while (check(TokenType::COMMA)) {
            advance();
            expect(TokenType::NUMBER, "expected type size");
        }
        expect(TokenType::RPAREN, "expected ')' after type size");
    }
}

// Parses CREATE TABLE <name> (<col1> [type], <col2> [type], ...);
Stmt Parser::parseCreateTable() {
    expect(TokenType::CREATE, "expected CREATE");
    expect(TokenType::TABLE, "expected TABLE");

    auto stmt = std::make_shared<CreateTableStmt>();
    stmt->tableName = expect(TokenType::IDENTIFIER, "expected table name").text;

    expect(TokenType::LPAREN, "expected '('");
    stmt->columns.push_back(expect(TokenType::IDENTIFIER, "expected column name").text);
    skipColumnType();
    while (check(TokenType::COMMA)) {
        advance();
        stmt->columns.push_back(expect(TokenType::IDENTIFIER, "expected column name").text);
        skipColumnType();
    }
    expect(TokenType::RPAREN, "expected ')'");
    expect(TokenType::SEMICOLON, "expected ';'");

    Stmt s;
    s.type = StmtType::CREATE_TABLE;
    s.createStmt = stmt;
    return s;
}

// Parses INSERT INTO <table_name> VALUES (<val1>, <val2>, ...);
Stmt Parser::parseInsert() {
    expect(TokenType::INSERT, "expected INSERT");
    expect(TokenType::INTO, "expected INTO");

    auto stmt = std::make_shared<InsertStmt>();
    stmt->tableName = expect(TokenType::IDENTIFIER, "expected table name").text;

    expect(TokenType::VALUES, "expected VALUES");
    expect(TokenType::LPAREN, "expected '('");

    auto readValue = [&]() {
        if (check(TokenType::NUMBER) || check(TokenType::STRING))
            stmt->values.push_back(advance().text);
        else
            throw std::runtime_error("Parse error at line " + std::to_string(peek().line) +
                                      ": expected a number or string value");
    };
    readValue();
    while (check(TokenType::COMMA)) { advance(); readValue(); }

    expect(TokenType::RPAREN, "expected ')'");
    expect(TokenType::SEMICOLON, "expected ';'");

    Stmt s;
    s.type = StmtType::INSERT;
    s.insertStmt = stmt;
    return s;
}

// Parses SELECT (* | <cols>) FROM <table> [WHERE <expr>] [ORDER BY <col>];
Stmt Parser::parseSelect() {
    expect(TokenType::SELECT, "expected SELECT");

    auto stmt = std::make_shared<SelectStmt>();
    if (check(TokenType::STAR)) {
        advance();
        stmt->selectAll = true;
    } else {
        stmt->columns.push_back(expect(TokenType::IDENTIFIER, "expected column name").text);
        while (check(TokenType::COMMA)) {
            advance();
            stmt->columns.push_back(expect(TokenType::IDENTIFIER, "expected column name").text);
        }
    }

    expect(TokenType::FROM, "expected FROM");
    stmt->tableName = expect(TokenType::IDENTIFIER, "expected table name").text;

    // Optional WHERE clause
    if (check(TokenType::WHERE)) {
        advance();
        stmt->hasWhere = true;
        stmt->whereClause = parseOr();
    }

    // Optional ORDER BY clause
    if (check(TokenType::ORDER)) {
        advance();
        expect(TokenType::BY, "expected BY after ORDER");
        stmt->hasOrderBy = true;
        stmt->orderByColumn = expect(TokenType::IDENTIFIER, "expected column name").text;
    }

    expect(TokenType::SEMICOLON, "expected ';'");

    Stmt s;
    s.type = StmtType::SELECT;
    s.selectStmt = stmt;
    return s;
}

// Parses DELETE FROM <table_name> [WHERE <expr>];
Stmt Parser::parseDelete() {
    expect(TokenType::DELETE, "expected DELETE");
    expect(TokenType::FROM, "expected FROM");

    auto stmt = std::make_shared<DeleteStmt>();
    stmt->tableName = expect(TokenType::IDENTIFIER, "expected table name").text;

    // Optional WHERE clause
    if (check(TokenType::WHERE)) {
        advance();
        stmt->hasWhere = true;
        stmt->whereClause = parseOr();
    }

    expect(TokenType::SEMICOLON, "expected ';'");

    Stmt s;
    s.type = StmtType::DELETE;
    s.deleteStmt = stmt;
    return s;
}

// Parses UPDATE <table_name> SET <column_name> = <val> [WHERE <expr>];
Stmt Parser::parseUpdate() {
    expect(TokenType::UPDATE, "expected UPDATE");

    auto stmt = std::make_shared<UpdateStmt>();
    stmt->tableName = expect(TokenType::IDENTIFIER, "expected table name").text;

    expect(TokenType::SET, "expected SET");
    stmt->columnName = expect(TokenType::IDENTIFIER, "expected column name").text;
    expect(TokenType::EQUALS, "expected '='");

    if (check(TokenType::NUMBER) || check(TokenType::STRING)) {
        stmt->newValue = advance().text;
    } else {
        throw std::runtime_error("Parse error at line " + std::to_string(peek().line) +
                                  ": expected a number or string value for UPDATE");
    }

    // Optional WHERE clause
    if (check(TokenType::WHERE)) {
        advance();
        stmt->hasWhere = true;
        stmt->whereClause = parseOr();
    }

    expect(TokenType::SEMICOLON, "expected ';'");

    Stmt s;
    s.type = StmtType::UPDATE;
    s.updateStmt = stmt;
    return s;
}

/* Grammar for WHERE clause precedence (lowest to highest):
     or_expr   -> and_expr (OR and_expr)*
     and_expr  -> comparison (AND comparison)*
     comparison-> operand (= | != | > | < | >= | <=) operand
     operand   -> IDENTIFIER | NUMBER | STRING
*/

// Parses logical OR operations (lowest precedence in WHERE expression grammar).
std::shared_ptr<Expr> Parser::parseOr() {
    auto left = parseAnd();
    while (check(TokenType::OR)) {
        advance();
        auto right = parseAnd();
        auto node = std::make_shared<Expr>();
        node->type = ExprType::BINARY;
        node->op = "OR";
        node->left = left;
        node->right = right;
        left = node;
    }
    return left;
}

// Parses logical AND operations.
std::shared_ptr<Expr> Parser::parseAnd() {
    auto left = parseComparison();
    while (check(TokenType::AND)) {
        advance();
        auto right = parseComparison();
        auto node = std::make_shared<Expr>();
        node->type = ExprType::BINARY;
        node->op = "AND";
        node->left = left;
        node->right = right;
        left = node;
    }
    return left;
}

// Parses binary comparison operations (=, !=, >, <, >=, <=).
std::shared_ptr<Expr> Parser::parseComparison() {
    auto left = parseOperand();

    std::string op;
    if (check(TokenType::EQUALS)) op = "=";
    else if (check(TokenType::NOT_EQUALS)) op = "!=";
    else if (check(TokenType::GREATER)) op = ">";
    else if (check(TokenType::LESS)) op = "<";
    else if (check(TokenType::GREATER_EQ)) op = ">=";
    else if (check(TokenType::LESS_EQ)) op = "<=";
    else throw std::runtime_error("Parse error at line " + std::to_string(peek().line) +
                                   ": expected comparison operator (got '" + peek().text + "')");
    advance();

    auto right = parseOperand();

    auto node = std::make_shared<Expr>();
    node->type = ExprType::BINARY;
    node->op = op;
    node->left = left;
    node->right = right;
    return node;
}

// Parses leaf operand expression nodes (column references, numbers, strings).
std::shared_ptr<Expr> Parser::parseOperand() {
    auto node = std::make_shared<Expr>();
    if (check(TokenType::IDENTIFIER)) {
        node->type = ExprType::COLUMN_REF;
        node->columnName = advance().text;
    } else if (check(TokenType::NUMBER)) {
        node->type = ExprType::LITERAL_NUM;
        node->literalValue = advance().text;
    } else if (check(TokenType::STRING)) {
        node->type = ExprType::LITERAL_STR;
        node->literalValue = advance().text;
    } else {
        throw std::runtime_error("Parse error at line " + std::to_string(peek().line) +
                                  ": expected column name, number, or string (got '" + peek().text + "')");
    }
    return node;
}
