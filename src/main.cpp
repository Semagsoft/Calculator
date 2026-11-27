#include "CalculatorWidget.h"
#include <QApplication>
int
main(int argc, char* argv[]) {
    QApplication a(argc, argv);

    CalculatorWidget w;
    w.setWindowTitle("Calculator");
    w.resize(560, 520);
    w.show();

    return a.exec();
}
