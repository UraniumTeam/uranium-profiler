#pragma once
#include "ProfilingSession.h"
#include <QFrame>
#include <QCursor>

class MainFrame : public QFrame
{
    Q_OBJECT

    std::vector<UN::ProfilingSession> m_ProfilingSessions;

    double m_PixelsPerTick;
    int64_t m_StartPosition;
    int m_FunctionHeight;

    double m_WheelSensitivity;
    bool m_MousePressed;
    QPoint m_LocalMousePosition;
    QPoint m_GlobalMousePosition;
    QPoint m_LastGlobalMousePosition;

    void drawFunction(QPainter& painter, const std::string& functionName, int x, int y, int w, bool isSelected) const;
    void drawThread(QPainter& painter, int index, const QRect& rect) const;
    static QColor getFunctionColor(const char* functionName);
    [[nodiscard]] int threadHeight(int index) const;

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
    [[nodiscard]] int64_t mousePositionInTicks() const;
    void addProfilingSession(const UN::ProfilingSession& session);
    void clearProfilingSessions();
    ~MainFrame() override = default;
};
