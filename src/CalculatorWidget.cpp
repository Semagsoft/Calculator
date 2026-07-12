#include "CalculatorWidget.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QPushButton>
#include <QApplication>
#include <QClipboard>
#include <QFont>
#include <QKeyEvent>
#include <QResizeEvent>
#include <QLabel>
#include <QLocale>
#include <QMap>
#include <cmath>

CalculatorWidget::CalculatorWidget(QWidget *parent)
    : QWidget(parent), currentExpression("0")
{
    setMinimumSize(480, 440);
    setupUI();
    applyStyle(Theme::Light);
    loadHistory();
    updateStatusLabel();
}

void CalculatorWidget::setupUI() {
    QHBoxLayout* outerLayout = new QHBoxLayout(this);
    outerLayout->setSpacing(6);

    // --- History panel ---
    historyPanel = new QWidget(this);
    historyPanel->setMinimumWidth(120);
    QVBoxLayout* histLayout = new QVBoxLayout(historyPanel);
    histLayout->setContentsMargins(0, 0, 0, 0);

    historyList = new QListWidget();
    historyList->setWordWrap(true);
    historyList->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    historyList->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    QObject::connect(historyList, &QListWidget::itemClicked, [this](QListWidgetItem* item) {
        int idx = historyList->row(item);
        if (idx >= 0 && idx < history.size()) {
            currentExpression = history[idx].expression;
            display->setText(currentExpression);
            justEvaluated = false;
        }
    });

    QPushButton* clearHistBtn = new QPushButton("Clear History");
    clearHistBtn->setFont(QFont("Arial", 10));

    histLayout->addWidget(new QLabel("History"));
    histLayout->addWidget(historyList);
    histLayout->addWidget(clearHistBtn);

    QObject::connect(clearHistBtn, &QPushButton::clicked, [this]() {
        history.clear();
        historyList->clear();
        historyIndex = -1;
        saveHistory();
    });

    // --- Calculator section ---
    QVBoxLayout* calcLayout = new QVBoxLayout();
    calcLayout->setSpacing(4);

    QHBoxLayout* displayLayout = new QHBoxLayout();
    displayLayout->setSpacing(2);

    memIndicator = new QLabel();
    memIndicator->setFont(QFont("Arial", 14, QFont::Bold));
    memIndicator->setFixedWidth(24);
    memIndicator->setAlignment(Qt::AlignCenter);
    memIndicator->setText("");

    display = new QLineEdit();
    display->setReadOnly(true);
    display->setAlignment(Qt::AlignRight);
    display->setFont(QFont("Arial", 24));
    display->setText(currentExpression);
    display->setMinimumHeight(50);

    displayLayout->addWidget(memIndicator);
    displayLayout->addWidget(display, 1);
    calcLayout->addLayout(displayLayout);

    QGridLayout* grid = new QGridLayout();
    grid->setSpacing(4);

    struct ButtonDef { QString text; int row; int col; };
    QVector<ButtonDef> buttons = {
        {"MC",  0,0}, {"MR",  0,1}, {"M+",  0,2}, {"M-",  0,3}, {"C",   0,4}, {"DEL", 0,5},
        {"%",   1,0}, {"^",   1,1}, {"√",   1,2}, {"DEG", 1,3}, {"1/x", 1,4}, {"x²",  1,5},
        {"x³",  2,0}, {"n!",  2,1}, {"10ˣ", 2,2}, {"|x|", 2,3}, {"sin", 2,4}, {"cos", 2,5},
        {"tan", 3,0}, {"ln",  3,1}, {"log", 3,2}, {"7",   3,3}, {"8",   3,4}, {"9",   3,5},
        {"*",   4,0}, {"/",   4,1}, {"4",   4,2}, {"5",   4,3}, {"6",   4,4}, {"+",   4,5},
        {"-",   5,0}, {"1",   5,1}, {"2",   5,2}, {"3",   5,3}, {"π",   5,4}, {"e",   5,5},
        {"0",   6,0}, {".",   6,1}, {"=",   6,2}, {"(",   6,3}, {")",   6,4}, {"perc", 6,5},
    };

    QMap<QString, QString> tips;
    tips["MC"] = "Clear memory";
    tips["MR"] = "Recall memory";
    tips["M+"] = "Add to memory";
    tips["M-"] = "Subtract from memory";
    tips["C"] = "Clear expression";
    tips["DEL"] = "Delete last character";
    tips["%"] = "Modulo";
    tips["^"] = "Exponentiation";
    tips["√"] = "Square root";
    tips["DEG"] = "Toggle degrees/radians";
    tips["1/x"] = "Reciprocal";
    tips["x²"] = "Square";
    tips["x³"] = "Cube";
    tips["n!"] = "Factorial";
    tips["10ˣ"] = "10 to the power of x";
    tips["|x|"] = "Absolute value";
    tips["sin"] = "Sine";
    tips["cos"] = "Cosine";
    tips["tan"] = "Tangent";
    tips["ln"] = "Natural logarithm";
    tips["log"] = "Log base 10";
    tips["π"] = "Pi (3.14159...)";
    tips["e"] = "Euler's number (2.71828...)";
    tips["perc"] = "Percent (÷100)";

    for (const auto& b : buttons) {
        QPushButton* btn = new QPushButton(b.text);
        btn->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        btn->setMinimumSize(36, 28);
        btn->setFont(QFont("Arial", 12));
        grid->addWidget(btn, b.row, b.col);

        if (b.text == "MC")  mcBtn = btn;
        if (b.text == "MR")  mrBtn = btn;
        if (b.text == "DEG") angleBtn = btn;
        if (b.text == "sin") sinBtn = btn;
        if (b.text == "cos") cosBtn = btn;
        if (b.text == "tan") tanBtn = btn;
        if (b.text == "ln")  lnBtn = btn;
        if (b.text == "=")   btn->setObjectName("equalsBtn");

        auto it = tips.find(b.text);
        if (it != tips.end())
            btn->setToolTip(it.value());

        QObject::connect(btn, &QPushButton::clicked, [this, text = b.text]() {
            keyBuffer.clear();
            this->onButtonClicked(text);
        });
    }

    for (int i = 0; i < 6; ++i) grid->setColumnStretch(i, 1);
    for (int i = 0; i < 7; ++i) grid->setRowStretch(i, 1);

    calcLayout->addLayout(grid);

    QHBoxLayout* toolbar = new QHBoxLayout();
    toolbar->setSpacing(4);

    themeBtn = new QPushButton("Theme: " + themeName(currentTheme));
    themeBtn->setFont(QFont("Arial", 10));
    themeBtn->setFixedWidth(130);

    secondBtn = new QPushButton("2nd");
    secondBtn->setFont(QFont("Arial", 10));
    secondBtn->setFixedWidth(50);
    secondBtn->setCheckable(true);

    QPushButton* toggleHistBtn = new QPushButton("Hist");
    toggleHistBtn->setFont(QFont("Arial", 10));
    toggleHistBtn->setFixedWidth(60);

    sciBtn = new QPushButton(scientificNotation ? "NORM" : "SCI");
    sciBtn->setFont(QFont("Arial", 10));
    sciBtn->setFixedWidth(50);

    statusLabel = new QLabel();
    statusLabel->setFont(QFont("Arial", 10));
    statusLabel->setFixedHeight(24);

    toolbar->addWidget(themeBtn);
    toolbar->addWidget(secondBtn);
    toolbar->addWidget(toggleHistBtn);
    toolbar->addWidget(sciBtn);
    toolbar->addStretch();
    toolbar->addWidget(statusLabel);
    calcLayout->addLayout(toolbar);

    QObject::connect(themeBtn, &QPushButton::clicked, [this]() {
        cycleTheme();
    });

    QObject::connect(secondBtn, &QPushButton::clicked, [this]() {
        secondMode = secondBtn->isChecked();
        updateButtonLabels();
        updateStatusLabel();
    });

    QObject::connect(toggleHistBtn, &QPushButton::clicked, [this]() {
        historyVisible = !historyVisible;
        historyPanel->setVisible(historyVisible);
    });

    QObject::connect(sciBtn, &QPushButton::clicked, [this]() {
        onButtonClicked(scientificNotation ? "NORM" : "SCI");
    });

    outerLayout->addWidget(historyPanel, 1);
    outerLayout->addLayout(calcLayout, 3);

    updateMemoryButtons();
}

