#include "MainFrame.h"
#include <QPainter>
#include <array>
#include <stack>

MainFrame::MainFrame(QWidget* parent)
    : QFrame(parent)
    , m_ProfilingSession(UN::ProfilingSession::GetFakeProfilingSession())
    , m_PixelsPerTick(0.06)
    , m_FunctionHeight(30)
{
    m_StartPosition = m_ProfilingSession.Events()[0].CpuTicks();
    setFrameStyle(QFrame::Panel | QFrame::Sunken);
    setFocusPolicy(Qt::StrongFocus);
}

void MainFrame::paintEvent(QPaintEvent* e)
{
    QFrame::paintEvent(e);

    QPainter painter(this);
    auto rect = contentsRect();

    std::stack<UN::SessionEvent> eventStack;
    for (const auto& event : m_ProfilingSession.Events())
    {
        if (event.EventType() == UN::EventType::Begin)
        {
            eventStack.push(event);
            continue;
        }

        auto beginEvent = eventStack.top();
        eventStack.pop();
        auto startPos = std::clamp((double)(beginEvent.CpuTicks() - m_StartPosition) * m_PixelsPerTick,
                                   (double)rect.left(), (double)rect.right());
        auto endPos   = std::clamp((double)(event.CpuTicks() - m_StartPosition) * m_PixelsPerTick,
                                   (double)rect.left(), (double)rect.right());

        if (std::abs(endPos - startPos) < 1.0)
        {
            continue;
        }

        const auto& name = m_ProfilingSession.Header().FunctionNames()[event.FunctionIndex()];
        drawFunction(painter, name, (int)startPos,
                     m_FunctionHeight * (int)eventStack.size() + 10,
                     (int)(endPos - startPos));
    }
}

void MainFrame::drawFunction(QPainter& painter, const std::string& functionName, int x, int y, int w)
{
    auto color = getFunctionColor(functionName.c_str());
    painter.fillRect(x + 1, y + 1, w - 2, m_FunctionHeight - 2, color);

    painter.setPen(color.lighter());
    painter.drawLine(x, y + m_FunctionHeight - 1, x, y);
    painter.drawLine(x, y, x + w - 1, y);
    painter.drawLine(x + 1, y + m_FunctionHeight - 1, x + w - 1, y + m_FunctionHeight - 1);
    painter.drawLine(x + w - 1, y + m_FunctionHeight - 1, x + w - 1, y + 1);

    painter.setPen(color.darker());
    QTextOption opt;
    opt.setAlignment(Qt::AlignCenter);
    painter.drawText(QRect(x + 1, y + 1, w - 2, m_FunctionHeight - 2), functionName.c_str(), opt);
}

QColor MainFrame::getFunctionColor(const char* functionName)
{
    static constexpr auto colorTable =
        std::array{ 0xCC6666, 0x66CC66, 0x6666CC, 0xCCCC66, 0xCC66CC, 0x66CCCC, 0xDAAA00 };

    auto hash = std::hash<const char*>()(functionName);
    return colorTable[hash % colorTable.size()];
}
