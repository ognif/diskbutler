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

#ifndef MDICHILD_DIALOG_H
#define MDICHILD_DIALOG_H

//#include <QDialog>
//#include <QMainWindow>
#include "mdichild_base.h"
#include <QListView>
#include <QFileSystemModel>
#include <QSplitter>
#include "CommonTreeWidget.h"
#include "QDataItem.h"
#include "vrulemanager.h"

//QT_BEGIN_NAMESPACE
//class QTreeWidget;
//class QTreeWidgetItem;
//QT_END_NAMESPACE

class MdiChildDialog : public MdiChildBase
{
  Q_OBJECT

public:
  MdiChildDialog();
  MdiChildDialog(RuleManager::ProjectType projectType);

  void newFile();
  bool loadFile(const QString &fileName);
  bool loadImage(const QString &fileName);
  bool loadDisk(const QString &fileName);
  bool save();
  bool saveAs();
  bool saveFile(const QString &fileName);
  bool exportIndexFile();
  QString userFriendlyCurrentFile();
  QString currentFile() {return curFile;}
  bool isImagefile() {return isImageFile;}
  QDataItem *GetSelectedTreeItem();

  bool hasSelectedTreeItem();
  void insertItem();
  void insertNode(const QString &name);
  //void addFolder(const QString &name);
  void deleteItem();
  void renameItem();
  void addNewFolder();
  bool isDataTrackExist();
  void addDataTrack();
  void addAudioTrack();
  void selectAll();
  void reverseSelection();
  void updateProject();
  void moveAudioTrack(int index_change);
  void toggleFileExplorerTree();
  void toggleFileExplorerList();
  void toggleFileExplorer();
  QString updateStatus();
  bool gotoListViewParent();
  QString getListViewRootPath() const;
  void triggerFSSync();
  void triggerFSUnsync();
  void triggerReset();
  RuleManager::ProjectType GetProjectType() {return mProjectType;}
  bool GetEraseable() {return false;}
  bool GetOpenDisk() {return false;}
  CommonTreeWidget *getTreeWidget() {return treeWidget;}
  void checkAndTags(int32 sourceTags);

Q_SIGNALS:
  void status_changed(const QString &text);
  void list_view_root_changed(QString &root_path);
  void datatrack_changed();
  void audiotrack_changed(bool bEnable);
  void extract_item();
  void grab_item();

protected:
  void closeEvent(QCloseEvent *event);
  void createFileExplorers();

private:
  bool maybeSave();
  void setCurrentFile(const QString &fileName, bool isFile = true);
  QString strippedName(const QString &fullFileName);
  void createTreeWidget(RuleManager::ProjectType projectType, bool isXbel = false);
  QString buildExploreStatusBar(QString inProjectType);
  QString buildCommonCD(QString inProjectType);
  QString buildAudioCD(QString inProjectType);

private slots:
  void documentWasModified(bool modified);
  void on_treeView_clicked(const QModelIndex &index);
  void on_listView_doubleClicked(const QModelIndex &index);
  void on_project_selection_changed();
  void on_datatrack_changed();
  void on_audiotrack_changed(bool);
  void on_extract_item();
  void on_grab_item();

private:
  RuleManager::ProjectType mProjectType;
  QString curFile;
  bool isUntitled;
  bool isImageFile;
  CommonTreeWidget *treeWidget;
  QTreeView *listFileExplorer;
  QTreeView *treeFileExplorer;
  QFileSystemModel *listModel;
  QFileSystemModel *treeModel;

  QSplitter *fileExplorerSplitter;

  QDataItem *item;
  QPixmap folderOnPixmap, folderOffPixmap, filePixmap;
};

#endif
