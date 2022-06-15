#include "MainFrame.h"
#include "FunctionColors.h"
#include "TimelinePainter.h"
#include <QMouseEvent>
#include <QPainter>
#include <functional>
#include <stack>
#include <unordered_map>

MainFrame::MainFrame(QWidget* parent)
    : QFrame(parent)
    , m_PixelsPerTick(0.002)
    , m_WheelSensitivity(1.05)
    , m_FunctionHeight(25)
    , m_TimelineHeight(30)
    , m_RightMousePressed(false)
    , m_StartPosition(0)
{
    setMouseTracking(true);
    setFrameStyle(QFrame::Panel | QFrame::Sunken);
    setFocusPolicy(Qt::StrongFocus);
    clearProfilingSessions();

    FunctionSelectionChanged = new QAction(this);
    FunctionHoverChanged     = new QAction(this);

    connect(FunctionHoverChanged, &QAction::triggered, this, [this]() {
        QCursor cursor;
        if (HoveredFunction.has_value())
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
    if (index == 0)
    {
        return 0;
    }
    return threadHeight(index - 1) +  m_FunctionHeight * (int)m_ProfilingSessions[index - 1].globalStats().MaxHeight + 5;
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

    auto wasHovered = HoveredFunction.has_value();
    HoveredFunction.reset();
    for (int i = 0; i < m_ProfilingSessions.size(); ++i)
    {
        drawThread(painter, i, rect);
    }
    if (wasHovered != HoveredFunction.has_value())
    {
        FunctionHoverChanged->trigger();
    }

    UN::TimelinePainter tlPainter(painter, m_ProfilingSessions[0].header().nanosecondsInTick());
    tlPainter.setRegion(m_StartPosition, m_PixelsPerTick);
    tlPainter.setColor(Qt::white);
    tlPainter.setRect(rect);
    tlPainter.setWidth(m_TimelineHeight);
    tlPainter.setLineWidth(1);
    tlPainter.draw();
}

void MainFrame::drawThread(QPainter& painter, int index, const QRect& rect)
{
    auto mousePosition = mousePositionInTicks();
    auto& session      = m_ProfilingSessions[index];
    std::stack<size_t> eventStack;

    auto threadOffset = threadHeight(index) + m_TimelineHeight;
    std::vector<std::function<void()>> renderLater;
    for (size_t i = 0; i < session.events().size(); ++i)
    {
        auto& event = session.events()[i];
        if (event.type() == UN::EventType::Begin)
        {
            eventStack.push(i);
            continue;
        }

        auto beginIndex  = eventStack.top();
        auto& beginEvent = session.events()[eventStack.top()];
        eventStack.pop();
        auto startPos = (double)((int64_t)beginEvent.cpuTicks() - m_StartPosition) * m_PixelsPerTick;
        auto endPos   = (double)((int64_t)event.cpuTicks() - m_StartPosition) * m_PixelsPerTick;

        if (startPos >= rect.right() && endPos >= rect.right() || startPos <= rect.left() && endPos <= rect.left())
        {
            continue;
        }

        const int pad = 5;
        startPos      = std::clamp(startPos, (double)(rect.left() - pad), (double)(rect.right() + pad));
        endPos        = std::clamp(endPos, (double)(rect.left() - pad), (double)(rect.right() + pad));

        const auto& name = session.header().functionNames()[event.functionIndex()];
        auto yPosition   = m_FunctionHeight * (int)eventStack.size() + threadOffset + 5;
        auto isSelected  = SelectedFunction.has_value() && &SelectedFunction.value().begin() == &beginEvent;
        auto isHovered   = mousePosition >= beginEvent.cpuTicks() && mousePosition < event.cpuTicks()
            && m_LocalMousePosition.y() >= yPosition && m_LocalMousePosition.y() < yPosition + m_FunctionHeight;
        if (isHovered)
        {
            HoveredFunction = FunctionCall(&session, beginIndex, i);
        }
        if (isSelected || isHovered)
        {
            renderLater.emplace_back([this, &painter, name, startPos, yPosition, endPos, isHovered, isSelected]() {
                drawFunction(painter, name, (int)startPos, yPosition, (int)(endPos - startPos), isHovered, isSelected);
            });
        }
        else
        {
            drawFunction(painter, name, (int)startPos, yPosition, (int)(endPos - startPos), isHovered, false);
        }
    }
    for (auto& f : renderLater)
    {
        f();
    }

    if (index > 0)
    {
        painter.fillRect(rect.left(), threadOffset + 1, rect.width(), 3, QColor(0x90a0c0));
    }
}

void MainFrame::drawFunction(
    QPainter& painter, const std::string& functionName, int x, int y, int w, bool isHovered, bool isSelected) const
{
    auto color = getFunctionColor(functionName.c_str());
    if (w <= 3)
    {
        if (isSelected)
        {
            painter.setPen(QPen(Qt::green, w));
            painter.drawLine(x, y + m_FunctionHeight - 1, x, 0);
        }
        else
        {
            painter.setPen(QPen(color, w));
            painter.drawLine(x, y + m_FunctionHeight - 1, x, y);
        }
        return;
    }
    if (isHovered)
    {
        color = color.lighter();
    }
    if (isSelected || isHovered)
    {
        painter.fillRect(x, 0, w, m_FunctionHeight + y, QColor::fromRgb(32, 32, 32, 32));
    }

    painter.fillRect(x, y, w, m_FunctionHeight, color);

    if (isSelected)
    {
        painter.setPen(QPen(Qt::green, 2));
        painter.drawLine(x, y + m_FunctionHeight - 1, x, 0);
        painter.drawLine(x + 1, y + m_FunctionHeight - 1, x + w - 1, y + m_FunctionHeight - 1);
        painter.drawLine(x + w - 1, y + m_FunctionHeight - 1, x + w - 1, 0);
    }
    else if (w > 20)
    {
        painter.setPen(QPen(color.lighter(), 1));
        painter.drawLine(x, y + m_FunctionHeight - 1, x, y);
        painter.drawLine(x, y, x + w - 1, y);
        painter.drawLine(x + 1, y + m_FunctionHeight - 1, x + w - 1, y + m_FunctionHeight - 1);
        painter.drawLine(x + w - 1, y + m_FunctionHeight - 1, x + w - 1, y + 1);
    }

    if (w > 30)
    {
        painter.setPen(QPen(color.darker(), 3));
        QRect clip(x + 1, y + 1, w - 2, m_FunctionHeight - 2);
        painter.drawText(clip, Qt::AlignCenter | Qt::TextSingleLine, functionName.c_str());
    }
}

QColor MainFrame::getFunctionColor(const char* functionName)
{
    static std::unordered_map<std::string, QColor> colors;
    if (colors.find(functionName) == colors.end())
    {
        auto hash            = std::hash<const char*>()(functionName);
        colors[functionName] = UN::FunctionColors[hash % UN::FunctionColors.size()];
    }
    return colors[functionName];
}

void MainFrame::mouseMoveEvent(QMouseEvent* event)
{
    QWidget::mouseMoveEvent(event);

    m_LocalMousePosition  = event->pos();
    m_GlobalMousePosition = QCursor::pos();
    if (m_RightMousePressed)
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
    if (event->button() == Qt::RightButton)
    {
        m_RightMousePressed = true;
    }
    if (event->button() == Qt::LeftButton)
    {
        SelectedFunction = HoveredFunction;
        FunctionSelectionChanged->trigger();
        update();
    }
}

void MainFrame::mouseReleaseEvent(QMouseEvent* event)
{
    QWidget::mouseReleaseEvent(event);
    if (event->button() == Qt::MouseButton::RightButton)
    {
        m_RightMousePressed = false;
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
    if (m_FunctionHeight < 25)
    {
        m_FunctionHeight = 25;
    }

    m_StartPosition = startPosition - (int64_t)(m_GlobalMousePosition.x() / m_PixelsPerTick);

    update();
}

void MainFrame::addProfilingSession(const UN::ProfilingSession& session)
{
    auto& s               = m_ProfilingSessions.emplace_back(session);
    m_CurrentSessionBegin = std::min(m_CurrentSessionBegin, s.globalStats().SessionStart);
    m_CurrentSessionEnd   = std::max(m_CurrentSessionEnd, s.globalStats().SessionEnd);
    m_StartPosition       = (int64_t)m_CurrentSessionBegin;
    auto length           = m_CurrentSessionEnd - m_CurrentSessionBegin;
    m_PixelsPerTick       = (double)width() / (double)length;
}

void MainFrame::clearProfilingSessions()
{
    SelectedFunction = {};
    HoveredFunction = {};
    m_ProfilingSessions.clear();
    m_CurrentSessionBegin = std::numeric_limits<uint64_t>::max();
    m_CurrentSessionEnd   = std::numeric_limits<uint64_t>::min();
}