void CalculatorWidget::updateMemoryButtons() {
    if (mcBtn) mcBtn->setEnabled(memoryValue != 0);
    if (mrBtn) mrBtn->setEnabled(memoryValue != 0);
    if (memIndicator) memIndicator->setText(memoryValue != 0 ? "M" : "");
    updateStatusLabel();
}

void CalculatorWidget::updateStatusLabel() {
    QStringList parts;
    parts << (engine.angleMode() == AngleMode::DEGREES ? "DEG" : "RAD");
    parts << (scientificNotation ? "SCI" : "NORM");
    if (secondMode) parts << "2nd";
    if (memoryValue != 0) parts << "M";
    if (statusLabel) statusLabel->setText(parts.join("  "));
}

static QListWidgetItem* makeHistoryItem(QListWidget* list, const QString& expr, const QString& result) {
    QLabel* label = new QLabel(
        QString("<div style='font-size:10px; color:gray;'>%1</div>"
                "<div style='font-size:13px; font-weight:bold;'>%2</div>")
            .arg(expr.toHtmlEscaped(), result.toHtmlEscaped())
    );
    label->setWordWrap(true);
    label->setContentsMargins(4, 2, 4, 2);
    label->setTextFormat(Qt::RichText);

    int h = QFontMetrics(QFont("Arial", 10)).lineSpacing()
          + QFontMetrics(QFont("Arial", 13, QFont::Bold)).lineSpacing() + 12;
    QListWidgetItem* item = new QListWidgetItem();
    item->setSizeHint(QSize(0, h));
    list->insertItem(0, item);
    list->setItemWidget(item, label);
    return item;
}

