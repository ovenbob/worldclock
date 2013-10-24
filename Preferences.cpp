#include "Preferences.h"

#include "LocationModel.h"
#include "ZoneSelectorDelg.h"

#include <QTableView>
#include <QBoxLayout>
#include <QLabel>
#include <QDialogButtonBox>
#include <QCheckBox>
#include <QtDebug>


Preferences::Preferences (LocationModel* model, QWidget* parent)
    : QDialog(parent), mModel(model)
{
    QVBoxLayout* vl = new QVBoxLayout(this);
    vl->setContentsMargins(0, 0, 0, 0);
    QTableView* locs = new QTableView(this);
    vl->addWidget(locs);

    locs->setItemDelegateForColumn(LocationModel::eZone,
                                   new ZoneSelectorDelg(this));

    QDialogButtonBox* b = new QDialogButtonBox(
        QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Horizontal, this);
    vl->addWidget(b);

    connect(b, SIGNAL(accepted()), this, SLOT(accept()));
    connect(b, SIGNAL(rejected()), this, SLOT(reject()));
    locs->setModel(model);
    connect(locs, SIGNAL(doubleClicked(const QModelIndex&)),
            this, SLOT(modelDoubleClick(const QModelIndex&)));
    connect(this, SIGNAL(modelDoubleClick(const QModelIndex&, QWidget*)),
            model, SLOT(modelDoubleClick(const QModelIndex&, QWidget*)));
}

void Preferences::modelDoubleClick(const QModelIndex& idx)
{
    emit modelDoubleClick(idx, this);
}
