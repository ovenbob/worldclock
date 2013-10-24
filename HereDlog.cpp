
#include "HereDlog.h"

#include <QBoxLayout>
#include <QDialogButtonBox>
#include <QLabel>


HereDlog::HereDlog (QWidget* parent)
    : QDialog(parent)
{
    QVBoxLayout* vl = new QVBoxLayout(this);
    vl->setContentsMargins(0, 0, 0, 0);

    QLabel* l = new QLabel(
        "Time zone to view from, leave blank to use system time zone", this);
    vl->addWidget(l);

    mCurrTz = new ZoneSelector(this);
    mCurrTz->setEditText(getenv("TZ"));
    vl->addWidget(mCurrTz);

    QDialogButtonBox* b = new QDialogButtonBox(
        QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Horizontal, this);
    vl->addWidget(b);

    connect(b, SIGNAL(accepted()), this, SLOT(accept()));
    connect(b, SIGNAL(rejected()), this, SLOT(reject()));
}
