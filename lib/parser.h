/*
здесь пишем только то, что должно быть видно извне и стараемся только
сигнатуры тут писать, пример ниже
*/
#ifndef CRYPT_PARSER_H
#define CRYPT_PARSER_H

#include <string>
#include <exception>
#include <memory>
#include "ast.h"

class AlwaysException : std::exception {
    std::string what;
public:

    AlwaysException(std::string s) {
        what = s;
    }
};
//TODO:
// - panic on statements
// - for loop support
// - class support?
// - if expression support
// - short syntax for functions
// - parse null?
// - return from function syntax
// - test parser features
namespace parser {

    typedef std::unique_ptr<ast::Node> (*PrefixRule)();

    typedef std::unique_ptr<ast::Node> (*InfixRule)(std::unique_ptr<ast::Node> lhs);

    enum Precedence {
        PREC_NONE,
        PREC_ASSIGN,
        PREC_OR,
        PREC_AND,
        PREC_EQ,
        PREC_CMP,
        PREC_ADD,
        PREC_FACTOR,
        PREC_UNARY,
        PREC_CALL,
        PREC_PRIMARY
    };
    struct RuleInfo {
        PrefixRule prefix;
        InfixRule infix;
        int precedence;
        bool left_assoc = true;
    };
    enum TokenInfo {
        TOKEN_EOF = 0,
        TOKEN_ADD,
        TOKEN_SUB,
        TOKEN_MUL,
        TOKEN_DIV,
        TOKEN_MOD,
        TOKEN_IDENTIFIER,
        TOKEN_INT_LIT,
        TOKEN_STR_LIT,
        TOKEN_LBRACKET,
        TOKEN_RBRACKET,
        TOKEN_LPAREN,
        TOKEN_RPAREN,
        TOKEN_LCURLY,
        TOKEN_RCURLY,
        TOKEN_FN,
        TOKEN_ARROW,
        TOKEN_IF,
        TOKEN_ELSE,
        TOKEN_FOR,
        TOKEN_WHILE,
        TOKEN_RETURN,
        TOKEN_ASSIGN,
        TOKEN_AND,
        TOKEN_OR,
        TOKEN_EQ,
        TOKEN_LS,
        TOKEN_LE,
        TOKEN_GR,
        TOKEN_GE,
        TOKEN_SEMICOLON,
        TOKEN_COMMA,
        TOKEN_DOT,
        TOKEN_UNKNOWN,
    };
    constexpr int OPERATOR_EXPECTED = 1;
    constexpr int VALUE_EXPECTED = 2;
    constexpr int ANY_TOKEN_EXPECTED = OPERATOR_EXPECTED | VALUE_EXPECTED;

    unique_ptr<ast::Node> parse_expression();

    unique_ptr<ast::FunctionDef> parse_function();

    unique_ptr<ast::Block> parse_block();

    unique_ptr<ast::Node> parse_statement();

    void parse_annotations();

    ast::Program parse_program();

    void init_parser(std::istream& in);
}


/* Grammar */ /*`
 let's assume LL(1) grammar for now ( TODO: proof )
 (TODO: while?, arrays, map?...)
-   Types: int, (array?), (map?)

           function call
                \/
-    Operators: f(), -(unary), /, *, +, -, ==, !=, <=, >=, <, >, &&, ||, =(assignment)
  (priority in non-increasing order)

-  Function -> fn <name>(T1 <n1>, ...) ~> { Block }
-  Block -> (Statements | Expr)*
-  Statements ->
        if (Expr) { Block } |
        if (Expr) { Block } else { Block } |
        for (Expr; Expr; Expr) { Block } |
- Expr -> Var = Expr | Term
 Term -> кобинация тех операторов, Var, Number
 Var -> [a-zA-Z_][a-zA-Z0-9_]*


`*/

#endif