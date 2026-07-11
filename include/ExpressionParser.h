#ifndef EXPRESSION_PARSER_H
#define EXPRESSION_PARSER_H

#include <string>
#include <vector>
#include <stack>
#include <map>
#include <functional>
#include "CalculatorEngine.h"

class ExpressionParser {
public:
    static double evaluate(const std::string& expression, CalculatorEngine& engine);

private:
    static std::vector<std::string> tokenize(const std::string& expression);
    static std::vector<std::string> shuntingYard(const std::vector<std::string>& tokens);
    static double evaluateRPN(const std::vector<std::string>& rpn, CalculatorEngine& engine);

    static bool isOperator(const std::string& token);
    static int getPrecedence(const std::string& token);
    static bool isRightAssociative(const std::string& token);
};

#endif // EXPRESSION_PARSER_H
