#include "TimelinePainter.h"

namespace UN
{
    TimelinePainter::TimelinePainter(QPainter& painter, double nanosecondsPerTick)
        : m_Painter(&painter)
        , m_NanosecondsPerTick(nanosecondsPerTick)
    {
        //m_Measures.emplace_back("n", 1 / nanosecondsPerTick);
        m_Measures.emplace_back("mcs", 1'000 / nanosecondsPerTick);
        m_Measures.emplace_back("ms", 1'000'000 / nanosecondsPerTick);
        m_Measures.emplace_back("sec", 1'000'000'000 / nanosecondsPerTick);
        m_Measures.emplace_back("min", 60'000'000'000 / nanosecondsPerTick);
        m_Measures.emplace_back("h", 3'600'000'000'000 / nanosecondsPerTick);

        auto measureCount = m_Measures.size();
        for (size_t i = 0; i < measureCount - 1; ++i)
        {
            auto curr = 10;
            while (m_Measures[i].TickCount * curr < m_Measures[i + 1].TickCount)
            {
                auto name = std::to_string(curr).substr(1) + m_Measures[i].Name;
                m_Measures.emplace_back(name, m_Measures[i].TickCount * curr);
                curr *= 10;
            }
        }

        std::sort(m_Measures.begin(), m_Measures.end(), [](const TimelineMeasure& l, const TimelineMeasure& r) {
            return l.TickCount < r.TickCount;
        });
    }

    void TimelinePainter::draw()
    {
        TimelineMeasure& usedMeasure = m_Measures[0];
        for (auto& measure : m_Measures)
        {
            auto length = measure.lengthInPixels(m_PixelsPerTick);
            if (length >= m_MinPixelWidth)
            {
                usedMeasure = measure;
                break;
            }
        }

        auto pixelLength = usedMeasure.lengthInPixels(m_PixelsPerTick);

        auto currentIndex = 0;
        double currentPos   = 0;

        m_Painter->setPen(QPen(m_Color, m_LineWidth));

        while (currentPos <= m_Rect.right())
        {
            auto width = m_Width;
            if (currentIndex % 10 != 0)
            {
                width /= 2;
            }
            else if (currentIndex == 0)
            {
                QTextOption opt;
                opt.setAlignment(Qt::AlignLeft);
                QRect textRect(currentPos + 5, m_Rect.top() + m_Width / 2, (int)m_MinPixelWidth, m_Width / 2);
                auto text = QString("+ %1%2")
                                .arg((int64_t)(m_StartTick * m_NanosecondsPerTick / usedMeasure.TickCount))
                                .arg(usedMeasure.Name.c_str());
                m_Painter->drawText(textRect, text, opt);
            }
            else
            {
                QTextOption opt;
                opt.setAlignment(Qt::AlignLeft);
                QRect textRect(currentPos + 5, m_Rect.top() + m_Width / 2, (int)m_MinPixelWidth, m_Width / 2);
                auto text = QString("%1%2").arg(currentIndex / 10).arg(usedMeasure.Name.c_str());
                m_Painter->drawText(textRect, text, opt);
            }
            m_Painter->drawLine(currentPos, m_Rect.top(), currentPos, m_Rect.top() + width);
            currentPos += pixelLength / 10;
            currentIndex++;
        }
    }

    void TimelinePainter::setRect(const QRect& rect)
    {
        m_Rect = rect;
    }

    void TimelinePainter::setRegion(int64_t startTick, double pixelsPerTick)
    {
        m_StartTick     = startTick;
        m_PixelsPerTick = pixelsPerTick;
    }

    void TimelinePainter::setWidth(int width)
    {
        m_Width = width;
    }

    void TimelinePainter::setLineWidth(int lineWidth)
    {
        m_LineWidth = lineWidth;
    }

    void TimelinePainter::setColor(const QColor& color)
    {
        m_Color = color;
    }
} // namespace UN
