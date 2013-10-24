
#include "SwatchCache.h"

#include <QPixmap>
#include <QBuffer>


const QString& SwatchCache::swatch (const QColor& colour)
{
    std::pair<ColourMap::iterator, bool> ins = mSwatches.insert(
        std::make_pair(colour.rgb(), QString()));
    if(ins.second)
    {
        QPixmap swatch(16, 16);
        swatch.fill(colour);
        QByteArray ba;
        QBuffer buffer(&ba);
        buffer.open(QIODevice::WriteOnly);
        swatch.save(&buffer, "PNG");

        ins.first->second = QString(buffer.data().toBase64());
    }
    return ins.first->second;
}


