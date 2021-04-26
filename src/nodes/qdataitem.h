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

#ifndef QDATAITEM_H
#define QDATAITEM_H

#include <QTreeWidgetItem>
#include <QXmlDefaultHandler>
#include <QDateTime>
#include <QDebug>
#include "FoxSDKExport.h"

//We have changed it from slash to backslash. We keep everything backslash here.

#if defined (WIN32)
#define PATHSEPSTRING "\\"
#else
#define PATHSEPSTRING "/"
#endif


class CommonTreeWidget;

class QDataItem : public QTreeWidgetItem
{
public:
    enum ValueType {
        File,
        Folder,
        VirtualFolder,
        FixedFolder,
        DataTrack,
        AudioTrack,
        Session,
        Disk
    };
public:
    explicit QDataItem(CommonTreeWidget *view);
    explicit QDataItem(QDataItem *parent = 0);
    virtual ~QDataItem(){}
    virtual QDataItem *clone() const;

    void SetType(ValueType type);
    ValueType GetType() {return mType;}
    QString GetName() {return mName;}
    void SetName(const QString &name) {mName = name;}
    void checkPathAndNaming();
    void setIconWarning(bool isWarning, const QString &strWarning);

    //Virtual for Files and Folders
    virtual QString GetLBA() {return "";}
    virtual QString getPath() {return "";}
    virtual QString createAttributeString() {return "";}
    virtual std::vector<uint32>* GetExtentLocation() {return NULL;}
    virtual std::vector<uint32>* GetExtentLength() {return NULL;}

    //Virtual for Disk
    virtual QString getMediumType() {return "";}
    virtual QString getMediumStatus() {return ""; }
    virtual QString getUPCCode() {return "";}
    virtual int getTrackCount() {return 0;}
    virtual int getSessionCount() {return 0;}
    virtual qint64 getMediumSize() {return 0;}
    virtual qint64 getMediumUsedSize() {return 0;}
    virtual qint64 getMediumFreeSize() {return 0;}
    virtual int16 getImageFormats() {return 0;}

    //Virtual for Session
    virtual int getFirstTrack() {return 0;}
    virtual int getLastTrack() {return 0;}
    virtual int getLastSession() {return 0;}
    virtual qint64 getSessionSize() {return 0;}
    virtual int getStartLBA() {return 0;}

    //Virtual for Track
    virtual int getTrackIndex() {return 0;}
    virtual int getSessionNumber() {return 0;}
    virtual qint64 getTrackSize(){return 0;}
    virtual qint64 getSectorSize(){return 0;}
    virtual QString GetModeStr() {return "";}

    //Virtual for UDF
    virtual QString getUDFTypeString() {return "";}
    virtual QString getUDFPartionType() {return "";}

    //Test
    virtual bool isUDFFileSystem() {return false;}
    virtual bool isISOFileSystem() {return false;}

    virtual int32 fsTypeInt() {return BS_FS_UNKNOWN;}

    void SetText0(const QString &text) {setText(0, text);}
    static void AddDefaultIcon(const QPixmap &folderIconOn,
                               const QPixmap &folderIconOff,
                               const QPixmap &fixedFolderIconOn,
                               const QPixmap &fixedFolderIconOff,
                               const QPixmap &fileIcon);
    void SetDefaultIcon();
    void SetDataWithName(const QString &name);
    void SetData(const QString &path);
    void SetDataAudio(const QString &path, qint64 data_time, const QString &comment);
    void SetData(ValueType type, const QString &path, const QString &name,
                 qint64 size, int item_count, int node_count);
    void SetData(ValueType type, const QString &path, const QString &name,
                 qint64 size, int item_count, int node_count,
                 bool bHidden, bool bHiddenExt, int priority,
                 QDateTime date_added, QDateTime date_created, QDateTime date_modified,
                 const QString &checksum, const QString &comment);
    void SetDataSize(const qint64 data_size,bool isExplorer=false);
    void SetDataLBA(const qint64 data_size);

    void SetDataTime(const qint64 data_time);
    qint64 GetDataSize() {return mSize;}
    void SetFullPath(const QString &path) {mFullPath = path;}
    QString GetFullPath() {return mFullPath;}
    QString GetDiskPath();
    QString GetDiskFilePath();
    void SetDataItemCount(const int item_count) {mItemCount = item_count;}
    int GetDataItemCount() {return mItemCount;}
    void SetDataNodeCount(const int node_count) {mNodeCount = node_count;}
    int GetDataNodeCount() {return mNodeCount;}
    int SetDataTrackMode(int mode) {
        mDataTrackMode = mode;
        QString text = "DataTrack(Mode" + QString::number(mode) + ")";
        setText(0, text);
        return mode;
    }
    void SetAudioTrackInfo(QString strInfo){
        setText(0, strInfo);
    }

    int GetDataTrackMode() {return mDataTrackMode;}
    bool DoValidate();
    virtual QString ToXMLElementIndexFile(int depth);
    virtual QString ToXMLElement(int depth);
    virtual void FromXMLElement(const QString &qName, const QXmlAttributes &attributes);

    bool GetHidden() {return mIsHidden;}
    void SetHidden(bool isChecked) {mIsHidden = isChecked;}
    bool GetHiddenExt() {return mIsHiddenExt;}
    void SetHiddenExt(bool isChecked) {mIsHiddenExt = isChecked;}
    int GetPriority() {return mPriority;}
    void SetPriority(int priority) {mPriority = priority;}
    QDateTime GetDateAdded() {return mDateAdded;}
    void SetDateAdded(QDateTime date) {mDateAdded = date;}
    QDateTime GetDateCreated() {return mDateCreated;}
    void SetDateCreated(QDateTime date) {mDateCreated = date;}
    QDateTime GetDateModified() {return mDateModified;}
    void SetDateModified(QDateTime date) {mDateModified = date;}
    void ParseAndSetDateTime(const SFileEntry &info);
    QByteArray GetChecksum() {return mChecksum;}
    void SetChecksum(QByteArray checksum) {mChecksum = checksum;}
    QString GetComment() {return mComment;}
    void SetComment(QString comment) {mComment = comment;}
    int getItemPlayTime() {return mPlayTime;}

protected:
    QString indent(int depth);
    QString escapedText(const QString &str);
    QString escapedAttribute(const QString &str);

private:
    ValueType mType;
    // if no need to set item as different folder or file icon, that's better
    // to set following member as static
    static QIcon mFolderIcon;
    static QIcon mFixedFolderIcon;
    static QIcon mFileIcon;
    QIcon mOrgIcon;

    QString mFullPath;
    QString mName;
    QString mWarning;
    qint64 mSize;
    int mItemCount;
    int mNodeCount;
    bool mIsError;
    QString mErrorDescription;
    int mDataTrackMode;


    // item extension property
    QDateTime mDateAdded;
    QDateTime mDateCreated;
    QDateTime mDateModified;
    bool mIsHidden;
    bool mIsHiddenExt;
    int  mPriority;
    QByteArray mChecksum;
    QString mComment;
    qint64 mPlayTime;
};

#endif
