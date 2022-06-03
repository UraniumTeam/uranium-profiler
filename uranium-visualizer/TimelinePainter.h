#pragma once
#include <QPainter>
#include <utility>

namespace UN
{
    struct TimelineMeasure
    {
        std::string Name;
        int64_t TickCount;

        inline TimelineMeasure(std::string name, int64_t tickCount)
            : Name(std::move(name))
            , TickCount(tickCount)
        {
        }

        [[nodiscard]] inline double lengthInPixels(double pixelsPerTick) const
        {
            return (double)TickCount * pixelsPerTick;
        }
    };

    class TimelinePainter
    {
        QPainter* m_Painter;
        QRect m_Rect{};
        int m_Width = 30;
        int64_t m_StartTick    = 0;
        double m_PixelsPerTick = 1;
        double m_NanosecondsPerTick = 1;

        QColor m_Color = Qt::white;
        int m_LineWidth         = 2;
        int64_t m_MinPixelWidth = 80;

        std::vector<TimelineMeasure> m_Measures;

    public:
        explicit TimelinePainter(QPainter& painter, double nanosecondsPerTick);

        void setRect(const QRect& rect);
        void setRegion(int64_t startTick, double pixelsPerTick);
        void setWidth(int width);
        void setLineWidth(int lineWidth);
        void setColor(const QColor& color);

        void draw();
    };
}
