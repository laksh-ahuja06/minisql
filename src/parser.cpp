#include "parser.h"
#include <stdexcept>

Parser::Parser(const std::vector<Token>& toks) : tokens(toks) {}

Token Parser::peek() { return tokens[pos]; }
Token Parser::advance() { return tokens[pos++]; }
bool Parser::isAtEnd() { return peek().type == TokenType::END_OF_FILE; }
bool Parser::check(TokenType type) { return peek().type == type; }

Token Parser::expect(TokenType type, const std::string& errMsg) {
    if (check(type)) return advance();
    throw std::runtime_error("Parse error at line " + std::to_string(peek().line) +
                              ": " + errMsg + " (got '" + peek().text + "')");
}

std::vector<Stmt> Parser::parseProgram() {
    std::vector<Stmt> stmts;
    while (!isAtEnd()) {
        stmts.push_back(parseStatement());
    }
    return stmts;
}

Stmt Parser::parseStatement() {
    if (check(TokenType::CREATE)) return parseCreateTable();
    if (check(TokenType::INSERT)) return parseInsert();
    if (check(TokenType::SELECT)) return parseSelect();
    throw std::runtime_error("Parse error at line " + std::to_string(peek().line) +
                              ": expected CREATE, INSERT, or SELECT (got '" + peek().text + "')");
}

Stmt Parser::parseCreateTable() {
    expect(TokenType::CREATE, "expected CREATE");
    expect(TokenType::TABLE, "expected TABLE");

    auto stmt = std::make_shared<CreateTableStmt>();
    stmt->tableName = expect(TokenType::IDENTIFIER, "expected table name").text;

    expect(TokenType::LPAREN, "expected '('");
    stmt->columns.push_back(expect(TokenType::IDENTIFIER, "expected column name").text);
    while (check(TokenType::COMMA)) {
        advance();
        stmt->columns.push_back(expect(TokenType::IDENTIFIER, "expected column name").text);
    }
    expect(TokenType::RPAREN, "expected ')'");
    expect(TokenType::SEMICOLON, "expected ';'");

    Stmt s;
    s.type = StmtType::CREATE_TABLE;
    s.createStmt = stmt;
    return s;
}

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

    if (check(TokenType::WHERE)) {
        advance();
        stmt->hasWhere = true;
        stmt->whereClause = parseOr();
    }

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

/* Grammar for WHERE, lowest to highest precedence:
     or_expr   -> and_expr (OR and_expr)*
     and_expr  -> comparison (AND comparison)*
     comparison-> operand (= | > | < | >= | <=) operand
     operand   -> IDENTIFIER | NUMBER | STRING
*/

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

std::shared_ptr<Expr> Parser::parseComparison() {
    auto left = parseOperand();

    std::string op;
    if (check(TokenType::EQUALS)) op = "=";
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
