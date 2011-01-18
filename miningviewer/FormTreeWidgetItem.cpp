#include <QTreeWidgetItem>

#include "FormTreeWidgetItem.hh"

using namespace miningviewer;

bool FormTreeWidgetItem::operator<(QTreeWidgetItem const &other) const
{
	int sortColumn = treeWidget()->sortColumn();
	double number = text(sortColumn).toDouble();
	double otherNumber = other.text(sortColumn).toDouble();
	return number < otherNumber;
}

