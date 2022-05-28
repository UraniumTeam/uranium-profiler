#pragma once
#include "ProfilingSession.h"
#include <QFrame>
#include <QCursor>

class MainFrame : public QFrame
{
    Q_OBJECT

    UN::ProfilingSession m_ProfilingSession;

    double m_PixelsPerTick;
    int64_t m_StartPosition;
    int m_FunctionHeight;

    double m_WheelSensitivity;
    bool m_MousePressed;
    QPoint m_MousePosition;
    QPoint m_LastMousePosition;

    void drawFunction(QPainter& painter, const std::string& functionName, int x, int y, int w) const;
    static QColor getFunctionColor(const char* functionName);

protected:
    void paintEvent(QPaintEvent* event) override;

    [[nodiscard]] QSize sizeHint() const override {
        return ((QWidget*) parent())->size();
    }

    void mouseMoveEvent(QMouseEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void wheelEvent(QWheelEvent* event) override;

public:
    explicit MainFrame(QWidget* parent = nullptr);
    void setProfilingSession(const UN::ProfilingSession& session);
    ~MainFrame() override = default;
};