void CalculatorWidget::addHistoryEntry(const QString& expr, const QString& result) {
    history.prepend({expr, result});
    if (history.size() > 100) history.removeLast();

    makeHistoryItem(historyList, expr, result);
    saveHistory();
}

QString CalculatorWidget::formatNumber(double result) const {
    QString s = QString::number(result, 'g', 10);
    if (scientificNotation)
        s = QString::number(result, 'E', 10);
    return s;
}

void CalculatorWidget::onButtonClicked(const QString& text) {
    // C
    if (text == "C") {
        currentExpression = "0";
        display->setText(currentExpression);
        justEvaluated = false;
        return;
    }

    // DEL
    if (text == "DEL") {
        if (currentExpression.length() <= 1) {
            currentExpression = "0";
        } else {
            currentExpression.chop(1);
        }
        display->setText(currentExpression);
        justEvaluated = false;
        return;
    }

    // = (evaluate)
    if (text == "=") {
        try {
            double result = ExpressionParser::evaluate(currentExpression.toStdString(), engine);
            if (std::isnan(result)) {
                display->setText("Math Error");
                currentExpression = "0";
                justEvaluated = false;
            } else if (std::isinf(result)) {
                display->setText("Overflow");
                currentExpression = "0";
                justEvaluated = false;
            } else {
                addHistoryEntry(currentExpression, formatNumber(result));
                historyIndex = -1;

                currentExpression = formatNumber(result);
                display->setText(currentExpression);
                justEvaluated = true;
            }
        } catch (const std::invalid_argument&) {
            display->setText("Syntax Error");
            currentExpression = "0";
            justEvaluated = false;
        } catch (const std::domain_error&) {
            display->setText("Math Error");
            currentExpression = "0";
            justEvaluated = false;
        } catch (...) {
            display->setText("Error");
            currentExpression = "0";
            justEvaluated = false;
        }
        return;
    }

    // Operators and parens
    if (text == "+" || text == "-" || text == "*" || text == "/" || text == "^"
        || text == "%" || text == "(" || text == ")") {
        justEvaluated = false;
        if (text == "-" && currentExpression == "0") {
            currentExpression = "-";
        } else {
            currentExpression += text;
        }
        display->setText(currentExpression);
        return;
    }

    // Single-operand unary functions (applied immediately)
    auto applyUnary = [&](auto func) -> bool {
        try {
            double val = ExpressionParser::evaluate(currentExpression.toStdString(), engine);
            if (std::isnan(val)) {
                display->setText("Math Error");
                currentExpression = "0";
                justEvaluated = false;
                return true;
            }
            if (std::isinf(val)) {
                display->setText("Overflow");
                currentExpression = "0";
                justEvaluated = false;
                return true;
            }
            double result = func(val);
            if (std::isnan(result)) {
                display->setText("Math Error");
                currentExpression = "0";
                justEvaluated = false;
            } else if (std::isinf(result)) {
                display->setText("Overflow");
                currentExpression = "0";
                justEvaluated = false;
            } else {
                currentExpression = formatNumber(result);
                display->setText(currentExpression);
                justEvaluated = true;
            }
        } catch (const std::invalid_argument&) {
            display->setText("Syntax Error");
            currentExpression = "0";
            justEvaluated = false;
        } catch (const std::domain_error&) {
            display->setText("Math Error");
            currentExpression = "0";
            justEvaluated = false;
        } catch (...) {
            display->setText("Error");
            currentExpression = "0";
            justEvaluated = false;
        }
        return true;
    };

    if (text == "√")       { applyUnary([&](double v){ return engine.sqrt(v); }); return; }
    if (text == "sin")     { if (secondMode) applyUnary([&](double v){ return engine.asin(v); }); else applyUnary([&](double v){ return engine.sin(v); }); return; }
    if (text == "cos")     { if (secondMode) applyUnary([&](double v){ return engine.acos(v); }); else applyUnary([&](double v){ return engine.cos(v); }); return; }
    if (text == "tan")     { if (secondMode) applyUnary([&](double v){ return engine.atan(v); }); else applyUnary([&](double v){ return engine.tan(v); }); return; }
    if (text == "ln")      { if (secondMode) applyUnary([&](double v){ return engine.exp(v); }); else applyUnary([&](double v){ return engine.ln(v); }); return; }
    if (text == "log")     { applyUnary([&](double v){ return engine.log10(v); }); return; }
    if (text == "1/x")     { applyUnary([&](double v){ return engine.reciprocal(v); }); return; }
    if (text == "x²")      { applyUnary([&](double v){ return engine.square(v); }); return; }
    if (text == "x³")      { applyUnary([&](double v){ return engine.cube(v); }); return; }
    if (text == "n!")      { applyUnary([&](double v){ return engine.factorial(v); }); return; }
    if (text == "10ˣ")     { applyUnary([&](double v){ return engine.tenPower(v); }); return; }
    if (text == "|x|")     { applyUnary([&](double v){ return engine.abs(v); }); return; }
    if (text == "perc")    { applyUnary([&](double v){ return engine.percentage(v); }); return; }

    // Constants
    if (text == "π") {
        justEvaluated = false;
        currentExpression += QString::number(engine.pi(), 'g', 10);
        display->setText(currentExpression);
        return;
    }

    if (text == "e") {
        justEvaluated = false;
        currentExpression += QString::number(engine.e(), 'g', 10);
        display->setText(currentExpression);
        return;
    }

    // Angle mode toggle
    if (text == "DEG" || text == "RAD") {
        justEvaluated = false;
        AngleMode newMode = (engine.angleMode() == AngleMode::RADIANS)
            ? AngleMode::DEGREES : AngleMode::RADIANS;
        engine.setAngleMode(newMode);
        if (angleBtn) angleBtn->setText(newMode == AngleMode::DEGREES ? "DEG" : "RAD");
        updateStatusLabel();
        return;
    }

    // SCI toggle
    if (text == "SCI" || text == "NORM") {
        justEvaluated = false;
        scientificNotation = !scientificNotation;
        if (sciBtn) sciBtn->setText(scientificNotation ? "NORM" : "SCI");
        updateStatusLabel();
        bool ok;
        double val = currentExpression.toDouble(&ok);
        if (ok) {
            currentExpression = formatNumber(val);
            display->setText(currentExpression);
        }
        return;
    }

    // Memory
    if (text == "MC") {
        justEvaluated = false;
        memoryValue = 0;
        updateMemoryButtons();
        return;
    }

    if (text == "MR") {
        justEvaluated = false;
        currentExpression += QString::number(memoryValue, 'g', 10);
        display->setText(currentExpression);
        return;
    }

    if (text == "M+" || text == "M-") {
        justEvaluated = false;
        try {
            double val = ExpressionParser::evaluate(currentExpression.toStdString(), engine);
            if (!std::isnan(val) && !std::isinf(val)) {
                if (text == "M+") memoryValue += val;
                else memoryValue -= val;
                updateMemoryButtons();
            }
        } catch (...) {}
        return;
    }

    // Numbers and decimal point
    if (currentExpression == "0" || justEvaluated) {
        currentExpression = text;
        justEvaluated = false;
    } else {
        currentExpression += text;
    }
    display->setText(currentExpression);
}

