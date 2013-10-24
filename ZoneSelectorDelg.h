#ifndef ZoneSelectorDelg_H
#define ZoneSelectorDelg_H

#include <QStyledItemDelegate>


class ZoneSelectorDelg : public QStyledItemDelegate
{
    Q_OBJECT

public:
    ZoneSelectorDelg (QWidget* parent = 0);

    QWidget *createEditor (QWidget *parent, const QStyleOptionViewItem &option,
                           const QModelIndex &index) const;
    void setEditorData (QWidget *editor, const QModelIndex &index) const;
    void setModelData (QWidget *editor, QAbstractItemModel *model,
                       const QModelIndex &index) const;
};

#endif

