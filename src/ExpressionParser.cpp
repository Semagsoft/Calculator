#include "ExpressionParser.h"
#include <sstream>
#include <algorithm>
#include <stdexcept>
#include <cctype>
#include <set>

static const std::set<std::string> functions = {
    "sin", "cos", "tan", "ln", "log", "sqrt",
    "recip", "square", "cube", "fact", "tenpow", "abs"
};

std::vector<std::string> ExpressionParser::tokenize(const std::string& expression) {
    std::vector<std::string> tokens;
    std::string current;
    for (size_t i = 0; i < expression.length(); ++i) {
        unsigned char c = static_cast<unsigned char>(expression[i]);
        if (isspace(c)) continue;

        if (isdigit(c) || c == '.') {
            if (!current.empty() && !isdigit(static_cast<unsigned char>(current.back())) && current.back() != '.') {
                tokens.push_back(current);
                current.clear();
            }
            current += static_cast<char>(c);
        } else if (isalpha(c)) {
            if (!current.empty() && (isdigit(static_cast<unsigned char>(current.back())) || current.back() == '.')) {
                tokens.push_back(current);
                current.clear();
            }
            current += static_cast<char>(c);
        } else {
            if (!current.empty()) {
                tokens.push_back(current);
                current.clear();
            }
            // Handle multi-byte UTF-8 characters as single tokens
            if ((c & 0x80) == 0) {
                char cc = static_cast<char>(c);
                if (cc == '-' && (tokens.empty() || tokens.back() == "(" || isOperator(tokens.back()))) {
                    tokens.push_back("u-");
                } else {
                    tokens.push_back(std::string(1, cc));
                }
            } else if ((c & 0xE0) == 0xC0 && i + 1 < expression.length()) {
                std::string utf8(1, c);
                utf8 += expression[++i];
                tokens.push_back(utf8);
            } else if ((c & 0xF0) == 0xE0 && i + 2 < expression.length()) {
                std::string utf8(1, c);
                utf8 += expression[++i];
                utf8 += expression[++i];
                tokens.push_back(utf8);
            } else if ((c & 0xF8) == 0xF0 && i + 3 < expression.length()) {
                std::string utf8(1, c);
                utf8 += expression[++i];
                utf8 += expression[++i];
                utf8 += expression[++i];
                tokens.push_back(utf8);
            }
        }
    }
    if (!current.empty()) {
        tokens.push_back(current);
    }
    return tokens;
}

bool ExpressionParser::isOperator(const std::string& token) {
    return token == "+" || token == "-" || token == "*" || token == "/"
        || token == "^" || token == "%" || token == "u-";
}

int ExpressionParser::getPrecedence(const std::string& token) {
    if (token == "u-") return 4;
    if (token == "^") return 3;
    if (token == "*" || token == "/" || token == "%") return 2;
    if (token == "+" || token == "-") return 1;
    return 0;
}

bool ExpressionParser::isRightAssociative(const std::string& token) {
    return token == "^" || token == "u-";
}

std::vector<std::string> ExpressionParser::shuntingYard(const std::vector<std::string>& tokens) {
    std::vector<std::string> output;
    std::stack<std::string> ops;

    for (const std::string& token : tokens) {
        if (isdigit(token[0]) || (token.length() > 1 && (isdigit(token[1]) || token[1] == '.'))) {
            output.push_back(token);
        } else if (token == "(") {
            ops.push(token);
        } else if (token == ")") {
            bool foundParen = false;
            while (!ops.empty() && ops.top() != "(") {
                output.push_back(ops.top());
                ops.pop();
            }
            if (!ops.empty()) { ops.pop(); foundParen = true; }
            if (!foundParen) throw std::invalid_argument("Mismatched parentheses");
            if (!ops.empty() && functions.count(ops.top())) {
                output.push_back(ops.top());
                ops.pop();
            }
        } else if (isOperator(token)) {
            while (!ops.empty() && ops.top() != "(" && 
                   (getPrecedence(ops.top()) > getPrecedence(token) || 
                   (getPrecedence(ops.top()) == getPrecedence(token) && !isRightAssociative(token)))) {
                output.push_back(ops.top());
                ops.pop();
            }
            ops.push(token);
        } else if (token == "π" || token == "e") {
            output.push_back(token);
        } else {
            ops.push(token);
        }
    }

    while (!ops.empty()) {
        if (ops.top() == "(") throw std::invalid_argument("Mismatched parentheses");
        output.push_back(ops.top());
        ops.pop();
    }
    return output;
}

double ExpressionParser::evaluateRPN(const std::vector<std::string>& rpn, CalculatorEngine& engine) {
    std::stack<double> values;
    for (const std::string& token : rpn) {
        if (isdigit(token[0]) || (token.length() > 1 && (isdigit(token[1]) || token[1] == '.'))) {
            values.push(std::stod(token));
        } else if (token == "(" || token == ")") {
            continue;
        } else if (token == "C") {
            values = std::stack<double>();
        } else if (token == "√" || token == "sqrt" || functions.count(token)) {
            if (values.empty()) return 0;
            double val = values.top();
            values.pop();
            if (token == "√" || token == "sqrt") values.push(engine.sqrt(val));
            else if (token == "sin") values.push(engine.sin(val));
            else if (token == "cos") values.push(engine.cos(val));
            else if (token == "tan") values.push(engine.tan(val));
            else if (token == "ln") values.push(engine.ln(val));
            else if (token == "log") values.push(engine.log10(val));
            else if (token == "recip") values.push(engine.reciprocal(val));
            else if (token == "square") values.push(engine.square(val));
            else if (token == "cube") values.push(engine.cube(val));
            else if (token == "fact") values.push(engine.factorial(val));
            else if (token == "tenpow") values.push(engine.tenPower(val));
            else if (token == "abs") values.push(engine.abs(val));
        } else if (token == "u-") {
            if (values.empty()) throw std::invalid_argument("Insufficient operands");
            double val = values.top(); values.pop();
            values.push(-val);
        } else if (isOperator(token)) {
            if (values.size() < 2) throw std::invalid_argument("Insufficient operands");
            double v2 = values.top(); values.pop();
            double v1 = values.top(); values.pop();
            if (token == "+") values.push(v1 + v2);
            else if (token == "-") values.push(v1 - v2);
            else if (token == "*") values.push(v1 * v2);
            else if (token == "/") values.push(engine.calculate(v1, v2, Operation::DIVIDE));
            else if (token == "^") values.push(engine.calculate(v1, v2, Operation::POWER));
            else if (token == "%") values.push(engine.calculate(v1, v2, Operation::MODULO));
        } else if (token == "π") {
            values.push(engine.pi());
        } else if (token == "e") {
            values.push(engine.e());
        } else {
            throw std::invalid_argument("Unknown token: " + token);
        }
    }
    return values.empty() ? 0 : values.top();
}

double ExpressionParser::evaluate(const std::string& expression, CalculatorEngine& engine) {
    if (expression.empty()) {
        throw std::invalid_argument("Empty expression");
    }
    auto tokens = tokenize(expression);
    if (tokens.empty()) {
        throw std::invalid_argument("Empty expression");
    }
    auto rpn = shuntingYard(tokens);
    if (rpn.empty()) {
        throw std::invalid_argument("Empty expression");
    }
    return evaluateRPN(rpn, engine);
}
