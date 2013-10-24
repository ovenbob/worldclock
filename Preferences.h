#ifndef Preferences_H
#define Preferences_H

#include <QDialog>


class LocationModel;

class QModelIndex;


class Preferences : public QDialog
{
    Q_OBJECT

public:
    Preferences (LocationModel* model, QWidget* parent = NULL);

public slots:
    void modelDoubleClick(const QModelIndex&);

signals:
    void modelDoubleClick(const QModelIndex&, QWidget*);

private:
    LocationModel* mModel;
};

#endif


