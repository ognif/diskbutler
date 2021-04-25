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

//#include <QtGui>
#include <QtWidgets>

#include "xbelhandler.h"
#include "qaudiotrackitem.h"
#include "qdiskitem.h"

XbelHandler::XbelHandler(CommonTreeWidget *treeWidget)
  : treeWidget(treeWidget)
{
  item = (QDataItem *)treeWidget->topLevelItem(0);
  //item = createChildItem("Session");
  //item->SetType(QDataItem::Session);
  metXbelTag = false;

  QStyle *style = treeWidget->style();
  folderIcon.addPixmap(style->standardPixmap(QStyle::SP_DirClosedIcon),
                       QIcon::Normal, QIcon::Off);
  folderIcon.addPixmap(style->standardPixmap(QStyle::SP_DirOpenIcon),
                       QIcon::Normal, QIcon::On);
}

bool XbelHandler::startElement(const QString & /* namespaceURI */,
                               const QString & /* localName */,
                               const QString &qName,
                               const QXmlAttributes &attributes)
{
  if (!metXbelTag && qName != "xbel") {
    errorStr = QObject::tr("The file is not an XBEL file.");
    return false;
  }

  if (qName == "xbel") {
    QString version = attributes.value("version");
    if (!version.isEmpty() && version != "1.0") {
      errorStr = QObject::tr("The file is not an XBEL version 1.0 file.");
      return false;
    }
    // Set whole tree
    treeWidget->SetDataSize(attributes.value("size").toInt());
    treeWidget->SetDataItemCount(attributes.value("item_count").toInt());
    // Set "Disk" node
    item->SetDataSize(attributes.value("size").toInt());
    item->SetDataItemCount(attributes.value("item_count").toInt());
    // Set "Session" node
    item = (QDataItem *)(item->child(0));
    item->SetDataSize(attributes.value("size").toInt());
    item->SetDataItemCount(attributes.value("item_count").toInt());
    metXbelTag = true;
  } else if (qName == "folder" || qName == "file"
             || qName == "datatrack" || qName == "audiotrack") {
    item = createChildItem(qName);
    item->FromXMLElement(qName, attributes);
    if (qName == "folder") {
      item->setIcon(0, folderIcon);
    } else if (qName == "datatrack") {
      treeWidget->SetDataTrackItem(item);
    }
  } else if (qName == "disk") {
    QDiskItem *diskItem = (QDiskItem *)treeWidget->topLevelItem(0);
    diskItem->FromXMLElement(qName, attributes); // read disk setting disk
  } else if (qName == "Options") {
      QDiskItem *diskItem = (QDiskItem *)treeWidget->topLevelItem(0);
      diskItem->FromXMLElement(qName, attributes); // read disk setting Options
  }else if (qName == "FileSystem") {
      QDiskItem *diskItem = (QDiskItem *)treeWidget->topLevelItem(0);
      diskItem->FromXMLElement(qName, attributes); // read disk setting FileSystem
  }else if (qName == "IsoExtension") {
      QDiskItem *diskItem = (QDiskItem *)treeWidget->topLevelItem(0);
      diskItem->FromXMLElement(qName, attributes); // read disk setting ISOEx
  }else if (qName == "Boot") {
      QDiskItem *diskItem = (QDiskItem *)treeWidget->topLevelItem(0);
      diskItem->FromXMLElement(qName, attributes); // read disk setting Boot
  }else if (qName == "DiskCDText") {
      QDiskItem *diskItem = (QDiskItem *)treeWidget->topLevelItem(0);
      diskItem->FromXMLElement(qName, attributes); // read disk setting CD Text
  }

  currentText.clear();
  return true;
}

bool XbelHandler::endElement(const QString & /* namespaceURI */,
                             const QString & /* localName */,
                             const QString &qName)
{
  if (qName == "title") {
    if (item)
      item->setText(0, currentText);
  } else if (qName == "folder" || qName == "file" || qName == "datatrack" || qName == "audiotrack") {
    item = (QDataItem *)item->parent();
  }
  return true;
}

bool XbelHandler::characters(const QString &str)
{
  currentText += str;
  return true;
}

bool XbelHandler::fatalError(const QXmlParseException &exception)
{
  QMessageBox::information(treeWidget->window(), QObject::tr("ZQtMdiTreeWidget"),
                           QObject::tr("Parse error at line %1, column %2:\n"
                                       "%3")
                           .arg(exception.lineNumber())
                           .arg(exception.columnNumber())
                           .arg(exception.message()));
  return false;
}

QString XbelHandler::errorString() const
{
  return errorStr;
}

QDataItem *XbelHandler::createChildItem(const QString &name)
{
  QDataItem *childItem = 0;
  if (item) {
    if (name == "audiotrack") {
      childItem = new QAudioTrackItem(item);
    } else {
      childItem = new QDataItem(item);
    }
  } else {
    childItem = new QDataItem(treeWidget);
  }
  //childItem->setData(0, Qt::UserRole, name);
  childItem->setText(0, name);
  return childItem;
}
