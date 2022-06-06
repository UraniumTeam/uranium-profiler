#pragma once
#include "ProfilingSession.h"
#include <QAction>
#include <QCursor>
#include <QFrame>
#include <optional>

struct FunctionCall
{
    UN::ProfilingSession* Session;
    size_t BeginIndex;
    size_t EndIndex;

    FunctionCall(UN::ProfilingSession* session, size_t beginIndex, size_t endIndex)
        : Session(session)
        , BeginIndex(beginIndex)
        , EndIndex(endIndex)
    {
    }

    [[nodiscard]] UN::SessionEvent& begin() const
    {
        return Session->events()[BeginIndex];
    }

    [[nodiscard]] UN::SessionEvent& end() const
    {
        return Session->events()[EndIndex];
    }
};

inline bool operator==(const FunctionCall& lhs, const FunctionCall& rhs)
{
    return &lhs.begin() == &rhs.begin();
}

class MainFrame : public QFrame
{
    Q_OBJECT

    std::vector<UN::ProfilingSession> m_ProfilingSessions;

    uint64_t m_CurrentSessionBegin;
    uint64_t m_CurrentSessionEnd;

    double m_PixelsPerTick;
    int64_t m_StartPosition;
    int m_FunctionHeight;

    double m_WheelSensitivity;
    bool m_RightMousePressed;
    QPoint m_LocalMousePosition;
    QPoint m_GlobalMousePosition;
    QPoint m_LastGlobalMousePosition;

    void drawFunction(
        QPainter& painter, const std::string& functionName, int x, int y, int w, bool isHovered, bool isSelected) const;
    void drawThread(QPainter& painter, int index, const QRect& rect);
    static QColor getFunctionColor(const char* functionName);
    [[nodiscard]] int threadHeight(int index) const;

protected:
    void paintEvent(QPaintEvent* event) override;

    [[nodiscard]] QSize sizeHint() const override
    {
        return ((QWidget*)parent())->size();
    }

    void mouseMoveEvent(QMouseEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void wheelEvent(QWheelEvent* event) override;

public:
    explicit MainFrame(QWidget* parent = nullptr);

    QAction* FunctionHoverChanged;
    QAction* FunctionSelectionChanged;

    std::optional<FunctionCall> HoveredFunction;
    std::optional<FunctionCall> SelectedFunction;

    [[nodiscard]] int64_t mousePositionInTicks() const;
    void addProfilingSession(const UN::ProfilingSession& session);
    void clearProfilingSessions();

    ~MainFrame() override = default;
};
