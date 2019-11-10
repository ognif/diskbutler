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
