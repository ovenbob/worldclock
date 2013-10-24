#ifndef LocationModel_H
#define LocationModel_H

#include <QAbstractTableModel>

#include <QColor>
#include <QTime>

#include <vector>


class LocationModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    LocationModel (QObject* parent = NULL);
    LocationModel (const LocationModel* src, QObject* parent = NULL);

    enum { eName, eZone, eColour, eStart, eEnd, eClearPeriod, eColumnCount };

    // Qt model interface
    virtual int rowCount (const QModelIndex& parent = QModelIndex()) const;
    virtual int columnCount (const QModelIndex& parent = QModelIndex()) const;
    virtual QVariant data (const QModelIndex& index,
                           int role = Qt::DisplayRole) const;
    virtual QVariant headerData (int section, Qt::Orientation orientation,
                                 int role = Qt::DisplayRole) const;
    bool setData (const QModelIndex& index, const QVariant& value,
                  int role = Qt::EditRole);
    Qt::ItemFlags flags (const QModelIndex& index) const;

    // Simple interface
    QColor colour (int row) const;
    QTime toZone (int row, const QDateTime& utc) const;
    QTime start (int row) const;
    QTime end (int row) const;

    // Editable interface
    struct Location
    {
        QString mName;
        QString mZone;
        QColor mColour;
        QTime mStart, mEnd;
    };
    typedef std::vector<Location> LocList;

    void allowEdits (bool on) { mAllowEdits = on; }
    void addLocation (Location loc);
    const LocList& locations () const { return mLocations; }

private slots:
    void modelDoubleClick (const QModelIndex& index, QWidget* view);

private:
    bool existingName (const QString& name, int excludeRow) const;

    LocList mLocations;
    bool mAllowEdits;
};

#endif
