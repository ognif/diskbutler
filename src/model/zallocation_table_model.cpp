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

#include "zallocation_table_model.h"

ZAllocationTableModel::ZAllocationTableModel(QObject *parent)
  : QAbstractTableModel(parent)
{
}

ZAllocationTableModel::ZAllocationTableModel(QList<QPair<QString, QString> > pairs, QObject *parent)
  : QAbstractTableModel(parent)
{
  listOfPairs = pairs;
}

int ZAllocationTableModel::rowCount(const QModelIndex &parent) const
{
  Q_UNUSED(parent);
  return listOfPairs.size();
}

int ZAllocationTableModel::columnCount(const QModelIndex &parent) const
{
  Q_UNUSED(parent);
  return 2;
}

QVariant ZAllocationTableModel::data(const QModelIndex &index, int role) const
{
  if (!index.isValid())
    return QVariant();

  if (index.row() >= listOfPairs.size() || index.row() < 0)
    return QVariant();

  if (role == Qt::DisplayRole) {
    QPair<QString, QString> pair = listOfPairs.at(index.row());

    if (index.column() == 0)
      return pair.first;
    else if (index.column() == 1)
      return pair.second;
  }
  return QVariant();
}

QVariant ZAllocationTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
  if (role != Qt::DisplayRole)
    return QVariant();

  if (orientation == Qt::Horizontal) {
    switch (section) {
    case 0:
      return tr("Extent Location");

    case 1:
      return tr("Extent Length (bytes)");

    default:
      return QVariant();
    }
  }
  return QVariant();
}

bool ZAllocationTableModel::insertRows(int position, int rows, const QModelIndex &index)
{
  Q_UNUSED(index);
  beginInsertRows(QModelIndex(), position, position + rows - 1);

  for (int row = 0; row < rows; ++row) {
    QPair<QString, QString> pair(" ", " ");
    listOfPairs.insert(position, pair);
  }

  endInsertRows();
  return true;
}

bool ZAllocationTableModel::removeRows(int position, int rows, const QModelIndex &index)
{
  Q_UNUSED(index);
  beginRemoveRows(QModelIndex(), position, position + rows - 1);

  for (int row = 0; row < rows; ++row) {
    listOfPairs.removeAt(position);
  }

  endRemoveRows();
  return true;
}

bool ZAllocationTableModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
  if (index.isValid() && role == Qt::EditRole) {
    int row = index.row();

    QPair<QString, QString> p = listOfPairs.value(row);

    if (index.column() == 0)
      p.first = value.toString();
    else if (index.column() == 1)
      p.second = value.toString();
    else
      return false;

    listOfPairs.replace(row, p);
    emit(dataChanged(index, index));

    return true;
  }

  return false;
}

Qt::ItemFlags ZAllocationTableModel::flags(const QModelIndex &index) const
{
  if (!index.isValid())
    return Qt::ItemIsEnabled;

  return QAbstractTableModel::flags(index) | Qt::ItemIsEditable;
}

QList< QPair<QString, QString> > ZAllocationTableModel::getList()
{
  return listOfPairs;
}
