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

#ifndef MDICHILD_DIALOG_H
#define MDICHILD_DIALOG_H

//#include <QDialog>
//#include <QMainWindow>
#include "mdichild_base.h"
#include <QListView>
#include <QFileSystemModel>
#include <QSplitter>
#include "commontreewidget.h"
#include "qdataitem.h"
#include "qdiskitem.h"
#include "vrulemanager.h"
#include "mainwindow.h"
#include "qttreepropertybrowser.h"
#include "qtpropertymanager.h"
#include "qtvariantproperty.h"
#include "qteditorfactory.h"

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
  void togglePropertyGrid();
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
  QString getlastSelectedTab(){ return strlastSelectedTab; }
  void setlastSelectedTab( QString nValue ) {
      qDebug() << "mdichild_dialog " << nValue;
      strlastSelectedTab = nValue;
  }
  void setUIControls(Ribbon *baseRibbon, QWidget* parent);
  void setBurnDeviceList(QWidget* parent);
  void setRibbonTabs(Ribbon *baseRibbon, QWidget* parent);


Q_SIGNALS:
  void status_changed(QString text, bool isRight);
  void list_view_root_changed(QString &root_path);
  void datatrack_changed();
  void audiotrack_changed(bool bEnable);
  void extract_item();
  void grab_item();
  void propertyItemChanged(QDataItem*);



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
  QString buildAudioCD(QString inProjectType, bool doDetails = true);
  bool isParentAudioTrack(QDataItem *item);
  int countTotalTracks();
  bool isAudioTrack();
  void blockAllWidgetEvents(QWidget* parent, bool bBlock = false);

private slots:
  void documentWasModified(bool modified);
  void onTreeViewClicked(const QModelIndex &index);
  void onListViewDoubleClicked(const QModelIndex &index);
  void onProjectSelectionChanged();
  void onDatatrackChanged();
  void onAudiotrackChanged(bool);
  void onExtractItem();
  void onGrabItem();
  void onStatusMessage(QString msg, bool isRight);
  void onUpdatePropertyValues();
  void onUpdatePropertyValuesExtern(QDataItem *item);

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

  void BuildPropertyTree();

  QSplitter *fileExplorerSplitter;
  QSplitter *fileTreeSplitter;

  QtTreePropertyBrowser *propertyTree;


  QDataItem *item;
  QPixmap folderOnPixmap, folderOffPixmap, filePixmap;
  QString strlastSelectedTab;


  //Property
  QtVariantPropertyManager *sdkItemPropManager;
  QtVariantProperty *sItemPropName;
  QtVariantProperty *sItemPropSourcePath;
  QtVariantProperty *sItemPropChecksum;
  QtVariantProperty *sItemPropComment;
  QtVariantProperty *sItemPropDiskPath;
  QtVariantProperty *nItemPropSize;

  QtVariantProperty *nDiskPropSize;
  QtVariantProperty *sDiskPropAmount;
  QtVariantEditorFactory *sdkFilePropFactory;
  QtEnumPropertyManager *sdkEmuManager;
  QtProperty *groupFilePropDate;
  QtProperty *groupFilePropEx;
  QtProperty *groupFilePropAtrributes;
  QtProperty *nItemPropPriority;
  QtEnumEditorFactory *cbPriorityFactory;

  //Property Attributes
  QtVariantProperty *bItemPropAttributeAHidden;
  QtVariantProperty *bItemPropAttributeHidden;
  QtVariantProperty *bItemPropAttributeArchive;
  QtVariantProperty *bItemPropAttributeDir;

  //Property Dates
  QtVariantProperty *dItemPropCreatedDateTime;
  QtVariantProperty *dItemPropModifiedDateTime;
  QtVariantProperty *dItemPropAccessDateTime;

  QtVariantProperty *bUseDatesProp;

  QtVariantProperty *bUsePropCreatedDate;
  QtVariantProperty *bUsePropModifiedDate;
  QtVariantProperty *bUsePropExpirationDate;
  QtVariantProperty *bUsePropEffectiveDate;

  QtVariantProperty *dDiskPropCreatedDateTime;
  QtVariantProperty *dDiskPropModifiedDateTime;
  QtVariantProperty *dDiskPropExpirationDateTime;
  QtVariantProperty *dDiskPropEffectiveDateTime;

};

#endif
