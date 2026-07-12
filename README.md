# Qt6 Calculator

A scientific calculator built with Qt6 and C++17.

## Features

- Basic arithmetic: +, −, ×, ÷, power (^), modulo (%)
- Scientific functions: sin, cos, tan, ln, log, sqrt
- Constants: π, e
- Parenthesized expressions with operator precedence
- Degree/radian toggle for trigonometric functions
- Memory: MC, MR, M+, M−
- Expression history (up/down arrow keys)
- Copy/paste (Ctrl+C / Ctrl+V)
- Light and dark themes

## Building

```sh
cmake -S . -B build
cmake --build build
./build/Calculator
```

### Requirements

- Qt6 (Widgets module)
- CMake ≥ 3.16
- C++17 compiler

## Running Tests

Tests are built by default:

```sh
cmake --build build
ctest --test-dir build -V
```

To disable:

```sh
cmake -S . -B build -DBUILD_TESTS=OFF
```

## Project Structure

```
├── CMakeLists.txt
├── include/
│   ├── CalculatorEngine.h     # Math operations + angle mode
│   ├── CalculatorWidget.h     # Main window
│   └── ExpressionParser.h     # Shunting-yard parser
├── src/
│   ├── main.cpp
│   ├── CalculatorEngine.cpp
│   ├── CalculatorWidget.cpp
│   └── ExpressionParser.cpp
└── tests/
    └── test_main.cpp          # Unit tests
```

## Controls

| Key | Action |
|-----|--------|
| Enter / Return | Evaluate (=) |
| Backspace | Delete last character |
| Escape | Clear (C) |
| ↑ / ↓ | Expression history |
| Ctrl+C | Copy display |
| Ctrl+V | Paste into expression |
