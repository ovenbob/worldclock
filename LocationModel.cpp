
#include "LocationModel.h"

#include <QFont>
#include <QPalette>
#include <QApplication>
#include <QMessageBox>
#include <QIcon>
#include <QColorDialog>
#include <QtDebug>

#include <algorithm>

#include <assert.h>


namespace
{
    const QString sColumnNames[LocationModel::eColumnCount] = 
    {
        "Location", "Offset", "Colour", "Start", "End", "Clear Period"
    };

    class OtherZone
    {
    public:
        OtherZone (const QByteArray& zone);
        ~OtherZone () { cancelZone(); }

        void cancelZone ();

    private:
        void setZone (const QByteArray& zone);

        QByteArray mOldZone;
        bool mCancelZone;
    };

    OtherZone::OtherZone (const QByteArray& zone) : mCancelZone(true)
    {
        const char* envTZ = getenv("TZ");
        if(envTZ != NULL)
            mOldZone = envTZ;
        setZone(zone);
    }

    void OtherZone::cancelZone ()
    {
        if(mCancelZone)
        {
            mCancelZone = false;
            setZone(mOldZone);
        }
    }

    void OtherZone::setZone (const QByteArray& zone)
    {
        if(zone.isEmpty())
            unsetenv("TZ");
        else
            setenv("TZ", zone.constData(), 1);
    }
}


LocationModel::LocationModel (QObject* parent)
    : QAbstractTableModel(parent), mAllowEdits(false)
{
}

LocationModel::LocationModel (const LocationModel* src, QObject* parent)
    : QAbstractTableModel(parent), mAllowEdits(false)
{
    const LocList& locs = src->locations();
    mLocations.assign(locs.begin(), locs.end());
}

int LocationModel::rowCount (const QModelIndex& parent) const
{
    if(parent.isValid())
        return 0;
    return mLocations.size() + (mAllowEdits ? 1 : 0);
}

int LocationModel::columnCount (const QModelIndex& parent) const
{
    if(parent.isValid())
        return 0;
    return eColumnCount;
}

QVariant LocationModel::data (const QModelIndex& index, int role) const
{
    if(!index.isValid() || (index.column() >= eColumnCount))
        return QVariant();

    size_t row = static_cast<size_t>(index.row());
    if(row > mLocations.size())
    {
        return QVariant();
    }
    else if(row == mLocations.size())
    {
        if(index.column() == eName)
        {
            switch(role)
            {
            case Qt::DisplayRole :
                return "add location";
            case Qt::FontRole :
            {
                QFont f = QApplication::font();
                f.setItalic(true);
                f.setPointSize(f.pointSize() - 2);
                return f;
            }
            break;
            case Qt::ForegroundRole :
                return QApplication::palette().color(
                    QPalette::Disabled, QPalette::Text);
            }
        }
        return QVariant();
    }

    const Location& loc = mLocations[row];
    switch(role)
    {
    case Qt::EditRole :
        switch(index.column())
        {
        case eColour :
            return loc.mColour;
        }
        // fall through
    case Qt::DisplayRole :
        switch(index.column())
        {
        case eName :
            return loc.mName;
        case eZone :
            return loc.mZone;
        case eStart :
            return loc.mStart;
        case eEnd :
            return loc.mEnd;
        default :
            break;
        }
        break;
    case Qt::BackgroundRole :
        if(index.column() == eColour)
            return loc.mColour;
        break;
    case Qt::DecorationRole :
        if(index.column() == eClearPeriod)
            if(loc.mStart.isValid() || loc.mEnd.isValid())
                return QIcon(":icons/cross.png");
        break;
    }

    return QVariant();
}

