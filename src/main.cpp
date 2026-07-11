#include <QApplication>
#include "CalculatorWidget.h"

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);

    CalculatorWidget w;
    w.setWindowTitle("Qt6 Calculator");
    w.resize(560, 520);
    w.show();

    return a.exec();
}
