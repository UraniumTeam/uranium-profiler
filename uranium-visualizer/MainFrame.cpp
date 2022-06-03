#include "MainFrame.h"
#include "TimelinePainter.h"
#include <QMouseEvent>
#include <QPainter>
#include <array>
#include <stack>

MainFrame::MainFrame(QWidget* parent)
    : QFrame(parent)
    , m_PixelsPerTick(0.06)
    , m_WheelSensitivity(1.05)
    , m_FunctionHeight(30)
    , m_ThreadHeight(100)
    , m_MousePressed(false)
    , m_StartPosition(0)
{
    setMouseTracking(true);
    setFrameStyle(QFrame::Panel | QFrame::Sunken);
    setFocusPolicy(Qt::StrongFocus);
}

void MainFrame::paintEvent(QPaintEvent* e)
{
    if (!isEnabled()) {
        return;
    }
    QFrame::paintEvent(e);

    QPainter painter(this);
    auto rect = contentsRect();

    for (int i = 0; i < m_ProfilingSessions.size(); ++i)
    {
        drawThread(painter, i, rect);
    }

    UN::TimelinePainter tlPainter(painter, m_ProfilingSessions[0].Header().NanosecondsInTick());
    tlPainter.setRegion(m_StartPosition, m_PixelsPerTick);
    tlPainter.setColor(Qt::white);
    tlPainter.setRect(rect);
    tlPainter.setWidth(m_FunctionHeight);
    tlPainter.setLineWidth(1);
    tlPainter.draw();
}

void MainFrame::drawThread(QPainter& painter, int index, const QRect& rect) const
{
    auto& session = m_ProfilingSessions[index];
    std::stack<UN::SessionEvent> eventStack;
    for (const auto& event : session.Events())
    {
        if (event.EventType() == UN::EventType::Begin)
        {
            eventStack.push(event);
            continue;
        }

        auto beginEvent = eventStack.top();
        eventStack.pop();
        auto startPos = (double)((int64_t)beginEvent.CpuTicks() - m_StartPosition) * m_PixelsPerTick;
        auto endPos = (double)((int64_t)event.CpuTicks() - m_StartPosition) * m_PixelsPerTick;

        if (startPos >= rect.right() && endPos >= rect.right() ||
            startPos <= rect.left() && endPos <= rect.left()) {
            continue;
        }

        const int pad = 5;
        startPos = std::clamp(startPos, (double)(rect.left() - pad), (double)(rect.right() + pad));
        endPos = std::clamp(endPos, (double)(rect.left() - pad), (double)(rect.right() + pad));

        const auto& name = session.Header().FunctionNames()[event.FunctionIndex()];
        auto yPosition = m_FunctionHeight * (int)(eventStack.size() + 1) + m_ThreadHeight * index + 5;
        drawFunction(painter, name, (int)startPos, yPosition, (int)(endPos - startPos));
    }
}

void MainFrame::drawFunction(QPainter& painter, const std::string& functionName, int x, int y, int w) const
{
    auto color = getFunctionColor(functionName.c_str());
    painter.fillRect(x + 1, y + 1, w - 2, m_FunctionHeight - 2, color);

    painter.setPen(QPen(color.lighter(), 1));
    painter.drawLine(x, y + m_FunctionHeight - 1, x, y);
    painter.drawLine(x, y, x + w - 1, y);
    painter.drawLine(x + 1, y + m_FunctionHeight - 1, x + w - 1, y + m_FunctionHeight - 1);
    painter.drawLine(x + w - 1, y + m_FunctionHeight - 1, x + w - 1, y + 1);

    painter.setPen(QPen(color.darker(), 3));
    QTextOption opt;
    opt.setAlignment(Qt::AlignCenter);
    painter.drawText(QRect(x + 1, y + 1, w - 2, m_FunctionHeight - 2), functionName.c_str(), opt);
}

QColor MainFrame::getFunctionColor(const char* functionName)
{
    static constexpr auto colorTable = std::array{ 0xCC6666, 0x66CC66, 0x6666CC, 0xCCCC66, 0xCC66CC, 0x66CCCC, 0xDAAA00 };

    auto hash = std::hash<const char*>()(functionName);
    return colorTable[hash % colorTable.size()];
}

void MainFrame::mouseMoveEvent(QMouseEvent* event)
{
    QWidget::mouseMoveEvent(event);

    m_MousePosition = QCursor::pos();
    if (m_MousePressed)
    {
        auto diffX     = m_LastMousePosition.x() - m_MousePosition.x();
        auto diffTicks = diffX / m_PixelsPerTick;
        m_StartPosition += (int64_t)diffTicks;

        m_MousePosition = m_LastMousePosition;
        QCursor::setPos(m_LastMousePosition);
    }
    m_LastMousePosition = m_MousePosition;
    update();
}

void MainFrame::mousePressEvent(QMouseEvent* event)
{
    QWidget::mousePressEvent(event);
    if (event->button() == Qt::MouseButton::RightButton)
    {
        m_MousePressed = true;
    }
}

void MainFrame::mouseReleaseEvent(QMouseEvent* event)
{
    QWidget::mouseReleaseEvent(event);
    if (event->button() == Qt::MouseButton::RightButton)
    {
        m_MousePressed = false;
    }
}

void MainFrame::wheelEvent(QWheelEvent* event)
{
    QWidget::wheelEvent(event);

    auto startPosition = m_StartPosition + (int64_t)(m_MousePosition.x() / m_PixelsPerTick);

    if (event->angleDelta().y() < 0)
    {
        m_PixelsPerTick /= m_WheelSensitivity;
    }
    else
    {
        m_PixelsPerTick *= m_WheelSensitivity;
    }

    const double minPixelsPerTick = 1e-8;
    if (m_PixelsPerTick < minPixelsPerTick)
    {
        m_PixelsPerTick = minPixelsPerTick;
    }

    m_StartPosition = startPosition - (int64_t)(m_MousePosition.x() / m_PixelsPerTick);

    update();
}

void MainFrame::addProfilingSession(const UN::ProfilingSession& session)
{
    auto& s = m_ProfilingSessions.emplace_back(session);
    m_StartPosition = (int64_t)s.Events()[0].CpuTicks();
    //s.SortEvents();
}

void MainFrame::ClearProfilingSessions() {
    m_ProfilingSessions.clear();
    m_PixelsPerTick = 0.06;
    m_StartPosition = 0;
}
