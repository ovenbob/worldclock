#ifndef HereDlog_H
#define HereDlog_H

#include <QDialog>

#include "ZoneSelector.h"


class HereDlog : public QDialog
{
    Q_OBJECT

public:
    HereDlog (QWidget* parent = 0);

    QByteArray timeZone () const { return mCurrTz->currentText().toAscii(); }

private:
    ZoneSelector* mCurrTz;
};

#endif
