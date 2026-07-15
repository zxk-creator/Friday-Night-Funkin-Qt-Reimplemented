#pragma once
#include <QString>

enum class ETokenType {
    // 单字符：(，)，{，},，.，-，+，;，/，*，:，@
    LEFT_PAREN, RIGHT_PAREN, LEFT_BRACE, RIGHT_BRACE,
    COMMA, DOT, MINUS, PLUS, SEMICOLON, SLASH, STAR, COLON, AT,

    // 双字符：!，!=，=，==，>，>=，<,<=
    BANG, BANG_EQUAL,
    EQUAL, EQUAL_EQUAL,
    GREATER, GREATER_EQUAL,
    LESS, LESS_EQUAL,

    // 字面量：标识符（变量名，函数名），字符串字面量，数字字面量
    IDENTIFIER, STRING, NUMBER,

    // 关键字
    AND, CLASS, ELSE, FALSE, FUNCTION, FOR, IF, NIL, OR,
    RETURN, SUPER, THIS, TRUE, VAR, WHILE, EXTENDS,
    INTERFACE, TRACE,
    NEW, PUBLIC, PRIVATE, PROTECTED, OVERRIDE, FINAL,
    INLINE, MACRO, STATIC,

    // 文件结束EOF，多个F是为了避免与关键字冲突
    EOFF
};

// AST中每个节点必定包含的东西
class Token {
public:
    ETokenType type;
    // 源码中原始字符串
    QString lexeme;
    // 解析后实际的值
    QString literal;
    // 行号
    int line;

    Token(ETokenType type, QString lexeme, QString literal, int line)
        : type(type), lexeme(std::move(lexeme)), literal(std::move(literal)), line(line) {}

    QString toString() const {
        return QString::number((int)type) + " " + lexeme + " " + literal;
    }
};