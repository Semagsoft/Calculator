#include "CalculatorEngine.h"
#include "ExpressionParser.h"
#include <cassert>
#include <cmath>
#include <iostream>
#include <limits>

#define ASSERT_NEAR(a, b, eps) do { \
    if (std::abs((a) - (b)) > (eps)) { \
        std::cerr << "FAIL at " << __LINE__ << ": " << (a) << " != " << (b) << " (eps=" << (eps) << ")\n"; \
        return 1; \
    } \
} while(0)

int testEngine() {
    CalculatorEngine eng;

    // Basic operations
    assert(eng.calculate(2, 3, Operation::ADD) == 5.0);
    assert(eng.calculate(5, 3, Operation::SUBTRACT) == 2.0);
    assert(eng.calculate(4, 3, Operation::MULTIPLY) == 12.0);
    assert(eng.calculate(10, 3, Operation::DIVIDE) == 10.0 / 3.0);
    assert(eng.calculate(2, 3, Operation::POWER) == 8.0);
    assert(eng.calculate(10, 3, Operation::MODULO) == 1.0);

    // Division by zero
    assert(std::isnan(eng.calculate(5, 0, Operation::DIVIDE)));
    assert(std::isnan(eng.calculate(5, 0, Operation::MODULO)));

    // NONE (default)
    assert(std::isnan(eng.calculate(1, 1, Operation::NONE)));

    // sqrt
    assert(eng.sqrt(9) == 3.0);
    assert(eng.sqrt(0) == 0.0);
    assert(std::isnan(eng.sqrt(-1)));

    // Percentage
    assert(eng.percentage(50) == 0.5);
    assert(eng.percentage(0) == 0.0);
    assert(eng.percentage(100) == 1.0);

    // Trig (radians — default)
    ASSERT_NEAR(eng.sin(0), 0.0, 1e-15);
    ASSERT_NEAR(eng.sin(M_PI / 2), 1.0, 1e-15);
    ASSERT_NEAR(eng.cos(0), 1.0, 1e-15);
    ASSERT_NEAR(eng.cos(M_PI), -1.0, 1e-15);
    ASSERT_NEAR(eng.tan(0), 0.0, 1e-15);
    ASSERT_NEAR(eng.tan(M_PI / 4), 1.0, 1e-15);

    // Trig (degrees)
    eng.setAngleMode(AngleMode::DEGREES);
    ASSERT_NEAR(eng.sin(0), 0.0, 1e-15);
    ASSERT_NEAR(eng.sin(90), 1.0, 1e-15);
    ASSERT_NEAR(eng.cos(0), 1.0, 1e-15);
    ASSERT_NEAR(eng.cos(180), -1.0, 1e-15);
    ASSERT_NEAR(eng.tan(45), 1.0, 1e-15);
    ASSERT_NEAR(eng.tan(135), -1.0, 1e-10);
    eng.setAngleMode(AngleMode::RADIANS);

    // Log
    assert(eng.log10(100) == 2.0);
    assert(eng.log10(1) == 0.0);
    assert(std::isnan(eng.log10(0)));
    assert(std::isnan(eng.log10(-1)));

    assert(eng.ln(1) == 0.0);
    ASSERT_NEAR(eng.ln(M_E), 1.0, 1e-15);
    assert(std::isnan(eng.ln(0)));
    assert(std::isnan(eng.ln(-1)));

    // Constants
    ASSERT_NEAR(eng.pi(), M_PI, 1e-15);
    ASSERT_NEAR(eng.e(), M_E, 1e-15);

    // Angle mode accessors
    eng.setAngleMode(AngleMode::DEGREES);
    assert(eng.angleMode() == AngleMode::DEGREES);
    eng.setAngleMode(AngleMode::RADIANS);
    assert(eng.angleMode() == AngleMode::RADIANS);

    // New functions
    // Reciprocal
    assert(eng.reciprocal(2) == 0.5);
    assert(eng.reciprocal(4) == 0.25);
    assert(std::isnan(eng.reciprocal(0)));

    // Square
    assert(eng.square(5) == 25.0);
    assert(eng.square(0) == 0.0);
    assert(eng.square(-3) == 9.0);

    // Cube
    assert(eng.cube(3) == 27.0);
    assert(eng.cube(-2) == -8.0);
    assert(eng.cube(0) == 0.0);

    // Factorial
    assert(eng.factorial(0) == 1.0);
    assert(eng.factorial(1) == 1.0);
    assert(eng.factorial(5) == 120.0);
    assert(eng.factorial(10) == 3628800.0);
    assert(std::isnan(eng.factorial(-1)));
    assert(std::isnan(eng.factorial(1.5)));

    // Ten power
    ASSERT_NEAR(eng.tenPower(0), 1.0, 1e-15);
    ASSERT_NEAR(eng.tenPower(1), 10.0, 1e-15);
    ASSERT_NEAR(eng.tenPower(2), 100.0, 1e-15);
    ASSERT_NEAR(eng.tenPower(-1), 0.1, 1e-15);

    // Absolute value
    assert(eng.abs(5) == 5.0);
    assert(eng.abs(-5) == 5.0);
    assert(eng.abs(0) == 0.0);

    std::cout << "  CalculatorEngine: PASS\n";
    return 0;
}

