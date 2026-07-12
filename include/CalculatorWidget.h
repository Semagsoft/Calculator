#ifndef CALCULATOR_WIDGET_H
#define CALCULATOR_WIDGET_H

#include <QWidget>
#include <QLineEdit>
#include <QListWidget>
#include <QLabel>
#include <QKeyEvent>
#include <QTimerEvent>
#include <QVector>
#include <QStringList>
#include <QSettings>
#include "CalculatorEngine.h"
#include "ExpressionParser.h"

class QPushButton;

struct HistoryEntry {
    QString expression;
    QString result;
};

class CalculatorWidget : public QWidget {
    Q_OBJECT

public:
    enum class Theme { Light, Dark, Blue, Green, Purple, HighContrast, Count };

    explicit CalculatorWidget(QWidget *parent = nullptr);
    void setTheme(Theme theme);

protected:
    void resizeEvent(QResizeEvent *event) override;
    void timerEvent(QTimerEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;

private slots:
    void onButtonClicked(const QString& text);

private:
    void setupUI();
    void applyStyle(Theme theme);
    void updateMemoryButtons();
    void updateStatusLabel();
    void addHistoryEntry(const QString& expr, const QString& result);
    QString formatNumber(double result) const;
    void cycleTheme();
    void loadHistory();
    void saveHistory();
    void updateButtonLabels();
    static QString themeName(Theme t);

    CalculatorEngine engine;
    QLineEdit* display;
    QString currentExpression;
    Theme currentTheme = Theme::Light;
    bool justEvaluated = false;

    double memoryValue = 0;
    QPushButton* mcBtn = nullptr;
    QPushButton* mrBtn = nullptr;
    QPushButton* angleBtn = nullptr;
    QPushButton* sciBtn = nullptr;
    QPushButton* themeBtn = nullptr;
    QPushButton* secondBtn = nullptr;
    QPushButton* sinBtn = nullptr;
    QPushButton* cosBtn = nullptr;
    QPushButton* tanBtn = nullptr;
    QPushButton* lnBtn = nullptr;
    QLabel* memIndicator = nullptr;
    QLabel* statusLabel = nullptr;

    bool secondMode = false;

    QVector<HistoryEntry> history;
    int historyIndex = -1;
    QString pendingExpression;

    QListWidget* historyList = nullptr;
    QWidget* historyPanel = nullptr;
    bool historyVisible = true;
    bool scientificNotation = false;

    QString keyBuffer;
    int keyBufferTimerId = 0;
};

#endif // CALCULATOR_WIDGET_H