bool LocationModel::setData (const QModelIndex& index, const QVariant& value,
                             int role)
{
    if(!index.isValid()
       || (index.column() >= eColumnCount) || (role != Qt::EditRole))
        return false;

    size_t row = static_cast<size_t>(index.row());
    if(row > mLocations.size())
    {
        return false;
    }
    else if(row == mLocations.size())
    {
        if(index.column() == eName)
        {
            QString newName = value.toString();
            if(existingName(newName, -1))
                return false;
            Location l;
            l.mName = newName;
            addLocation(l);
            return true;
        }
    }

    Location& loc = mLocations[row];
    switch(index.column())
    {
    case eName :
        {
        QString newName = value.toString();
        if(newName.isEmpty())
        {
            if(QMessageBox::question(NULL, "Delete Location",
                 QString("Are you sure you want to delete %1?").arg(loc.mName),
                 QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes)
            {
                beginRemoveRows(index.parent(), row, row);
                mLocations.erase(mLocations.begin() + row);
                endRemoveRows();
                return true;
            }
        }
        if(existingName(newName, index.row()))
            return false;
        loc.mName = value.toString();
        }
        break;
    case eZone :
        //+ select zone from system db?
        loc.mZone = value.toString();
        break;
    case eColour :
        //+ want colour editing
        loc.mColour = value.value<QColor>();
        break;
    case eStart :
        loc.mStart = value.toTime();
        break;
    case eEnd :
        loc.mEnd = value.toTime();
        break;
    }
    emit dataChanged(index, index);
    return true;
}

Qt::ItemFlags LocationModel::flags (const QModelIndex& index) const
{
    bool canEdit = mAllowEdits;
    if(canEdit)
    {
        switch(index.column())
        {
        case eColour :
        case eClearPeriod :
            canEdit = false;
        }
        if(static_cast<size_t>(index.row()) == mLocations.size())
            canEdit = canEdit && (index.column() == eName);
    }
    return QAbstractTableModel::flags(index)
        | (canEdit ? Qt::ItemIsEditable : Qt::NoItemFlags);
}

QVariant LocationModel::headerData (int section, Qt::Orientation orientation,
                                    int role) const
{
    if((section >= eColumnCount) || (orientation != Qt::Horizontal))
        return QVariant();

    if(role == Qt::DisplayRole)
        return sColumnNames[section];

    return QVariant();
}

void LocationModel::addLocation (Location loc)
{
    size_t pos = mLocations.size();
    beginInsertRows(QModelIndex(), pos, pos);
    mLocations.push_back(loc);
    endInsertRows();
}

void LocationModel::modelDoubleClick (const QModelIndex& idx, QWidget* view)
{
    switch(idx.column())
    {
    case eColour :
        {
        int row = idx.row();
        Location& l = mLocations[row];
        QColor newColour =
            QColorDialog::getColor(l.mColour, view, "Location Colour");
        if(newColour.isValid())
        {
            l.mColour = newColour;
            emit dataChanged(idx, idx);
        }
        }
        break;
    case eClearPeriod :
        {
        int row = idx.row();
        Location& l = mLocations[row];
        l.mStart = QTime();
        l.mEnd = QTime();
        emit dataChanged(index(row, eStart), index(row, eEnd));
        }
        break;
    }
}

QColor LocationModel::colour (int row) const
{
    return mLocations[row].mColour;
}

QTime LocationModel::toZone (int row, const QDateTime& utc) const
{
    assert(utc.timeSpec() == Qt::UTC);
    OtherZone oz(mLocations[row].mZone.toAscii());
    return utc.toLocalTime().time();
}

QTime LocationModel::start (int row) const
{
    if(!mLocations[row].mStart.isValid())
        return mLocations[row].mStart;

    OtherZone oz(mLocations[row].mZone.toAscii());
    QDateTime s = QDateTime::currentDateTime();
    s.setTime(mLocations[row].mStart);
    QDateTime su = s.toUTC();
    oz.cancelZone();
    return su.toLocalTime().time();
}

QTime LocationModel::end (int row) const
{
    if(!mLocations[row].mEnd.isValid())
        return mLocations[row].mEnd;

    OtherZone oz(mLocations[row].mZone.toAscii());
    QDateTime s = QDateTime::currentDateTime();
    s.setTime(mLocations[row].mEnd);
    QDateTime su = s.toUTC();
    oz.cancelZone();
    return su.toLocalTime().time();
}

bool LocationModel::existingName (const QString& name, int excludeRow) const
{
    for(int l = 0, lmax = mLocations.size(); l < lmax; ++l)
    {
        if((l != excludeRow) && (name == mLocations[l].mName))
            return true;
    }
    return false;
}

