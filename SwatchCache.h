
#ifndef SwatchCache_H
#define SwatchCache_H

#include <QColor>
#include <QString>

#include <map>


class SwatchCache
{
public:
    const QString& swatch (const QColor& colour);

private:
    typedef std::map<QRgb, QString> ColourMap;
    ColourMap mSwatches;
};

#endif
