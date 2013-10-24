
#include "MainWindow.h"

#include "Clock24.h"
#include "LocationModel.h"
#include "Preferences.h"
#include "HereDlog.h"

#include <QSettings>
#include <QCloseEvent>
#include <QFontDialog>
#include <QMenu>
#include <QtDebug>

#include <memory>


MainWindow::MainWindow (Settings* factory, QWidget* parent)
    : QMainWindow(parent), mSettingsFactory(factory), mLocations(NULL)
{
    setObjectName("mainWindow");
    mClock = new Clock24(this);

    setCentralWidget(mClock);

    setLocations(new LocationModel(this));

    loadSettings();
}

void MainWindow::closeEvent (QCloseEvent* event)
{
    saveSettings();
    event->accept();
}

void MainWindow::contextMenuEvent (QContextMenuEvent* e)
{
    QMenu m;
    m.addAction("Font...", this, SLOT(editFont()));
    m.addAction("Here...", this, SLOT(editHere()));
    m.addAction("Locations...", this, SLOT(editPrefs()));
    m.exec(e->globalPos());
}

void MainWindow::loadSettings ()
{
    std::auto_ptr<QSettings> s(mSettingsFactory->create());
    s->beginGroup(objectName());
    restoreGeometry(s->value("geometry").toByteArray());
    s->endGroup();

    s->beginGroup("font");
    bool custom = s->value("custom", false).toBool();
    if(custom)
    {
        QFont f;
        f.fromString(s->value("descr").toString());
        mClock->customiseFont(f);
    }
    s->endGroup();

    size_t imax = s->beginReadArray("Locations");
    for(size_t i = 0; i < imax; ++i)
    {
        LocationModel::Location l;
        s->setArrayIndex(i);
        l.mName = s->value("name").toString();
        l.mZone = s->value("zone", 0).toString();
        l.mColour = s->value("colour", Qt::yellow).value<QColor>();
        l.mStart = s->value("start", QTime()).toTime();
        l.mEnd = s->value("end", QTime()).toTime();
        if(!l.mStart.isValid() || !l.mEnd.isValid())
        {
            l.mStart = QTime();
            l.mEnd = QTime();
        }
        mLocations->addLocation(l);
    }
    s->endArray();
}

void MainWindow::saveSettings ()
{
    std::auto_ptr<QSettings> s(mSettingsFactory->create());
    s->beginGroup(objectName());
    s->setValue("geometry", saveGeometry());
    s->endGroup();

    s->beginGroup("font");
    bool saveFont = mClock->fontSet();
    s->setValue("custom", saveFont);
    if(saveFont)
        s->setValue("descr", mClock->font().toString());
    s->endGroup();

    const LocationModel::LocList& locs = mLocations->locations();
    s->beginWriteArray("Locations");
    for(size_t i = 0, imax = locs.size(); i < imax; ++i)
    {
        const LocationModel::Location& l = locs[i];
        s->setArrayIndex(i);
        s->setValue("name", l.mName);
        s->setValue("zone", l.mZone);
        s->setValue("colour", l.mColour);
        s->setValue("start", l.mStart);
        s->setValue("end", l.mEnd);
    }
    s->endArray();
}

void MainWindow::editPrefs ()
{
    LocationModel* newLocs = new LocationModel(mLocations, this);
    newLocs->allowEdits(true);
    Preferences d(newLocs);
    if(d.exec() == QDialog::Accepted)
    {
        newLocs->allowEdits(false);
        setLocations(newLocs);
    }
    else
    {
        delete newLocs;
    }
}

void MainWindow::editFont ()
{
    bool ok;
    QFont f = QFontDialog::getFont(&ok, mClock->font(), this, "Numeral Font");
    if(ok)
        mClock->customiseFont(f);
}

void MainWindow::editHere ()
{
    HereDlog d;
    if(d.exec() == QDialog::Accepted)
    {
        QByteArray tz = d.timeZone();
        if(tz.isEmpty())
            unsetenv("TZ");
        else
            setenv("TZ", tz.constData(), 1);
    }
}

void MainWindow::setLocations (LocationModel* locs)
{
    LocationModel* oldModel = mLocations;
    mLocations = locs;
    mClock->setModel(locs);
    delete oldModel;
}
