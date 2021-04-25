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

#ifndef ZPLAYLISTLOADER_H
#define ZPLAYLISTLOADER_H

#include <QIcon>
#include "CommonTreeWidget.h"
#include "qdataitem.h"
#include "FoxSDKBurningLib.h"
#include <vector>

class ZPlaylistItem
{
public:
  ZPlaylistItem()
    : mPath(""), mTitle(""), mSeconds(-1), mType(NORM) {}
  ZPlaylistItem(QString path, QString title, int seconds, int type)
    : mPath(path), mTitle(title), mSeconds(seconds), mType(type) {}
  void set(QString path, QString title, int seconds, int type) {
    mPath = path;
    mTitle = title;
    mSeconds = seconds;
    mType = type;
  }

  enum TYPE {
    NORM,
    HTTP
  };

  QString mPath;
  QString mTitle;
  int mSeconds;
  int mType;
};

class ZPlaylistLoader
{
public:
  ZPlaylistLoader(CommonTreeWidget *tree);
  void Load(const QString &playlistPath);

private:
  bool parse(const QString &playlistPath);
  void parseM3U(const QString &playlistPath, QString &all);
  void parsePLS(const QString &playlistPath, QString &all);
  void addItem(const ZPlaylistItem &item);
  void updateTree(const QString &playlistPath);
private:
  CommonTreeWidget *mTree;
  std::vector<ZPlaylistItem> mItemList;
};

#endif
