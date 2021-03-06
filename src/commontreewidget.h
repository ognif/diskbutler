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

#ifndef COMMONTREEWIDGET_H
#define COMMONTREEWIDGET_H

#include <QTreeWidget>
#include <QSignalMapper>
#include <QAction>
#include <QThread>
#include "qdataitem.h"
#include "QDir"
#include "vrulemanager.h"
#include "zimport_strategy.h"
#include "qtwaitingspinner.h"

class QtWaitingSpinner;

class ThreadAddTree : public QThread
{
    Q_OBJECT
public:
    ThreadAddTree( CommonTreeWidget *tree, QDataItem *parent, const QString &path ) {
        mTree = tree;
        mParent = parent;
        mPath = path;
    }
signals:
    void completed( QDataItem *parent, QDataItem *item );
    void addTreeEx( bool*, QString );
protected:
    void run();
    CommonTreeWidget *mTree;
    QDataItem *mParent;
    QString mPath;
};

class CommonTreeWidget : public QTreeWidget
{
    Q_OBJECT
    friend class ThreadAddTree;
public:
    CommonTreeWidget( RuleManager::ProjectType projectType, bool isXbel, QWidget *parent = nullptr );
    QDataItem* InsertItem( const QString &path );
    QDataItem* InsertPlaylistItem( const QString &path );
    void InsertItemFromRibbon();
    void InsertNode( const QString &path );
    QDataItem* InsertNodeToISOUDFRoot( const QString &path );
    void Import( const QString &path );
    void setOverwriteFlag( bool setState ) { addTRackCDTextToDisc=setState; }

    void SetDataTrackItem( QDataItem *datatrack );
    double getTotalTime();
    void setRootItem();
    void calculateRealSizes();

Q_SIGNALS:
    void contentsChanged( bool changed );
    void datatrackChanged();
    void audiotrackChanged( bool bEnable );
    void extractItem();
    void grabItem();
    void testMessage( const QString, bool* );
    void statusMessage(QString msg, bool isRight);
    void propertyItemChanged(QDataItem*);

public Q_SLOTS:
    QDataItem* AddNewFolder();
    void DeleteItem();
    QDataItem* AddDataTrack();
    QDataItem* AddAudioTrack();
    void ImportPlaylist();
    void SelectAll();
    void ReverseSelection();
    void UpdateProject();
    void MoveAudioTrack( int index_change );
    void doFSSync(const QString &remove, const QString &keep);
    void FSSync();
    void FSUnsync();
    void ResetFiles();
    void synchronizeItem();
    void createContentList();
    void createIndex();
    void slot_rename_in_place();
    bool onAddTree(QString path,bool* newData);

public:
    QDataItem *SetSelectedParent();
    QDataItem *GetSelectedItem();
    void SetDataSize(qint64 size) { mSize = size; }
    qint64 GetDataSize() { return mSize; }
    void SetDataItemCount( int count ) { mItemCount = count; }
    int GetDataItemCount() { return mItemCount; }
    bool IsModified() { return mModified; }
    void resetModified() { mModified=false; }
    bool IsDataTrackExist() { return nullptr != mDataTrackItem; }
    void SetProjectType( RuleManager::ProjectType type ) { mProjectType = type; }
    RuleManager::ProjectType GetProjectType() { return mProjectType; }
    bool GetEraseable() { return false; }
    bool GetOpenDisk() { return false; }
    QDataItem* getSessionItem() { return mSessionItem; }
    void randomizeAudioTracks();
    void getTags(int32 sourceTags);
    int getTotalTimeExplorer();
    void stopFromExternal();

protected:
    void dragEnterEvent( QDragEnterEvent *event );
    void dragMoveEvent( QDragMoveEvent *event );
    void dragLeaveEvent( QDragLeaveEvent *event );
    void dropEvent( QDropEvent *event );
    void mousePressEvent( QMouseEvent *event );
    void contextMenuEvent( QContextMenuEvent *event );
    //bool dropMimeData(QTreeWidgetItem *parent, int index, const QMimeData *data, Qt::DropAction action);

protected:
    QDataItem* InsertDataItem( const QString &path );
    QDataItem* InsertAudioItem( const QString &path );
    QDataItem* InsertItemOperation( QDataItem *parent, const QString &path );
    QDataItem* InsertItem( QDataItem *parent, const QString &path, bool silent = false );
    QDataItem* addTree( QDataItem *parent, const QString &path,
                        qint64 &data_size, int &item_count, int &node_count,
                        bool sort = false );
    QDataItem* addFile( QDataItem *parent, const QFileInfo &entry
                        , qint64 &data_size, int &item_count );
    QDataItem* addFileEx( QDataItem *parent, const QFileInfo &entry
                          , qint64 &data_size, int &item_count );
    void setSelected( QDataItem *item );
    void updateDataSize( QDataItem *parent, qint64 size, int item_count, int node_count );
    bool isAudioFile( const QString &path );
    bool isAudioFile( const QFileInfo &file );
    QDataItem* getDataTrackItem();
    QDataItem* addAudioTrack();
    QDataItem* addAudioToNewAudioTrack( const QString &path );
    QDataItem* replaceAudioInAudioTrack( QDataItem *audio_track, const QString &path );
    QDataItem* addAudioToEmptyAudioTrack( QDataItem *audio_track, const QString &path );
    QDataItem* addFSVirtualFolder( const QString &name );
    inline QList<QTreeWidgetItem*> takeChildrenWithUpdatedInfo(QDataItem *parent, bool doInfos = true);
    void removeChildWithUpdatedInfo( QDataItem *parent, QDataItem *child );
    void addChildrenWithUpdatedInfo( QDataItem *parent, const QList<QTreeWidgetItem*> &children, qint64 data_size, int item_count, int node_count );
    void addChildWithUpdatedInfo( QDataItem *parent, QDataItem *child );
    void updateAudioTrackName();
    bool queryAddAudioTrack();
    void resetQueryAddAudioTrack();
    int findSameNameInLevel( QDataItem *parent, const QString &path, Qt::CaseSensitivity cs = Qt::CaseSensitive );
    bool queryReplaceFile( const QString &path );
    void resetQueryReplaceFile();
    bool isFiltered( QFileInfo info );
    bool getIsFSSyncAvailable();
    bool getIsFSUnsyncAvailable();
    void changeFolderPos( QDataItem *parent, QDataItem *child_folder );
    QString buildDiskName();

