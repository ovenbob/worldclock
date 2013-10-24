
#include "MainWindow.h"

#include <QApplication>
#include <QSettings>
#include <QtDebug>

#include <iostream>
#include <memory>


class DefaultSettings : public MainWindow::Settings
{
public:
    virtual QSettings* create () { return new QSettings; }
};

class FileSettings : public MainWindow::Settings
{
public:
    FileSettings (const QString& path) : mPath(path) { }
    virtual QSettings* create ()
        { return new QSettings(mPath, QSettings::IniFormat); }

private:
    QString mPath;
};


int main (int argc, char** argv)
{
    QApplication a(argc, argv);
    QApplication::setOrganizationName("BobWare");
    QApplication::setApplicationName("worldclock");

    std::auto_ptr<MainWindow::Settings> s; 
    QStringList args = a.arguments();
    if(args.length() > 1)
    {
        s.reset(new FileSettings(args[1]));
    }
    else
    {
        s.reset(new DefaultSettings);
    }

    MainWindow* win = new MainWindow(s.get());
    a.connect(&a, SIGNAL(lastWindowClosed()), &a, SLOT(quit()));

    win->show();
    return a.exec();
}

