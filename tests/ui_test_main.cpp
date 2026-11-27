#include "CalculatorWidget.h"
#include <QApplication>
#include <QLineEdit>
#include <QPushButton>
#include <QTest>
#include <iostream>
namespace {
// Find a push button by its displayed text.
QPushButton*
findButton(QWidget* w, const QString& text) {
    for (QPushButton* btn : w->findChildren<QPushButton*>()) {
        if (btn->text() == text)
            return btn;
    }
    return nullptr;
}
// Click a push button by its displayed text.
void
click(QWidget* w, const QString& text) {
    if (QPushButton* btn = findButton(w, text))
        QTest::mouseClick(btn, Qt::LeftButton);
    else
        std::cerr << "FAIL: button \"" << text.toStdString() << "\" not found\n";
}
// Check the display text, reporting failure on mismatch.
bool
checkDisplay(QWidget* w, const QString& expected, int line) {
    QLineEdit* display = w->findChild<QLineEdit*>();
    if (!display || display->text() != expected) {
        std::cerr << "FAIL at line " << line << ": expected display \"" << expected.toStdString() << "\", got \""
                  << (display ? display->text() : QString("<null>")).toStdString() << "\"\n";
        return false;
    }
    return true;
}
} // namespace
#define CHECK_DISPLAY(w, expected) checkDisplay(w, expected, __LINE__)
int
main(int argc, char* argv[]) {
    QApplication app(argc, argv);
    CalculatorWidget w;
    w.show();
    w.setFocus();
    QTest::qWaitForWindowExposed(&w);
    QTest::qWait(100);

    QLineEdit* display = w.findChild<QLineEdit*>();
    if (!display)
        return 1;

    // Initial state
    if (!CHECK_DISPLAY(&w, "0"))
        return 1;

    // Test digit input
    QTest::keyClick(&w, '2');
    QTest::keyClick(&w, '3');
    if (!CHECK_DISPLAY(&w, "23"))
        return 1;

    // Test clear
    QTest::keyClick(&w, Qt::Key_Escape);
    if (!CHECK_DISPLAY(&w, "0"))
        return 1;

    // Test basic calculation: 2 + 3 =
    QTest::keyClick(&w, '2');
    QTest::keyClick(&w, '+');
    QTest::keyClick(&w, '3');
    QTest::keyClick(&w, Qt::Key_Return);
    if (!CHECK_DISPLAY(&w, "5"))
        return 1;

    // --- Scientific functions ---

    // sqrt(9) == 3
    QTest::keyClick(&w, '9');
    click(&w, "√");
    if (!CHECK_DISPLAY(&w, "3"))
        return 1;

    // cos(0) == 1
    QTest::keyClick(&w, Qt::Key_Escape);
    click(&w, "cos");
    if (!CHECK_DISPLAY(&w, "1"))
        return 1;

    // --- Memory ---

    // MR on an empty ("0") display recalls memory without producing "05"
    QTest::keyClick(&w, Qt::Key_Escape); // display "0", memory cleared
    QTest::keyClick(&w, '5');
    click(&w, "M+");                     // memory = 5
    QTest::keyClick(&w, Qt::Key_Escape); // display "0"
    click(&w, "MR");                     // should replace "0", not append -> "5"
    if (!CHECK_DISPLAY(&w, "5"))
        return 1;

    // Clear memory; MR should now show "0"
    click(&w, "MC");
    QTest::keyClick(&w, Qt::Key_Escape); // display "0", memory is 0
    click(&w, "MR");
    if (!CHECK_DISPLAY(&w, "0"))
        return 1;

    // --- Themes ---

    // Cycle theme: click the current theme button; its label must change
    QPushButton* themeBtn = nullptr;
    for (QPushButton* b : w.findChildren<QPushButton*>()) {
        if (b->text().startsWith("Theme:")) {
            themeBtn = b;
            break;
        }
    }
    if (!themeBtn) {
        std::cerr << "FAIL: theme button not found\n";
        return 1;
    }
    QString themeBefore = themeBtn->text();
    QTest::mouseClick(themeBtn, Qt::LeftButton);
    QTest::qWait(50);
    if (themeBtn->text() == themeBefore) {
        std::cerr << "FAIL: theme did not change from \"" << themeBefore.toStdString() << "\"\n";
        return 1;
    }

    // --- Expression history navigation ---

    // Evaluate 2 + 3 = so it lands in history
    QTest::keyClick(&w, Qt::Key_Escape);
    QTest::keyClick(&w, '2');
    QTest::keyClick(&w, '+');
    QTest::keyClick(&w, '3');
    QTest::keyClick(&w, Qt::Key_Return);
    if (!CHECK_DISPLAY(&w, "5"))
        return 1;

    // Up arrow recalls the newest history entry's expression
    QTest::keyClick(&w, Qt::Key_Up);
    QTest::qWait(50);
    if (!CHECK_DISPLAY(&w, "2+3"))
        return 1;

    // Down arrow returns to the pending expression
    QTest::keyClick(&w, Qt::Key_Down);
    QTest::qWait(50);
    if (!CHECK_DISPLAY(&w, "5"))
        return 1;

    return 0;
}
