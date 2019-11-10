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
