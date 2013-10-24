
#include "ZoneSelectorDelg.h"

#include "ZoneSelector.h"


ZoneSelectorDelg::ZoneSelectorDelg (QWidget* parent)
    : QStyledItemDelegate(parent)
{
}

QWidget *ZoneSelectorDelg::createEditor (QWidget *parent,
                                         const QStyleOptionViewItem &option,
                                         const QModelIndex &index) const
{
    if(index.data().canConvert<QString>())
    {
        ZoneSelector* ze = new ZoneSelector(parent);
        return ze;
    }
    else
    {
        return QStyledItemDelegate::createEditor(parent, option, index);
    }
}

void ZoneSelectorDelg::setEditorData (QWidget *editor,
                                      const QModelIndex &index) const
{
    ZoneSelector* zs = qobject_cast<ZoneSelector*>(editor);
    zs->setEditText(index.data().toString());
}

void ZoneSelectorDelg::setModelData (QWidget *editor, QAbstractItemModel *model,
                                     const QModelIndex &index) const
{
    ZoneSelector* zs = qobject_cast<ZoneSelector*>(editor);
    model->setData(index, zs->currentText());
}