void CalculatorWidget::setTheme(Theme theme) {
    currentTheme = theme;
    applyStyle(theme);
}

void CalculatorWidget::applyStyle(Theme theme) {
    switch (theme) {
    case Theme::Light: // Light
        setStyleSheet(
            "QWidget { background-color: #ecf0f1; color: black; }"
            "QLineEdit { background-color: white; color: black; border: 1px solid #bdc3c7; padding: 5px; }"
            "QPushButton { background-color: #bdc3c7; border: none; padding: 8px; font-size: 13px; border-radius: 4px; }"
            "QPushButton:hover { background-color: #95a5a6; }"
            "QPushButton:disabled { background-color: #d5dbdb; color: #bdc3c7; }"
            "#equalsBtn { background-color: #e67e22; color: white; }"
            "#equalsBtn:hover { background-color: #d35400; }"
            "QListWidget { background-color: white; color: black; border: 1px solid #bdc3c7; }"
            "QListWidget::item:selected { background-color: #3498db; color: white; }"
            "QLabel { color: black; }"
        );
        break;
    case Theme::Dark: // Dark
        setStyleSheet(
            "QWidget { background-color: #2c3e50; color: white; }"
            "QLineEdit { background-color: #34495e; color: white; border: 1px solid #ecf0f1; padding: 5px; }"
            "QPushButton { background-color: #3498db; border: none; padding: 8px; font-size: 13px; border-radius: 4px; }"
            "QPushButton:hover { background-color: #2980b9; }"
            "QPushButton:disabled { background-color: #1a252f; color: #7f8c8d; }"
            "#equalsBtn { background-color: #e67e22; color: white; }"
            "#equalsBtn:hover { background-color: #d35400; }"
            "QListWidget { background-color: #34495e; color: white; border: 1px solid #ecf0f1; }"
            "QListWidget::item:selected { background-color: #3498db; }"
            "QLabel { color: white; }"
        );
        break;
    case Theme::Blue: // Blue
        setStyleSheet(
            "QWidget { background-color: #1a1a2e; color: #e0e0e0; }"
            "QLineEdit { background-color: #16213e; color: #e0e0e0; border: 1px solid #0f3460; padding: 5px; }"
            "QPushButton { background-color: #0f3460; border: none; padding: 8px; font-size: 13px; border-radius: 4px; }"
            "QPushButton:hover { background-color: #1a5276; }"
            "QPushButton:disabled { background-color: #0a1628; color: #4a5568; }"
            "#equalsBtn { background-color: #e94560; color: white; }"
            "#equalsBtn:hover { background-color: #d63851; }"
            "QListWidget { background-color: #16213e; color: #e0e0e0; border: 1px solid #0f3460; }"
            "QListWidget::item:selected { background-color: #0f3460; }"
            "QLabel { color: #e0e0e0; }"
        );
        break;
    case Theme::Green: // Green
        setStyleSheet(
            "QWidget { background-color: #1b4332; color: #d4edda; }"
            "QLineEdit { background-color: #2d6a4f; color: #d4edda; border: 1px solid #40916c; padding: 5px; }"
            "QPushButton { background-color: #40916c; border: none; padding: 8px; font-size: 13px; border-radius: 4px; }"
            "QPushButton:hover { background-color: #52b788; }"
            "QPushButton:disabled { background-color: #1b4332; color: #6c757d; }"
            "#equalsBtn { background-color: #e76f51; color: white; }"
            "#equalsBtn:hover { background-color: #d65a40; }"
            "QListWidget { background-color: #2d6a4f; color: #d4edda; border: 1px solid #40916c; }"
            "QListWidget::item:selected { background-color: #40916c; }"
            "QLabel { color: #d4edda; }"
        );
        break;
    case Theme::Purple: // Purple
        setStyleSheet(
            "QWidget { background-color: #1a0a2e; color: #e0d4f0; }"
            "QLineEdit { background-color: #2d1b69; color: #e0d4f0; border: 1px solid #4a2c8a; padding: 5px; }"
            "QPushButton { background-color: #4a2c8a; border: none; padding: 8px; font-size: 13px; border-radius: 4px; }"
            "QPushButton:hover { background-color: #6b3fa0; }"
            "QPushButton:disabled { background-color: #1a0a2e; color: #6c5b7a; }"
            "#equalsBtn { background-color: #e8491d; color: white; }"
            "#equalsBtn:hover { background-color: #d43d15; }"
            "QListWidget { background-color: #2d1b69; color: #e0d4f0; border: 1px solid #4a2c8a; }"
            "QListWidget::item:selected { background-color: #4a2c8a; }"
            "QLabel { color: #e0d4f0; }"
        );
        break;
    case Theme::HighContrast: // High Contrast
        setStyleSheet(
            "QWidget { background-color: #000000; color: #ffffff; }"
            "QLineEdit { background-color: #1a1a1a; color: #ffffff; border: 2px solid #ffffff; padding: 5px; }"
            "QPushButton { background-color: #333333; border: 2px solid #ffffff; padding: 8px; font-size: 13px; border-radius: 4px; color: #ffffff; }"
            "QPushButton:hover { background-color: #555555; }"
            "QPushButton:disabled { background-color: #111111; color: #666666; border: 1px solid #444444; }"
            "#equalsBtn { background-color: #0044cc; color: #ffffff; border: 2px solid #ffffff; }"
            "#equalsBtn:hover { background-color: #0066ff; }"
            "QListWidget { background-color: #1a1a1a; color: #ffffff; border: 2px solid #ffffff; }"
            "QListWidget::item:selected { background-color: #0044cc; }"
            "QLabel { color: #ffffff; }"
        );
        break;
    }
}

