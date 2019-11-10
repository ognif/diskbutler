#include <QtGui>

#include "xbelgenerator.h"

XbelGenerator::XbelGenerator(CommonTreeWidget *treeWidget)
  : treeWidget(treeWidget)
{
}

bool XbelGenerator::write(QIODevice *device, bool isIndexFile)
{
  out.setDevice(device);
  out.setCodec("UTF-8");
  QDate date = QDate::currentDate();
  QString info("<xbel name=\"project\" version=\"1.0\" date=\"%1\" size=\"%2\" item_count=\"%3\">\n");
  info = info
      .arg(date.currentDate().toString())
      .arg(treeWidget->GetDataSize())
      .arg(treeWidget->GetDataItemCount());
  out << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
      << "<!DOCTYPE xbel>\n"
      << info
      << indent(1) << "<settings/>\n";

  //for (int i = 0; i < treeWidget->topLevelItemCount(); ++i)
  //  generateItem((QDataItem*)treeWidget->topLevelItem(i), 1);
  if (1 == treeWidget->topLevelItemCount()) {
    QTreeWidgetItem *root = treeWidget->topLevelItem(0);
    generateItem((QDataItem*)root, 1, isIndexFile); //save disk setting
    if (root->childCount() > 0) {
      root = root->child(0); // get "session"
      for (int i = 0; i < root->childCount(); i++) {
        generateItem((QDataItem*)root->child(i), 1, isIndexFile);
      }
    }
  }

  out << "</xbel>\n";
  return true;
}

QString XbelGenerator::indent(int depth)
{
  const int IndentSize = 2;
  return QString(IndentSize * depth, ' ');
}

QString XbelGenerator::escapedText(const QString &str)
{
  QString result = str;
  result.replace("&", "&amp;");
  result.replace("<", "&lt;");
  result.replace(">", "&gt;");
  return result;
}

QString XbelGenerator::escapedAttribute(const QString &str)
{
  QString result = escapedText(str);
  result.replace("\"", "&quot;");
  result.prepend("\"");
  result.append("\"");
  return result;
}

void XbelGenerator::generateItem(QDataItem *item, int depth, bool isIndexFile)
{
  if (isIndexFile) {
    out << item->ToXMLElementIndexFile(depth);
  } else {
    out << item->ToXMLElement(depth);
  }
}
