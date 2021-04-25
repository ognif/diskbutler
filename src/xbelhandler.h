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

#ifndef XBELHANDLER_H
#define XBELHANDLER_H

#include <QIcon>
#include <QXmlDefaultHandler>
#include "CommonTreeWidget.h"
#include "qdataitem.h"

QT_BEGIN_NAMESPACE
class QTreeWidget;
class QTreeWidgetItem;
QT_END_NAMESPACE

class XbelHandler : public QXmlDefaultHandler
{
public:
  XbelHandler(CommonTreeWidget *treeWidget);

  bool startElement(const QString &namespaceURI, const QString &localName,
                    const QString &qName, const QXmlAttributes &attributes);
  bool endElement(const QString &namespaceURI, const QString &localName,
                  const QString &qName);
  bool characters(const QString &str);
  bool fatalError(const QXmlParseException &exception);
  QString errorString() const;

private:
  QDataItem *createChildItem(const QString &tagName);

  CommonTreeWidget *treeWidget;
  QDataItem *item;
  QString currentText;
  QString errorStr;
  bool metXbelTag;

  QIcon folderIcon;
};

#endif
