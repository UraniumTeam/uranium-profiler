#include "MainFrame.h"
#include "TimelinePainter.h"
#include <QMouseEvent>
#include <QPainter>
#include <array>
#include <stack>

MainFrame::MainFrame(QWidget* parent)
    : QFrame(parent)
    , m_PixelsPerTick(0.002)
    , m_WheelSensitivity(1.05)
    , m_FunctionHeight(30)
    , m_MousePressed(false)
    , m_StartPosition(0)
{
    setMouseTracking(true);
    setFrameStyle(QFrame::Panel | QFrame::Sunken);
    setFocusPolicy(Qt::StrongFocus);

    connect(&m_FunctionSelectedAction, &QAction::triggered, this, [this]() {
        QCursor cursor;
        if (m_HasSelectedFunction)
        {
            cursor.setShape(Qt::CrossCursor);
        }
        setCursor(cursor);
    });
}

int64_t MainFrame::mousePositionInTicks() const
{
    return m_StartPosition + (double)m_LocalMousePosition.x() / m_PixelsPerTick;
}

int MainFrame::threadHeight(int index) const
{
    return m_FunctionHeight * 4;
}

void MainFrame::paintEvent(QPaintEvent* e)
{
    if (!isEnabled())
    {
        return;
    }
    QFrame::paintEvent(e);

    QPainter painter(this);
    auto rect = contentsRect();

    auto wasSelected = m_HasSelectedFunction;
    m_HasSelectedFunction = false;
    for (int i = 0; i < m_ProfilingSessions.size(); ++i)
    {
        m_HasSelectedFunction |= drawThread(painter, i, rect);
    }
    if (wasSelected != m_HasSelectedFunction)
    {
        m_FunctionSelectedAction.trigger();
    }

    UN::TimelinePainter tlPainter(painter, m_ProfilingSessions[0].Header().NanosecondsInTick());
    tlPainter.setRegion(m_StartPosition, m_PixelsPerTick);
    tlPainter.setColor(Qt::white);
    tlPainter.setRect(rect);
    tlPainter.setWidth(m_FunctionHeight);
    tlPainter.setLineWidth(1);
    tlPainter.draw();
}

bool MainFrame::drawThread(QPainter& painter, int index, const QRect& rect)
{
    auto mousePosition = mousePositionInTicks();
    auto& session      = m_ProfilingSessions[index];
    std::stack<UN::SessionEvent> eventStack;

    int selX, selY, selW;
    uint32_t selIndex;
    auto hasSelected =  false;
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
        auto endPos   = (double)((int64_t)event.CpuTicks() - m_StartPosition) * m_PixelsPerTick;

        if (startPos >= rect.right() && endPos >= rect.right() || startPos <= rect.left() && endPos <= rect.left())
        {
            continue;
        }

        const int pad = 5;
        startPos      = std::clamp(startPos, (double)(rect.left() - pad), (double)(rect.right() + pad));
        endPos        = std::clamp(endPos, (double)(rect.left() - pad), (double)(rect.right() + pad));

        const auto& name = session.Header().FunctionNames()[event.FunctionIndex()];
        auto yPosition   = m_FunctionHeight * (int)(eventStack.size() + 1) + threadHeight(index) * index + 5;
        auto isSelected  = mousePosition >= beginEvent.CpuTicks() && mousePosition < event.CpuTicks()
            && m_LocalMousePosition.y() >= yPosition && m_LocalMousePosition.y() < yPosition + m_FunctionHeight;
        if (isSelected)
        {
            selIndex              = event.FunctionIndex();
            selX                  = (int)startPos;
            selY                  = yPosition;
            selW                  = (int)(endPos - startPos);
            hasSelected = true;
        }
        else
        {
            drawFunction(painter, name, (int)startPos, yPosition, (int)(endPos - startPos), false);
        }
    }
    if (hasSelected)
    {
        const auto& name = session.Header().FunctionNames()[selIndex];
        drawFunction(painter, name, selX, selY, selW, true);
    }

    return hasSelected;
}

void MainFrame::drawFunction(QPainter& painter, const std::string& functionName, int x, int y, int w, bool isSelected)
{
    auto color = getFunctionColor(functionName.c_str());
    if (isSelected)
    {
        color = color.lighter();
    }
    painter.fillRect(x + 1, y + 1, w - 2, m_FunctionHeight - 2, color);

    if (isSelected)
    {
        painter.setPen(QPen(Qt::green, 2));
        painter.drawLine(x, y + m_FunctionHeight - 1, x, 0);
        painter.drawLine(x + 1, y + m_FunctionHeight - 1, x + w - 1, y + m_FunctionHeight - 1);
        painter.drawLine(x + w - 1, y + m_FunctionHeight - 1, x + w - 1, 0);
    }
    else
    {
        painter.setPen(QPen(color.lighter(), 1));
        painter.drawLine(x, y + m_FunctionHeight - 1, x, y);
        painter.drawLine(x, y, x + w - 1, y);
        painter.drawLine(x + 1, y + m_FunctionHeight - 1, x + w - 1, y + m_FunctionHeight - 1);
        painter.drawLine(x + w - 1, y + m_FunctionHeight - 1, x + w - 1, y + 1);
    }

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

    m_LocalMousePosition  = event->pos();
    m_GlobalMousePosition = QCursor::pos();
    if (m_MousePressed)
    {
        auto diffX     = m_LastGlobalMousePosition.x() - m_GlobalMousePosition.x();
        auto diffTicks = diffX / m_PixelsPerTick;
        m_StartPosition += (int64_t)diffTicks;

        // m_GlobalMousePosition = m_LastGlobalMousePosition;
        // QCursor::setPos(m_LastGlobalMousePosition);
    }
    m_LastGlobalMousePosition = m_GlobalMousePosition;
    update();
    repaint();
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

    auto startPosition = m_StartPosition + (int64_t)(m_GlobalMousePosition.x() / m_PixelsPerTick);

    if (event->angleDelta().y() < 0)
    {
        if (event->modifiers() & Qt::ControlModifier)
        {
            m_FunctionHeight /= m_WheelSensitivity;
        }
        else
        {
            m_PixelsPerTick /= m_WheelSensitivity;
        }
    }
    else
    {
        if (event->modifiers() & Qt::ControlModifier)
        {
            m_FunctionHeight *= m_WheelSensitivity;
        }
        else
        {
            m_PixelsPerTick *= m_WheelSensitivity;
        }
    }

    const double minPixelsPerTick = 1e-8;
    if (m_PixelsPerTick < minPixelsPerTick)
    {
        m_PixelsPerTick = minPixelsPerTick;
    }
    if (m_FunctionHeight < 30)
    {
        m_FunctionHeight = 30;
    }

    m_StartPosition = startPosition - (int64_t)(m_GlobalMousePosition.x() / m_PixelsPerTick);

    update();
}

void MainFrame::addProfilingSession(const UN::ProfilingSession& session)
{
    auto& s         = m_ProfilingSessions.emplace_back(session);
    m_StartPosition = (int64_t)s.Events()[0].CpuTicks();
}

void MainFrame::clearProfilingSessions()
{
    m_ProfilingSessions.clear();
    m_PixelsPerTick = 0.002;
    m_StartPosition = 0;
}