void CalculatorWidget::cycleTheme() {
    int next = (static_cast<int>(currentTheme) + 1) % static_cast<int>(Theme::Count);
    currentTheme = static_cast<Theme>(next);
    if (themeBtn) themeBtn->setText("Theme: " + themeName(currentTheme));
    setTheme(currentTheme);
}

void CalculatorWidget::updateButtonLabels() {
    if (sinBtn) sinBtn->setText(secondMode ? "sin⁻¹" : "sin");
    if (cosBtn) cosBtn->setText(secondMode ? "cos⁻¹" : "cos");
    if (tanBtn) tanBtn->setText(secondMode ? "tan⁻¹" : "tan");
    if (lnBtn)  lnBtn->setText(secondMode ? "eˣ" : "ln");
}

QString CalculatorWidget::themeName(Theme t) {
    static const char* names[] = {
        "Light", "Dark", "Blue", "Green", "Purple", "High Contrast"
    };
    int idx = static_cast<int>(t);
    return (idx >= 0 && idx < static_cast<int>(Theme::Count)) ? names[idx] : "Unknown";
}

void CalculatorWidget::loadHistory() {
    QSettings settings("Calculator", "Qt6Calculator");
    int size = settings.beginReadArray("history");
    for (int i = 0; i < size; ++i) {
        settings.setArrayIndex(i);
        HistoryEntry entry;
        entry.expression = settings.value("expression").toString();
        entry.result = settings.value("result").toString();
        history.append(entry);
        makeHistoryItem(historyList, entry.expression, entry.result);
    }
    settings.endArray();
}

