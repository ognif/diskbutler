/*
 *  DiskButler - a powerful CD/DVD/BD recording software tool for Linux, macOS and Windows.
 *  Copyright (c) 2021 Ingo Foerster (pixbytesl@gmail.com).
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License 3 as published by
 *  the Free Software Foundation; either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License 3 for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include <QtGui>
#include <QTextCodec>
#if QT_VERSION > QT_VERSION_CHECK(5, 16, 0)
#include <QStringConverter>
#endif

#include "xbelgenerator.h"

XbelGenerator::XbelGenerator(CommonTreeWidget *treeWidget)
  : treeWidget(treeWidget)
{
}

bool XbelGenerator::write(QIODevice *device, bool isIndexFile)
{
  out.setDevice(device);
#if QT_VERSION > QT_VERSION_CHECK(5, 16, 0)
  out.setEncoding(QStringConverter::Utf8);
#else
  out.setCodec("UTF-8");
#endif

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
