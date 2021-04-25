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

#ifndef ZPROJECT_EXPLORE_H
#define ZPROJECT_EXPLORE_H

#include "CommonTreeWidget.h"

class ZProjectExplore : public CommonTreeWidget
{
  Q_OBJECT
public:
  ZProjectExplore(QWidget *parent = 0);

public Q_SLOTS:
  void extractContent();
  void extractTrack();
  void extractDisk();
  void contenList();
  void trackIndex();
  void grabTrack();

protected:
  QAction *mExploreExtractAct;
  QAction *mExploreExtractRunAct;
  QAction *mExploreContentList;
  QAction *mExploreExtractTrack;
  QAction *mExploreExtractDisk;
  QAction *mExploreIndexList;
  QAction *mExploreGrabTrack;

  virtual void contextMenuEvent(QContextMenuEvent *event);
  virtual void showProperty_File();
  virtual void showProperty_Disk();
};

#endif // ZPROJECT_EXPLORE_H
