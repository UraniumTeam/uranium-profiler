#pragma once
#include "ProfilingSession.h"
#include <QFrame>

class MainFrame : public QFrame
{
    Q_OBJECT

    UN::ProfilingSession m_ProfilingSession;

    double m_PixelsPerTick;
    uint64_t m_StartPosition;
    int m_FunctionHeight;

    void drawFunction(QPainter& painter, const std::string& functionName, int x, int y, int w);
    static QColor getFunctionColor(const char* functionName);

protected:
    void paintEvent(QPaintEvent* event) override;

    [[nodiscard]] QSize sizeHint() const override {
        return ((QWidget*) parent())->size();
    }

    // void mousePressEvent(QMouseEvent* event) override;
    // void mouseReleaseEvent(QMouseEvent* event) override;

public:
    explicit MainFrame(QWidget* parent = nullptr);
    ~MainFrame() override = default;
};