    int audioTrackCount();
    bool callerContainDataTrack(QDataItem *item);

    void setSizes();

    // For import
    int importFromRoot( QDataItem *parent, const QString &root,
                        qint64 &data_size, int &item_count, int &node_count );
    QDataItem* importTree( QDataItem *parent, const QString &path,
                           qint64 &data_size, int &item_count, int &node_count,
                           bool sort = true );

    QDataItem* importItem( QDataItem *parent, const QString &path );
    ZImportStrategy::ImportError importCheck( const QString &path );
    ZImportStrategy::ImportError importCheck2( const QString &folder_name, const QString &path, bool silent = false );
    void addFixedFolder( const QString &parent, const QStringList &folders );
    void createFixedFolder();
    QDataItem* findItemByName( QDataItem *parent, const QString &name );

    void handleRename( QTreeWidgetItem *item, int column );
    void handleChangeComment( QTreeWidgetItem *item, int column );

protected Q_SLOTS:
    //void slot_handle_collapse(QTreeWidgetItem *item);
    void slot_handle_itemChanged( QTreeWidgetItem *item, int column );
    void slot_insert_item_from_dialog();
    void slot_insert_node_from_dialog();
    void slot_import_from_dialog();
    //void slot_handle_currentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous);
    void slot_handle_itemSelectChanged();
    void slot_handle_itemDoubleClicked( QTreeWidgetItem *item, int column );

    void clearAudio();
    void insertAudio();

    void showPlay();
    void showStop();
    void dataTrackMode1();
    void dataTrackMode2();
    void onAddTreeCompleted(QDataItem *parent,QDataItem *item);

protected:
    bool mModified;
    bool mNeedQueryAddAudioTrack;
    bool mDoAddAudioTrack;
    bool mNeedQueryReplaceFile;
    bool mDoReplaceFile;
    QDataItem *mSelectedItem;
    QDataItem *mHeadItem;
    QDataItem *mSessionItem;
    QDataItem *mDataTrackItem;
    qint64 mSize;
    int mItemCount;
    int mNodeCount;
    int mAudioCount;
    int getAudioTrackCount();
    bool addTRackCDTextToDisc;
    ThreadAddTree *mThreadAddTree;
    QtWaitingSpinner *mBlockSpinner;

    QSignalMapper *mSignalMapper;
    QAction *mInsertItemAct;
    QAction *mInsertNodeAct;
    QAction *mDeleteAct;
    QAction *mAddNodeAct;
    QAction *mResetAct;
    // For session
    QAction *mInsertDataTrackAct;
    QAction *mInsertAudioTrackAct;
    QAction *mImportPlaylistAct;
    // For data track
    QAction *mDataTrackMode1Act;
    QAction *mDataTrackMode2Act;
    // For FSSync\FSUnsync
    QAction *mFSSyncAct;
    QAction *mFSUnsyncAct;
    // For audio
    QAction *mClearAudioAct;
    QAction *mInsertAudioAct;
    QAction *mPlayAct;
    QAction *mStopAct;
    QAction *mAudioUp;
    QAction *mAudioDown;
    // For folder and files
    QAction *mSyncItemAct;

    QAction *mSelectAllAct;
    QAction *mReverseSelectionAct;
    QAction *mRenameAct;

    RuleManager::ProjectType mProjectType;
    const static QString KStrISO;
    const static QString KStrUDF;

    // For import
    bool mQueryResetBeforeImport;
    QAction *mImportAct;
    QList<ZImportStrategy*> mImportStrategies;
    QString mRootToBeImport;
    QString mImportErr;
    QMap<QString, QStringList> mFixedFolderMap;

};

#endif
