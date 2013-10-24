
#include "Clock24.h"

#include "LocationModel.h"

#include <QPainter>
#include <QTime>
#include <QMouseEvent>
#include <QToolTip>
#include <QTextStream>
#include <QtDebug>

#include <map>

#include <math.h>


namespace
{
    QTime midnight(0, 0, 0);
    const qreal secondsInDay = 24 * 3600;

    QRectF centreOn (const QRectF& r, const QPointF& p)
    {
        return QRectF(p.x() - r.width() / 2.0, p.y() - r.height() / 2.0,
                      r.width(), r.height());
    }

    QPointF clockOffset (qreal radius, qreal angle)
    {
        return QPointF(- sin(angle) * radius, cos(angle) * radius);
    }

    QTime clockTime (const QPointF& offset)
    {
        qreal radians = atan2(offset.y(), offset.x());
        return QTime().addSecs(
            (radians * (12.0 * 3600.0) / M_PI) - (6.0 * 3600.0));
    }

    int pieAngle (const QTime& time)
    {
        int timeQ = (midnight.secsTo(time) * 360.0 * 16.0) / secondsInDay;
        return - timeQ - 90 * 16;
    }

    int duration (const QTime& start, const QTime& end)
    {
        int secs = start.secsTo(end);
        if(secs < 0)
            secs = secondsInDay + secs;
        return secs;
    }

    void clockMetrics (const QRectF& bounds, Clock24::ClockMetrics& cm)
    {
        cm.mClockBorder = 3;
        cm.mClockBounds = bounds.adjusted(cm.mClockBorder, cm.mClockBorder,
                                          -cm.mClockBorder, -cm.mClockBorder);
        cm.mDiameter = cm.mClockBounds.width();
        cm.mRadius = cm.mDiameter / 2.0;
        cm.mInnerRadius = 0;
        cm.mCentre = QPointF(cm.mClockBounds.left() + cm.mRadius,
                             cm.mClockBounds.top() + cm.mRadius);
    }
}


Clock24::Clock24 (QWidget* parent)
    : QWidget(parent), mLocations(NULL), mFontSet(false)
{
    connect(&mTickTimer, SIGNAL(timeout()), this, SLOT(update()));
    mTickTimer.setInterval(1000);
    mTickTimer.start();

    setBackgroundRole(QPalette::Base);
    setForegroundRole(QPalette::Text);
    setMouseTracking(true);
}

void Clock24::paintEvent (QPaintEvent*)
{
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);

    QRect r = rect();
    p.fillRect(r, palette().brush(backgroundRole()));
    qreal clockDiam = std::min(r.width(), r.height());
    drawClock(p, QRectF(r.width() - clockDiam, 0, clockDiam, clockDiam));
}

void Clock24::drawClock (QPainter& p, const QRectF& bounds)
{
    clockMetrics(bounds, mMetrics);

    const QPalette& pal = palette();

    typedef std::multimap<QTime, int> TimeIndex;
    int outerSize = 3;
    if(mLocations != NULL)
    {
        TimeIndex rowTimes;
        QDateTime nowGMT = QDateTime::currentDateTimeUtc();
        for(int m = 0, mmax = mLocations->rowCount(); m < mmax; ++m)
            rowTimes.insert(std::make_pair(mLocations->toZone(m, nowGMT), m));

        int lastDepth = 0;
        QTime lastTime;
        for(TimeIndex::iterator l = rowTimes.begin(), lend = rowTimes.end();
            l != lend; ++l)
        {
            if(l->first != lastTime)
            {
                outerSize = std::max(outerSize, lastDepth + 1);
                lastDepth = 0;
                lastTime = l->first;
            }
            drawMarker(p, mMetrics.mCentre,
                       mMetrics.mRadius * (1.0 - (lastDepth * 0.05)),
                       l->first, mLocations->colour(l->second));
            ++lastDepth;
        }
        outerSize = std::max(outerSize, lastDepth + 1);
    }
    p.setPen(pal.color(foregroundRole()));

    drawNumbers(p, mMetrics.mCentre, mMetrics.mRadius);

    p.setPen(Qt::NoPen);
    p.setBrush(palette().alternateBase());
    mMetrics.mInnerRadius = mMetrics.mRadius * (0.98 - (outerSize * 0.05));
    
    p.drawEllipse(mMetrics.mCentre,
                  mMetrics.mInnerRadius, mMetrics.mInnerRadius);

    if(mLocations != NULL)
    {
        TimeIndex rowTimes;
        for(int m = 0, mmax = mLocations->rowCount(); m < mmax; ++m)
        {
            QTime start = mLocations->start(m);
            if(start.isValid())
                rowTimes.insert(std::make_pair(start, m));
        }

        qreal otherRadius = mMetrics.mInnerRadius - (mMetrics.mRadius * 0.03);
        qreal otherStep = mMetrics.mRadius * 0.05;
        for(TimeIndex::iterator l = rowTimes.begin(), lend = rowTimes.end();
            l != lend; ++l)
        {
            QTime end = mLocations->end(l->second);
            if(!end.isValid())
                continue;

            p.setBrush(mLocations->colour(l->second));
            drawPeriod(p, mMetrics.mCentre, otherRadius, l->first, end);
            otherRadius -= otherStep;
        }
    }

    QTime nowLocal = QTime::currentTime();
    drawHand(p, mMetrics.mCentre, mMetrics.mRadius - 3, nowLocal,
             pal.color(QPalette::Disabled, foregroundRole()));
}