void CalculatorWidget::saveHistory() {
    QSettings settings("Calculator", "Qt6Calculator");
    settings.beginWriteArray("history");
    for (int i = 0; i < history.size(); ++i) {
        settings.setArrayIndex(i);
        settings.setValue("expression", history[i].expression);
        settings.setValue("result", history[i].result);
    }
    settings.endArray();
}

void CalculatorWidget::resizeEvent(QResizeEvent *event) {
    int w = width();
    int calcW = historyVisible ? w * 3 / 4 : w;

    int displaySize = qBound(16, calcW / 12, 40);
    QFont df("Arial", displaySize);
    display->setFont(df);

    int btnSize = qBound(9, calcW / 32, 16);
    QFont bf("Arial", btnSize);
    setFont(bf);

    QWidget::resizeEvent(event);
}

void CalculatorWidget::timerEvent(QTimerEvent *event) {
    if (event->timerId() == keyBufferTimerId) {
        killTimer(keyBufferTimerId);
        keyBufferTimerId = 0;
        keyBuffer.clear();
    }
}

void CalculatorWidget::keyPressEvent(QKeyEvent *event) {
    if (event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter) {
        keyBuffer.clear();
        onButtonClicked("=");
    } else if (event->key() == Qt::Key_Backspace) {
        keyBuffer.clear();
        onButtonClicked("DEL");
    } else if (event->key() == Qt::Key_Escape) {
        keyBuffer.clear();
        onButtonClicked("C");
    } else if (event->key() == Qt::Key_Up) {
        keyBuffer.clear();
        if (!history.isEmpty() && historyIndex < history.size() - 1) {
            if (historyIndex == -1)
                pendingExpression = currentExpression;
            historyIndex++;
            currentExpression = history[history.size() - 1 - historyIndex].expression;
            display->setText(currentExpression);
        }
    } else if (event->key() == Qt::Key_Down) {
        keyBuffer.clear();
        if (historyIndex > 0) {
            historyIndex--;
            currentExpression = history[history.size() - 1 - historyIndex].expression;
            display->setText(currentExpression);
        } else if (historyIndex == 0) {
            historyIndex = -1;
            currentExpression = pendingExpression;
            display->setText(currentExpression);
        }
    } else if (event->modifiers() == Qt::ControlModifier && event->key() == Qt::Key_T) {
        keyBuffer.clear();
        cycleTheme();
    } else if (event->modifiers() == Qt::ControlModifier && event->key() == Qt::Key_C) {
        QApplication::clipboard()->setText(display->text());
    } else if (event->modifiers() == Qt::ControlModifier && event->key() == Qt::Key_V) {
        keyBuffer.clear();
        QString clip = QApplication::clipboard()->text();
        if (!clip.isEmpty()) {
            if (currentExpression == "0")
                currentExpression = clip;
            else
                currentExpression += clip;
            display->setText(currentExpression);
        }
    } else if (!event->text().isEmpty()) {
        QString t = event->text();

        // Buffer alpha chars to detect multi-key function names
        if (t[0].isLetter()) {
            keyBuffer += t.toLower();
            if (keyBufferTimerId) killTimer(keyBufferTimerId);
            keyBufferTimerId = startTimer(1200);

            static const QStringList fns = {"sin", "cos", "tan", "ln", "log", "sqrt", "perc"};
            for (const auto& fn : fns) {
                if (keyBuffer == fn) {
                    keyBuffer.clear();
                    onButtonClicked(fn);
                    return;
                }
                // Partial match: continue buffering
                if (fn.startsWith(keyBuffer))
                    return;
            }
            // No partial match either → flush as regular chars
            for (int i = 0; i < keyBuffer.size(); ++i)
                onButtonClicked(QString(keyBuffer[i]));
            keyBuffer.clear();
            return;
        }

        keyBuffer.clear();
        if (t == "%") {
            onButtonClicked("perc");
            return;
        }
        if (t[0].isDigit() || t == "." || t == "+" || t == "-"
            || t == "*" || t == "/" || t == "^" || t == "(" || t == ")") {
            onButtonClicked(t);
        }
    }
}
