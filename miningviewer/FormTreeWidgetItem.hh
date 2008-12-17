#ifndef _FORMTREEWIDGETITEM_HH
#define _FORMTREEWIDGETITEM_HH

#include <QStringList>
#include <QTreeWidget>
#include <QTreeWidgetItem>

namespace miningviewer {

class FormTreeWidgetItem : public QTreeWidgetItem
{
public:
	FormTreeWidgetItem(QTreeWidget *parent) : QTreeWidgetItem(parent) {}
	bool operator<(QTreeWidgetItem const &other) const;
};

}

#endif // _FORMTREEWIDGETITEM_HH

