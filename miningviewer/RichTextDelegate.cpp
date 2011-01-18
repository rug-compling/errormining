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

    QStyleOptionViewItemV4 opt = option;
    initStyleOption(&opt, index);

    //painter->save();

    /*QTextDocument doc;
    QTextOption textOption = doc.defaultTextOption();
    textOption.setWrapMode(QTextOption::NoWrap);
    doc.setDefaultTextOption(textOption);
    doc.setDocumentMargin(0);
    doc.setHtml(opt.text);*/

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
    d_label->setFixedSize(qMax(0, labelRect.width()), labelRect.height());
    d_label->setPalette(palette);
    d_label->setText(opt.text);

    QPixmap pixmap(d_label->size());

    QString cacheKey = QString("LABEL:%1.%2.%3x%4").arg(selected).arg(opt.text).arg(labelRect.width()).arg(labelRect.height());
    if (!QPixmapCache::find(cacheKey, &pixmap)) {
        d_label->render(&pixmap);
        QPixmapCache::insert(cacheKey, pixmap);
    }

    painter->drawPixmap(labelRect, pixmap);

    //opt.text = "";
    //opt.widget->style()->drawControl(QStyle::CE_ItemViewItem, &opt, painter);


    //painter->translate(opt.rect.left(), opt.rect.top());
    //QRect clip(0, 0, opt.rect.width(), opt.rect.height());
    //doc.drawContents(painter, clip);

    //painter->restore();
}

QSize RichTextDelegate::sizeHint(QStyleOptionViewItem const &option,
                                 QModelIndex const &index) const
{
    if (index.column() != d_column)
        QStyledItemDelegate::sizeHint(option, index);

    QStyleOptionViewItemV4 opt = option;
    initStyleOption(&opt, index);

    QTextDocument doc;
    doc.setDefaultFont(option.font);
    doc.setHtml(opt.text);

    return QSize(doc.idealWidth(), option.fontMetrics.height());
}
