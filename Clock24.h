
#ifndef Clock24_H
#define Clock24_H

#include "SwatchCache.h"

#include <QWidget>
#include <QTimer>
#include <QTime>


class LocationModel;


class Clock24 : public QWidget
{
    Q_OBJECT

public:
    Clock24 (QWidget* parent = NULL);

    void setModel (LocationModel* model);

    bool fontSet () const { return mFontSet; }
    void customiseFont (const QFont& f);

    struct ClockMetrics
    {
        qreal mClockBorder, mDiameter, mRadius;
        qreal mInnerRadius;
        QPointF mCentre;
        QRectF mClockBounds;
    };

protected:
    virtual void paintEvent(QPaintEvent* e);
    virtual void mouseMoveEvent(QMouseEvent* e);

private:
    void drawClock (QPainter& p, const QRectF& r);
    void drawNumbers (QPainter& p, const QPointF& centre, qreal length);
    void drawHand (QPainter& p, const QPointF& centre, qreal length,
                   const QTime& time, const QColor& colour);
    void drawPeriod (QPainter& p, const QPointF& centre, qreal radius,
                     const QTime& start, const QTime& end);
    void drawMarker (QPainter& p, const QPointF& centre, qreal length,
                     const QTime& time, const QColor& colour);

    QTimer mTickTimer;

    LocationModel* mLocations;
    ClockMetrics mMetrics;
    SwatchCache mTipSwatches;
    bool mFontSet;
};

#endif
