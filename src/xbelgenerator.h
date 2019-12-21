/*
 *  DiskButler - a powerful CD/DVD/BD recording software tool for Linux, macOS and Windows.
 *  Copyright (c) 20019 Ingo Foerster (pixbytesl@gmail.com).
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
