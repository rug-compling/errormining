#ifndef RICHTEXTDELEGATE_HH
#define RICHTEXTDELEGATE_HH

#include <QModelIndex>
#include <QPainter>
#include <QStyleOptionViewItem>
#include <QStyledItemDelegate>

class QLabel;
class QObject;

class RichTextDelegate : public QStyledItemDelegate {
public:
    RichTextDelegate(int column, QObject *parent = 0);
protected:
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const;
private:
    int d_column;
    QLabel *d_label;
};

#endif // RICHTEXTDELEGATE_HH
