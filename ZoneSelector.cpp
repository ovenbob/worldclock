
#include "ZoneSelector.h"

#include <QStringList>
#include <QDirIterator>
#include <QtDebug>


namespace
{
    QStringList sAvailZones;

    const QStringList& availZones ()
    {
        if(sAvailZones.isEmpty())
        {
            const QString zoneInfoPath("/usr/share/zoneinfo");
            QDirIterator d(zoneInfoPath,
                           QDir::Dirs | QDir::Files | QDir::NoDotAndDotDot,
                           QDirIterator::Subdirectories);
            while(d.hasNext())
            {
                QString f = d.next();
                
                if((f[zoneInfoPath.length() + 1].isUpper()) &&
                   d.fileInfo().isFile())
                    sAvailZones << f.mid(zoneInfoPath.length() + 1);
            }
        }
        return sAvailZones;
    }
}


ZoneSelector::ZoneSelector (QWidget* parent)
    : QComboBox(parent)
{
    setEditable(true);
    addItems(availZones());
}