void Clock24::drawNumbers (QPainter& p, const QPointF& centre, qreal length)
{
    const int step = 1, maxt = 24;
    QRectF b = p.boundingRect(rect(), Qt::AlignLeft | Qt::AlignTop, "00");
    qreal br = sqrt((b.width() * b.width()) + (b.height() * b.height())) / 2.0;

    for(int t = step; t <= maxt; t += step)
    {
        QString s = QString::number(t);
        QPointF pt = clockOffset(length - br, (t * 2.0 * M_PI) / 24) + centre;
        p.drawText(centreOn(b, pt), Qt::AlignCenter, s);
    }
}

void Clock24::drawHand (QPainter& p, const QPointF& centre, qreal length,
                        const QTime& time, const QColor& colour)
{
    p.setPen(Qt::NoPen);
    p.setBrush(colour);
    qreal diff = midnight.secsTo(time);
    QPointF nowOff = clockOffset(length * 0.9,
                                 (diff * 2 * M_PI) / secondsInDay);
    QPointF widthOff(nowOff.y() / 40, -nowOff.x() / 40);
    QPointF wideOff = nowOff * 0.8;

    QPointF points[4];
    points[0] = centre;
    points[1] = centre + wideOff + widthOff;
    points[2] = centre + nowOff;
    points[3] = centre + wideOff - widthOff;
    p.drawPolygon(points, 4);
}

void Clock24::drawMarker (QPainter& p, const QPointF& centre, qreal length,
                          const QTime& time, const QColor& colour)
{
    p.setPen(Qt::NoPen);
    p.setBrush(colour);
    qreal diff = midnight.secsTo(time);
    QPointF timeOff = clockOffset(length, (diff * 2 * M_PI) / secondsInDay);
    QPointF markDelta = timeOff * -0.1;
    QPointF markTangent(markDelta.y() * 0.6, -markDelta.x() * 0.6);
    QPointF points[3];
    points[0] = centre + timeOff;
    points[1] = centre + timeOff + markDelta + markTangent;
    points[2] = centre + timeOff + markDelta - markTangent;
    p.drawPolygon(points, 3);
}

void Clock24::drawPeriod (QPainter& p, const QPointF& centre, qreal radius,
                          const QTime& start, const QTime& end)
{
    if(!start.isValid() || !end.isValid())
        return;

    QRectF b(centre.x() - radius, centre.y() - radius, 2 * radius, 2 * radius);
    int startQ = pieAngle(start);
    int endQ = pieAngle(end);
    int angle = endQ - startQ;
    if((angle < 0) != (startQ < 0))
    {
        // Keep angle turning the same wise as the start angle
        angle = (360 * 16) - angle;
        if(startQ < 0)
            angle = -angle;
    }
    p.drawPie(b, startQ, angle);
}

void Clock24::setModel (LocationModel* model)
{
    mLocations = model;
    update();
}

void Clock24::mouseMoveEvent(QMouseEvent* e)
{
    if(e->buttons() == Qt::NoButton)
    {
        if((mLocations == NULL) || mMetrics.mClockBounds.isNull())
            return;

        QPointF mouseOffset = e->pos() - mMetrics.mCentre;
        qreal mouseRadius2 = ((mouseOffset.x() * mouseOffset.x()) +
                             (mouseOffset.y() * mouseOffset.y()));
        if(mouseRadius2 > (mMetrics.mRadius * mMetrics.mRadius))
        {
            QToolTip::showText(e->globalPos(), QString(), this);
            return;
        }

        // Hit detection
        QTime mouseTime = clockTime(mouseOffset);
        std::vector<int> tipRows;
        if(mouseRadius2 > (mMetrics.mInnerRadius * mMetrics.mInnerRadius))
        {
            QDateTime nowUTC = QDateTime::currentDateTimeUtc();
            for(int m = 0, mmax = mLocations->rowCount(); m < mmax; ++m)
            {
                if(abs(mouseTime.secsTo(mLocations->toZone(m, nowUTC))) < 420.0)
                    tipRows.push_back(m);
            }
        }
        else
        {
            for(int m = 0, mmax = mLocations->rowCount(); m < mmax; ++m)
            {
                QTime start = mLocations->start(m);
                if(!start.isValid())
                    continue;
                QTime end = mLocations->end(m);
                if(!end.isValid())
                    continue;
                int dur = duration(start, mouseTime);
                if((dur > 0) && (dur < duration(start, end)))
                    tipRows.push_back(m);
            }
        }

        if(tipRows.empty())
        {
            QToolTip::showText(e->globalPos(), QString(), this);
            return;
        }

        // Build HTML
        QString buf;
        QTextStream s(&buf, QIODevice::WriteOnly);
        s << "<table>";
        for(size_t i = 0, imax = tipRows.size(); i < imax; ++i)
        {
            s << "<tr><td><img src=\"data:image/png;base64,"
              << mTipSwatches.swatch(mLocations->colour(tipRows[i]))
              << "\"></td><td>"
              << mLocations->index(tipRows[i],
                                   LocationModel::eName).data().toString()
              << "</td></tr>";
        }
        s << "</table>";
        QToolTip::showText(e->globalPos(), buf, this);
    }
}

void Clock24::customiseFont (const QFont& f)
{
    mFontSet = true;
    setFont(f);
}