int testParser() {
    CalculatorEngine eng;

    // Simple arithmetic
    assert(ExpressionParser::evaluate("2+3", eng) == 5.0);
    assert(ExpressionParser::evaluate("10-3", eng) == 7.0);
    assert(ExpressionParser::evaluate("4*5", eng) == 20.0);
    assert(ExpressionParser::evaluate("10/4", eng) == 2.5);

    // Operator precedence
    assert(ExpressionParser::evaluate("2+3*4", eng) == 14.0);
    assert(ExpressionParser::evaluate("2*3+4", eng) == 10.0);
    assert(ExpressionParser::evaluate("10-2*3", eng) == 4.0);
    assert(ExpressionParser::evaluate("10/2+3", eng) == 8.0);

    // Parentheses
    assert(ExpressionParser::evaluate("(2+3)*4", eng) == 20.0);
    assert(ExpressionParser::evaluate("2*(3+4)", eng) == 14.0);
    assert(ExpressionParser::evaluate("((2+3)*4)", eng) == 20.0);

    // Power
    assert(ExpressionParser::evaluate("2^3", eng) == 8.0);
    assert(ExpressionParser::evaluate("2^3+1", eng) == 9.0);
    assert(ExpressionParser::evaluate("2^3^2", eng) == 512.0); // right-associative: 2^(3^2) = 2^9 = 512
    assert(ExpressionParser::evaluate("(2^3)^2", eng) == 64.0);

    // Functions
    assert(ExpressionParser::evaluate("sqrt(9)", eng) == 3.0);
    assert(ExpressionParser::evaluate("sin(0)", eng) == 0.0);
    ASSERT_NEAR(ExpressionParser::evaluate("cos(0)", eng), 1.0, 1e-15);
    ASSERT_NEAR(ExpressionParser::evaluate("ln(1)", eng), 0.0, 1e-15);
    assert(ExpressionParser::evaluate("log(100)", eng) == 2.0);

    // Constants
    ASSERT_NEAR(ExpressionParser::evaluate("π", eng), M_PI, 1e-15);
    ASSERT_NEAR(ExpressionParser::evaluate("e", eng), M_E, 1e-15);

    // Mixed
    ASSERT_NEAR(ExpressionParser::evaluate("sin(π/2)", eng), 1.0, 1e-15);
    ASSERT_NEAR(ExpressionParser::evaluate("ln(e)", eng), 1.0, 1e-15);
    assert(ExpressionParser::evaluate("sqrt(25)+3", eng) == 8.0);

    // Decimal numbers
    assert(ExpressionParser::evaluate("0.5+0.5", eng) == 1.0);
    assert(ExpressionParser::evaluate("3.14*2", eng) == 6.28);

    // Division by zero
    assert(std::isnan(ExpressionParser::evaluate("1/0", eng)));

    // Unary minus
    assert(ExpressionParser::evaluate("-5+3", eng) == -2.0);
    assert(ExpressionParser::evaluate("-(5+3)", eng) == -8.0);
    assert(ExpressionParser::evaluate("2*-3", eng) == -6.0);
    assert(ExpressionParser::evaluate("(-5)^2", eng) == 25.0);
    assert(ExpressionParser::evaluate("--5", eng) == 5.0);
    assert(ExpressionParser::evaluate("3-5", eng) == -2.0);
    assert(ExpressionParser::evaluate("0-3", eng) == -3.0);

    // Modulo
    assert(ExpressionParser::evaluate("10%3", eng) == 1.0);
    assert(ExpressionParser::evaluate("10%3%2", eng) == 1.0); // left-assoc: (10%3)%2 = 1%2 = 1
    assert(ExpressionParser::evaluate("10+5%3", eng) == 12.0); // precedence: 10+(5%3) = 10+2 = 12

    std::cout << "  ExpressionParser: PASS\n";
    return 0;
}

int main() {
    std::cout << "Calculator Tests\n";
    std::cout << "================\n";

    int result = 0;
    result |= testEngine();
    result |= testParser();

    if (result == 0) {
        std::cout << "\nAll tests passed!\n";
    } else {
        std::cout << "\nSome tests FAILED.\n";
    }
    return result;
}
