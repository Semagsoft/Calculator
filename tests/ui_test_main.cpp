#include <QApplication>
#include <QLineEdit>
#include <QTest>
#include "CalculatorWidget.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    CalculatorWidget w;
    w.show();
    QTest::qWaitForWindowExposed(&w);

    auto* display = w.findChild<QLineEdit*>();
    if (!display) return 1;

    // Initial state
    if (display->text() != "0") return 1;

    // Test digit input
    QTest::keyClick(&w, '2');
    if (display->text() != "2") return 1;

    QTest::keyClick(&w, '3');
    if (display->text() != "23") return 1;

    // Test clear
    QTest::keyClick(&w, Qt::Key_Escape);
    if (display->text() != "0") return 1;

    // Test basic calculation: 2 + 3 =
    QTest::keyClick(&w, '2');
    QTest::keyClick(&w, '+');
    QTest::keyClick(&w, '3');
    QTest::keyClick(&w, Qt::Key_Return);
    if (display->text() != "5") return 1;

    return 0;
}
