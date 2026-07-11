#ifndef CALCULATOR_ENGINE_H
#define CALCULATOR_ENGINE_H

#include <string>
#include <cmath>
#include <limits>

enum class Operation {
    ADD, SUBTRACT, MULTIPLY, DIVIDE, POWER, MODULO, NONE
};

enum class AngleMode {
    RADIANS, DEGREES
};

class CalculatorEngine {
public:
    CalculatorEngine();

    double calculate(double operand1, double operand2, Operation op);
    double sqrt(double operand);
    double percentage(double operand);
    double reciprocal(double operand);
    double square(double operand);
    double cube(double operand);
    double factorial(double operand);
    double tenPower(double operand);
    double abs(double operand);

    double sin(double operand);
    double cos(double operand);
    double tan(double operand);
    double asin(double operand);
    double acos(double operand);
    double atan(double operand);
    double log10(double operand);
    double ln(double operand);
    double exp(double operand);

    double pi() const;
    double e() const;

    void setAngleMode(AngleMode mode);
    AngleMode angleMode() const;

private:
    double toRadians(double degrees) const;

    AngleMode angleMode_;
};

#endif // CALCULATOR_ENGINE_H
