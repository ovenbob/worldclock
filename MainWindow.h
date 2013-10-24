
#ifndef MainWindow_H
#define MainWindow_H

#include <QMainWindow>


class Clock24;
class LocationModel;

class QSettings;


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    class Settings
    {
    public:
        virtual QSettings* create() = 0;
    };

    MainWindow (Settings* factory, QWidget* parent = NULL);

    void setLocations (LocationModel* locs);

public slots:
    void editPrefs ();
    void editFont ();
    void editHere ();

protected:
    virtual void closeEvent (QCloseEvent* event);
    virtual void contextMenuEvent (QContextMenuEvent* e);

    void loadSettings ();
    void saveSettings ();

private:
    Clock24* mClock;

    Settings* mSettingsFactory;

    LocationModel* mLocations;
};

#endif
