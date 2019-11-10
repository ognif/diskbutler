#ifndef XBELGENERATOR_H
#define XBELGENERATOR_H

#include <QTextStream>
#include "CommonTreeWidget.h"
#include "QDataItem.h"

QT_BEGIN_NAMESPACE
class QTreeWidget;
class QTreeWidgetItem;
QT_END_NAMESPACE

class XbelGenerator
{
public:
  XbelGenerator(CommonTreeWidget *treeWidget);

  bool write(QIODevice *device, bool isIndexFile = false);

private:
  static QString indent(int indentLevel);
  static QString escapedText(const QString &str);
  static QString escapedAttribute(const QString &str);
  void generateItem(QDataItem *item, int depth, bool isIndexFile = false);

  CommonTreeWidget *treeWidget;
  QTextStream out;
};

#endif
