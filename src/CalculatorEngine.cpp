#define _USE_MATH_DEFINES
#include "CalculatorEngine.h"

CalculatorEngine::CalculatorEngine()
    : angleMode_(AngleMode::RADIANS) {}

double CalculatorEngine::calculate(double operand1, double operand2, Operation op) {
    switch (op) {
        case Operation::ADD:
            return operand1 + operand2;
        case Operation::SUBTRACT:
            return operand1 - operand2;
        case Operation::MULTIPLY:
            return operand1 * operand2;
        case Operation::DIVIDE:
            if (operand2 == 0)
                return std::numeric_limits<double>::quiet_NaN();
            return operand1 / operand2;
        case Operation::MODULO:
            if (operand2 == 0)
                return std::numeric_limits<double>::quiet_NaN();
            return std::fmod(operand1, operand2);
        case Operation::POWER:
            return std::pow(operand1, operand2);
        default:
            return std::numeric_limits<double>::quiet_NaN();
    }
}

double CalculatorEngine::sqrt(double operand) {
    if (operand < 0) return std::numeric_limits<double>::quiet_NaN();
    return std::sqrt(operand);
}

double CalculatorEngine::percentage(double operand) {
    return operand / 100.0;
}

double CalculatorEngine::reciprocal(double operand) {
    if (operand == 0) return std::numeric_limits<double>::quiet_NaN();
    return 1.0 / operand;
}

double CalculatorEngine::square(double operand) {
    return operand * operand;
}

double CalculatorEngine::cube(double operand) {
    return operand * operand * operand;
}

double CalculatorEngine::factorial(double operand) {
    if (operand < 0 || operand > 170 || std::floor(operand) != operand)
        return std::numeric_limits<double>::quiet_NaN();
    double result = 1.0;
    for (int i = 2; i <= static_cast<int>(operand); ++i)
        result *= i;
    return result;
}

double CalculatorEngine::tenPower(double operand) {
    return std::pow(10.0, operand);
}

double CalculatorEngine::abs(double operand) {
    return std::abs(operand);
}

double CalculatorEngine::toRadians(double degrees) const {
    return degrees * M_PI / 180.0;
}

double CalculatorEngine::sin(double operand) {
    double rad = (angleMode_ == AngleMode::DEGREES) ? toRadians(operand) : operand;
    return std::sin(rad);
}

double CalculatorEngine::cos(double operand) {
    double rad = (angleMode_ == AngleMode::DEGREES) ? toRadians(operand) : operand;
    return std::cos(rad);
}

double CalculatorEngine::tan(double operand) {
    double rad = (angleMode_ == AngleMode::DEGREES) ? toRadians(operand) : operand;
    return std::tan(rad);
}

double CalculatorEngine::asin(double operand) {
    if (operand < -1.0 || operand > 1.0) return std::numeric_limits<double>::quiet_NaN();
    double rad = std::asin(operand);
    return (angleMode_ == AngleMode::DEGREES) ? rad * 180.0 / M_PI : rad;
}

double CalculatorEngine::acos(double operand) {
    if (operand < -1.0 || operand > 1.0) return std::numeric_limits<double>::quiet_NaN();
    double rad = std::acos(operand);
    return (angleMode_ == AngleMode::DEGREES) ? rad * 180.0 / M_PI : rad;
}

double CalculatorEngine::atan(double operand) {
    double rad = std::atan(operand);
    return (angleMode_ == AngleMode::DEGREES) ? rad * 180.0 / M_PI : rad;
}

double CalculatorEngine::exp(double operand) {
    return std::exp(operand);
}

double CalculatorEngine::log10(double operand) {
    if (operand <= 0) return std::numeric_limits<double>::quiet_NaN();
    return std::log10(operand);
}

double CalculatorEngine::ln(double operand) {
    if (operand <= 0) return std::numeric_limits<double>::quiet_NaN();
    return std::log(operand);
}

double CalculatorEngine::pi() const {
    return M_PI;
}

double CalculatorEngine::e() const {
    return M_E;
}

void CalculatorEngine::setAngleMode(AngleMode mode) {
    angleMode_ = mode;
}

AngleMode CalculatorEngine::angleMode() const {
    return angleMode_;
}
