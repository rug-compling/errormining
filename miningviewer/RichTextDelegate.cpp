#include <QLabel>
#include <QModelIndex>
#include <QPainter>
#include <QPixmapCache>
#include <QSize>
#include <QStyleOptionViewItem>
#include <QStyledItemDelegate>
#include <QTextDocument>
#include <QTextEdit>

#include "RichTextDelegate.hh"

RichTextDelegate::RichTextDelegate(int column, QObject *parent)
    : QStyledItemDelegate(parent), d_column(column)
{
    d_label = new QLabel;
    d_label->setTextFormat(Qt::RichText);
    d_label->setWordWrap(false);
}

void RichTextDelegate::paint(QPainter *painter,
                             QStyleOptionViewItem const &option,
                             QModelIndex const &index) const
{
    Q_ASSERT(index.isValid());

    if (index.column() != d_column)
        return QStyledItemDelegate::paint(painter, option, index);

    bool selected = option.state & QStyle::State_Selected;
    QPalette palette(option.palette);
    palette.setColor(QPalette::Active,
                     QPalette::Window,
                     selected ? option.palette.highlight().color() :
                     option.palette.base().color());
    palette.setColor(QPalette::Active,
                     QPalette::WindowText,
                     selected ? option.palette.highlightedText().color()
                         : option.palette.text().color());

    QRect labelRect(option.rect);
    d_label->setPalette(palette);
    d_label->setFont(option.font);
    d_label->setFixedSize(qMax(0, labelRect.width()), labelRect.height());

    QString text = index.model()->data(index).toString();
    d_label->setText(text);

    QPixmap pixmap(d_label->size());

    QString cacheKey = QString("LABEL:%1.%2.%3x%4").arg(selected).arg(text).arg(labelRect.width()).arg(labelRect.height());
    if (!QPixmapCache::find(cacheKey, &pixmap)) {
        d_label->render(&pixmap);
        QPixmapCache::insert(cacheKey, pixmap);
    }

    painter->drawPixmap(labelRect, pixmap);
}

QSize RichTextDelegate::sizeHint(QStyleOptionViewItem const &option,
                                 QModelIndex const &index) const
{
    if (index.column() != d_column)
        QStyledItemDelegate::sizeHint(option, index);

    QString text = index.model()->data(index).toString();

    d_document.setDefaultFont(option.font);
    d_document.setHtml(text);

    return QSize(d_document.idealWidth(), option.fontMetrics.height());
}
