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

#include <QFileInfo>
#include <QFileIconProvider>
#include <qstack.h>

#include "qdataitem.h"
#include "commontreewidget.h"
#include "utils_common.h"
#include "settingspages.h"

QIcon QDataItem::mFolderIcon;
QIcon QDataItem::mFixedFolderIcon;
QIcon QDataItem::mFileIcon;

QDataItem::QDataItem(CommonTreeWidget *view)
    : QTreeWidgetItem(view, UserType)
    , mType(File)
    , mSize(0)
    , mItemCount(0)
    , mNodeCount(0)
    , mIsError(0)
    , mErrorDescription("")
    , mDataTrackMode(1)
    , mPlayTime(0)
{
}

QDataItem::QDataItem(QDataItem *parent)
    : QTreeWidgetItem(parent)
    , mType(File)
    , mSize(0)    
    , mItemCount(0)
    , mNodeCount(0)
    , mIsError(0)
    , mErrorDescription("")
    , mDataTrackMode(1)
    , mPlayTime(0)
{
}


QDataItem * QDataItem::clone() const
{
    QDataItem *copy = nullptr;

    QStack<const QDataItem*> stack;
    QStack<QDataItem*> parentStack;
    stack.push(this);
    parentStack.push(nullptr);

    QDataItem *root = nullptr;
    const QDataItem *item = nullptr;
    QDataItem *parent = nullptr;
    while (!stack.isEmpty()) {
        item = stack.pop();
        parent = parentStack.pop();

        copy = new QDataItem(*item);
        if (!root)
            root = copy;

        if (parent) {
            parent->insertChild(0, copy);
        }

        for (int i = 0; i < item->childCount(); ++i) {
            stack.push(static_cast<QDataItem*>(item->child(i)));
            parentStack.push(copy);
        }
    }
    return root;
}

void QDataItem::SetType(ValueType type)
{
    mType = type;
    //if (File == mType) {
    //  setIcon(0, mFileIcon);
    //} else if (Folder == mType) {
    //  setIcon(0, mFolderIcon);
    //}
}

void QDataItem::AddDefaultIcon(const QPixmap &folderIconOn,
                               const QPixmap &folderIconOff,
                               const QPixmap &fixedFolderIconOn,
                               const QPixmap &fixedFolderIconOff,
                               const QPixmap &fileIcon)
{
    mFolderIcon.addPixmap(folderIconOn, QIcon::Normal, QIcon::On);
    mFolderIcon.addPixmap(folderIconOff, QIcon::Normal, QIcon::Off);
    mFixedFolderIcon.addPixmap(fixedFolderIconOn, QIcon::Normal, QIcon::On);
    mFixedFolderIcon.addPixmap(fixedFolderIconOff, QIcon::Normal, QIcon::Off);
    mFileIcon.addPixmap(fileIcon);
}

void QDataItem::SetDefaultIcon()
{
    if (QDataItem::Folder == mType) {
        setIcon(0, mFolderIcon);
    } else if (QDataItem::FixedFolder == mType) {
        setIcon(0, mFixedFolderIcon);
    } else if (QDataItem::File == mType) {
        setIcon(0, mFileIcon);
    }
}

void QDataItem::SetDataWithName(const QString &name)
{
    mName = name;
    mIsHidden = false;
    mIsHiddenExt = false;
    mIsArchive = false;
    mIsDirectory = false;
    mPriority = 0;
    mDateAdded = QDateTime::currentDateTime();
    mDateCreated = mDateAdded;
    mDateModified = mDateAdded;
    SetDefaultIcon();
    setText(0, mName);
    setText(1, humanReadableSize(mSize,nullptr));
    setText(RuleManager::TYPE_PROJECT_MIXEDMODE == project_type?3:2, mComment);
}

void QDataItem::checkPathAndNaming()
{
    //Get the Path from this item
    //mName
    //mFullPath
    //mOrgIcon
    //Wo bekomme ich jetzt die Regeln her?

}

void QDataItem::SetData(const QString &path)
{
    QFileInfo fileinfo(path);

    //Path longer than 255 chars?
    fileinfo.setFile(fileinfo.absoluteFilePath());
    mName = fileinfo.fileName();

    mDateAdded = QDateTime::currentDateTime();
    if (0 == path.compare(mName)) {
        // it's a virtual new added item, not from filesystem.
        mFullPath = "";
        mDateCreated = mDateAdded;
        mDateModified = mDateAdded;
    } else {
        mFullPath = fileinfo.absoluteFilePath();
        mDateCreated = fileinfo.birthTime();
        mDateModified = fileinfo.lastModified();
    }

    mSize = fileinfo.size();
    mIsHidden = false;
    mIsHiddenExt = false;
    mIsArchive = false;
    mIsDirectory = false;
    mPriority = 0;

    //Der BEreich muss noch gethreaded werden
    int64 size = ConfigurationPage::mSettings.value("NoChecksumFileSize", "32").toInt();
    if (ConfigurationPage::mSettings.value("CreateAndShowChecksum", true).toBool()
            && mSize <= size*1024*1024) {
        mChecksum = CalcChecksum(mFullPath);
    } else {
        mChecksum = "";
    }


    mComment = "";
    //Check Icon, if Warning, we use the warn provider
    //Then we need to save the provider Icon in list.
    QFileIconProvider iconprovider;
    //QIcon icon = iconprovider.icon(fileinfo);
    mOrgIcon = iconprovider.icon(fileinfo);
    setIcon(0, mOrgIcon);
    setText(0, mName);
    setText(1, humanReadableSize(mSize,nullptr));
    setText(RuleManager::TYPE_PROJECT_MIXEDMODE == project_type?3:2, mComment);
}

void QDataItem::setIconWarning(bool isWarning, const QString &strWarning)
{
    if(isWarning == true){
        setIcon(0, QIcon(":/icons/warning16.png"));
        mWarning = strWarning;
        setText(RuleManager::TYPE_PROJECT_MIXEDMODE == project_type?3:2, mWarning);
    }else{
        mWarning = "";
        if (QDataItem::Folder == mType) {
            setIcon(0, mFolderIcon);
        } else if (QDataItem::FixedFolder == mType) {
            setIcon(0, mFixedFolderIcon);
        } else if (QDataItem::File == mType) {
            setIcon(0, mOrgIcon);
        }
        setText(RuleManager::TYPE_PROJECT_MIXEDMODE == project_type?3:2, mComment);
    }
}

void QDataItem::SetDataAudio(const QString &path, double data_time, const QString &comment)
{
    QFileInfo fileinfo(path);
    fileinfo.setFile(fileinfo.absoluteFilePath());
    mName = fileinfo.fileName();

    if (0 == path.compare(mName)) {
        mFullPath = "";
    } else {
        mFullPath = fileinfo.absoluteFilePath();
    }

    mSize = fileinfo.size();
    mIsHidden = false;
    mIsHiddenExt = false;
    mIsArchive = false;
    mIsDirectory = false;
    mPriority = 0;
    mDateAdded = QDateTime::currentDateTime();
    mDateCreated = fileinfo.birthTime();
    mDateModified = fileinfo.lastModified();
    mComment = comment;
    mPlayTime = data_time;
    QFileIconProvider iconprovider;
    QIcon icon = iconprovider.icon(fileinfo);
    //setIcon(0, icon);
    setText(0, mName);
    setText(1, humanReadableSize(mSize,nullptr));
    setText(2, QDateTime::fromSecsSinceEpoch(mPlayTime).toUTC().toString("hh:mm:ss"));
    setText(3, mComment);
}

void QDataItem::SetDataAudioTrack(ValueType type, const QString &, const QString &name,
                                  qint64 size, int item_count, double length)
{
    mType = type;
    mName = name;
    mSize = size;
    mItemCount = item_count;
    mNodeCount = 0;
    mPlayTime = length;
    setText(0, mName);
    setText(1, humanReadableSize(mSize,nullptr));
    setText(2, QDateTime::fromSecsSinceEpoch(mPlayTime).toUTC().toString("hh:mm:ss"));
    setText(3, mComment);

}

void QDataItem::SetData(ValueType type, const QString &path, const QString &name,
                        qint64 size, int item_count, int node_count)
{
    mType = type;
    if (File == type) {
        mFullPath = path;
        QFileInfo fileinfo(path);
        QFileIconProvider iconprovider;
        QIcon icon = iconprovider.icon(fileinfo);
        setIcon(0, icon);
    } else if (Folder == type) {
        mFullPath = path;
    }
    mName = name;
    mSize = size;
    mItemCount = item_count;
    mNodeCount = node_count;
    setText(0, mName);
    setText(1, humanReadableSize(mSize,nullptr));
    setText(RuleManager::TYPE_PROJECT_MIXEDMODE == project_type?3:2, mComment);
}

void QDataItem::SetData(ValueType type, const QString &path, const QString &name,
                        qint64 size, int item_count, int node_count,
                        bool bHidden, bool bHiddenExt, bool bArchive, bool bDirectory, int priority,
                        QDateTime date_added, QDateTime date_created, QDateTime date_modified,
                        const QString &checksum, const QString &comment)
{
    mIsHidden = bHidden;
    mIsHiddenExt = bHiddenExt;
    mIsArchive = bArchive;
    mIsDirectory = bDirectory;
    mPriority = priority;
    mDateAdded = date_added;
    mDateCreated = date_created;
    mDateModified = date_modified;
    mChecksum = checksum.toStdString().c_str();
    mComment = comment;
    SetData(type, path, name, size, item_count, node_count);
}

void QDataItem::SetData(ValueType, const QString &path, const QString &name,
                        qint64 size, qint64 length, int, int,
                        bool bHidden, bool bHiddenExt, bool bArchive, bool bDirectory, int priority,
                        QDateTime date_added, QDateTime date_created, QDateTime date_modified,
                        const QString &checksum, const QString &comment) //AudioFile
{
    mIsHidden = bHidden;
    mIsHiddenExt = bHiddenExt;
    mIsArchive = bArchive;
    mIsDirectory = bDirectory;
    mPriority = priority;
    mDateAdded = date_added;
    mDateCreated = date_created;
    mDateModified = date_modified;
    mChecksum = checksum.toStdString().c_str();
    mComment = comment;
    mPlayTime = length;
    mName = name;
    mSize = size;
    SetDataAudio(path, mPlayTime, mComment);
}

void QDataItem::SetDataLBA(const qint64 data_size)
{
    setText(1, QString::number(data_size));
}

void QDataItem::SetDataSize(const qint64 data_size, const qint64 audio_size ,bool isExplorer)
{
    mSize = data_size;

    if(isExplorer==false){
        setText(1, humanReadableSize(data_size+audio_size,nullptr));
    }else{
        setText(2, humanReadableSize(data_size+audio_size,nullptr));
    }
}

void QDataItem::SetDataSize(const qint64 data_size,bool isExplorer)
{

    mSize = data_size;

    if(isExplorer==false){
        setText(1, humanReadableSize(data_size,nullptr));
    }else{
        setText(2, humanReadableSize(data_size,nullptr));
    }

}

void QDataItem::SetDataTime(const double data_time)
{
    mPlayTime = data_time;
    setText(2, QDateTime::fromSecsSinceEpoch(data_time).toUTC().toString("hh:mm:ss"));
}

QString QDataItem::GetDiskPath()
{
    QString disk_path;// = GetName();

    QDataItem *parent = static_cast<QDataItem*>(this->parent());

    while (parent != nullptr) {

        if(parent->GetType()==QDataItem::DataTrack)break;

        //We have changed it from slash to backslash. We keep everything backslash here.
        disk_path.insert(0, parent->GetName() + PATHSEPSTRING);
        parent = static_cast<QDataItem*>(parent->parent());

    }
    //qDebug("Path1: %s",disk_path.toLatin1().constData());
    //We have changed it from slash to backslash. We keep everything backslash here.
    disk_path.insert(0, PATHSEPSTRING);
    //qDebug("Path2: %s",disk_path.toLatin1().constData());
    return disk_path;
}

QString QDataItem::GetDiskFilePath()
{
    QString disk_path = GetName();

    QDataItem *parent = static_cast<QDataItem*>(this->parent());
    while (parent != nullptr) {
        if(parent->GetType()==QDataItem::DataTrack)break;
        //We have changed it from slash to backslash. We keep everything backslash here.
        disk_path.insert(0, parent->GetName() + PATHSEPSTRING);
        parent = static_cast<QDataItem*>(parent->parent());
    }
    //We have changed it from slash to backslash. We keep everything backslash here.
    disk_path.insert(0, PATHSEPSTRING);
    return disk_path;
}

bool QDataItem::DoValidate()
{
    if (File == mType) {
        QFileInfo fileinfo(mFullPath);
        if (fileinfo.exists()) {
            return true;
        } else {
            mIsError = true;
            mErrorDescription = "File not found";
            setText(RuleManager::TYPE_PROJECT_MIXEDMODE == project_type?3:2, mErrorDescription);
            return false;
        }
    } else {
        return true;
    }
}

QString QDataItem::ToXMLElementIndexFile(int depth)
{
    QString out = "";
    //QString tagName = item->data(0, Qt::UserRole).toString();
    if (QDataItem::Folder == mType) {
        //bool folded = !isExpanded();
        out += indent(depth)
                + "<folder name=" + escapedAttribute(text(0))
                + " size=" + escapedAttribute(QString::number(GetDataSize()))
                + " hidden=" + escapedAttribute(QString::number(GetHidden()))
                + " hiddenext=" + escapedAttribute(QString::number(GetHiddenExt()))
                + " priority=" + escapedAttribute(QString::number(GetPriority()))
                + " dateadded=" + escapedAttribute(GetDateAdded().toString())
                + " datecreated=" + escapedAttribute(GetDateCreated().toString())
                + " datemodified=" + escapedAttribute(GetDateModified().toString())
                + " comment=" + escapedAttribute(GetComment())
                + " projecttype=" + escapedAttribute(QString::number(project_type))
                + ">\n";

        for (int i = 0; i < childCount(); ++i)
            out += static_cast<QDataItem*>(child(i))->ToXMLElementIndexFile(depth + 1);


        out += indent(depth) + "</folder>\n";
    } else if (QDataItem::File == mType) {
        out += indent(depth) + "<file";
        //if (!item->text(0).isEmpty())
        out += " name=" + escapedAttribute(text(0))
                + " size=" + escapedAttribute(QString::number(GetDataSize()))
                + " path=" + escapedAttribute(GetFullPath())
                + " hidden=" + escapedAttribute(QString::number(GetHidden()))
                + " hiddenext=" + escapedAttribute(QString::number(GetHiddenExt()))
                + " priority=" + escapedAttribute(QString::number(GetPriority()))
                + " dateadded=" + escapedAttribute(GetDateAdded().toString())
                + " datecreated=" + escapedAttribute(GetDateCreated().toString())
                + " datemodified=" + escapedAttribute(GetDateModified().toString())
                + " checksum=" + escapedAttribute(GetChecksum())
                + " comment=" + escapedAttribute(GetComment())
                + " projecttype=" + escapedAttribute(QString::number(project_type));
        out += "/>\n";
    } else if (QDataItem::DataTrack == mType) {
        //bool folded = !isExpanded();
        out += indent(depth)
                + "<datatrack name=" + escapedAttribute(text(0))
                + " size=" + escapedAttribute(QString::number(GetDataSize()))
                + " projecttype=" + escapedAttribute(QString::number(project_type))
                + ">\n";

        for (int i = 0; i < childCount(); ++i)
            out += static_cast<QDataItem*>(child(i))->ToXMLElementIndexFile(depth + 1);


        out += indent(depth) + "</datatrack>\n";
    }
    return out;
}

QString QDataItem::ToXMLElement(int depth)
{
    QString out = "";
    //QString tagName = item->data(0, Qt::UserRole).toString();
    if (QDataItem::Folder == mType) {
        bool folded = !isExpanded();
        out += indent(depth)
                + "<folder name=" + escapedAttribute(text(0))
                + " size=" + escapedAttribute(QString::number(GetDataSize()))
                + " path=" + escapedAttribute(GetFullPath())
                + " item_count=" + escapedAttribute(QString::number(GetDataItemCount()))
                + " node_count=" + escapedAttribute(QString::number(GetDataNodeCount()))
                + " folded=\"" + (folded ? "yes" : "no") + "\""
        + " hidden=" + escapedAttribute(QString::number(GetHidden()))
                + " hiddenext=" + escapedAttribute(QString::number(GetHiddenExt()))
                + " priority=" + escapedAttribute(QString::number(GetPriority()))
                + " dateadded=" + escapedAttribute(GetDateAdded().toString())
                + " datecreated=" + escapedAttribute(GetDateCreated().toString())
                + " datemodified=" + escapedAttribute(GetDateModified().toString())
                + " comment=" + escapedAttribute(GetComment())
                + " projecttype=" + escapedAttribute(QString::number(project_type))
                + ">\n";

        for (int i = 0; i < childCount(); ++i)
            out += static_cast<QDataItem*>(child(i))->ToXMLElement(depth + 1);



        out += indent(depth) + "</folder>\n";
    } else if (QDataItem::File == mType) {
        out += indent(depth) + "<file";
        //if (!item->text(0).isEmpty())

        out += " name=" + escapedAttribute(text(0))
                + " size=" + escapedAttribute(QString::number(GetDataSize()));
        if(static_cast<QDataItem*>(this->parent())->GetType() == QDataItem::AudioTrack){
            out += " length=" + escapedAttribute(QString::number(GetDataTime()));
        }
        out += " path=" + escapedAttribute(GetFullPath())
                + " hidden=" + escapedAttribute(QString::number(GetHidden()))
                + " hiddenext=" + escapedAttribute(QString::number(GetHiddenExt()))
                + " priority=" + escapedAttribute(QString::number(GetPriority()))
                + " dateadded=" + escapedAttribute(GetDateAdded().toString())
                + " datecreated=" + escapedAttribute(GetDateCreated().toString())
                + " datemodified=" + escapedAttribute(GetDateModified().toString())
                + " checksum=" + escapedAttribute(GetChecksum())
                + " comment=" + escapedAttribute(GetComment())
                + " projecttype=" + escapedAttribute(QString::number(project_type));
        out += "/>\n";
    } else if (QDataItem::DataTrack == mType) {
        bool folded = !isExpanded();
        out += indent(depth)
                + "<datatrack name=" + escapedAttribute(text(0))
                + " size=" + escapedAttribute(QString::number(GetDataSize()))
                + " item_count=" + escapedAttribute(QString::number(GetDataItemCount()))
                + " node_count=" + escapedAttribute(QString::number(GetDataNodeCount()))
                + " projecttype=" + escapedAttribute(QString::number(project_type))
                + " folded=\"" + (folded ? "yes" : "no")
                + "\">\n";

        for (int i = 0; i < childCount(); ++i)
            out += static_cast<QDataItem*>(child(i))->ToXMLElement(depth + 1);

        out += indent(depth) + "</datatrack>\n";
    } else if (QDataItem::AudioTrack == mType) {
        bool folded = !isExpanded();
        out += indent(depth)
                + "<audiotrack name=" + escapedAttribute(text(0))
                + " size=" + escapedAttribute(QString::number(GetDataSize()))
                + " item_count=" + escapedAttribute(QString::number(GetDataItemCount()))
                + " projecttype=" + escapedAttribute(QString::number(project_type))
                + " folded=\"" + (folded ? "yes" : "no")
                + "\">\n";

        for (int i = 0; i < childCount(); ++i)
            out += ((QDataItem*)child(i))->ToXMLElement(depth + 1);

        out += indent(depth) + "</audiotrack>\n";
    }
    return out;
}

void QDataItem::FromXMLElement(const QString &qName, const QXmlAttributes &attributes)
{

    if (qName == "folder") {
        setFlags(flags() | Qt::ItemIsEditable);
        QString size = attributes.value("size");
        QString item_count = attributes.value("item_count");
        QString node_count = attributes.value("node_count");
        QString hidden = attributes.value("hidden");
        QString hiddenExt = attributes.value("hiddenext");
        QString archive = attributes.value("archive");
        QString directory = attributes.value("directory");
        QString priority = attributes.value("priority");
        QString dateadded = attributes.value("dateadded");
        QString datecreated = attributes.value("datecreated");
        QString datemodified = attributes.value("datemodified");

        int nDiskType = -1;
        nDiskType = attributes.value("projecttype").toInt();
        SetProjectTypeFromXML(nDiskType);

        SetData(QDataItem::Folder, attributes.value("path"), attributes.value("name"),
                size.toInt(), item_count.toInt(), node_count.toInt(),
                hidden.toInt(), hiddenExt.toInt(), archive.toInt(), directory.toInt(), priority.toInt(),
                QDateTime::fromString(dateadded), QDateTime::fromString(datecreated),
                QDateTime::fromString(datemodified), "", attributes.value("comment"));
        //setIcon(0, mFolderIcon);
        bool folded = (attributes.value("folded") != "no");
        setExpanded(!folded);
    } else if (qName == "file") {
        setFlags((flags() & (~Qt::ItemIsDropEnabled)) | Qt::ItemIsEditable);
        QString size = attributes.value("size");

        QString hidden = attributes.value("hidden");
        QString hiddenExt = attributes.value("hiddenext");
        QString archive = attributes.value("archive");
        QString directory = attributes.value("directory");
        QString priority = attributes.value("priority");
        QString dateadded = attributes.value("dateadded");
        QString datecreated = attributes.value("datecreated");
        QString datemodified = attributes.value("datemodified");

        int nDiskType = -1;
        nDiskType = attributes.value("projecttype").toInt();
        SetProjectTypeFromXML(nDiskType);

        if(attributes.value("length").count()==0){

            SetData(QDataItem::File, attributes.value("path"),
                    attributes.value("name"), size.toInt(), 1, 0,
                    hidden.toInt(), hiddenExt.toInt(), archive.toInt(), directory.toInt(), priority.toInt(),
                    QDateTime::fromString(dateadded), QDateTime::fromString(datecreated),
                    QDateTime::fromString(datemodified), attributes.value("checksum"),
                    attributes.value("comment"));

        }else{
            QString length = attributes.value("length");

            int nDiskType = -1;
            nDiskType = attributes.value("projecttype").toInt();
            SetProjectTypeFromXML(nDiskType);

            SetData(QDataItem::File, attributes.value("path"),
                    attributes.value("name"), size.toInt(), length.toInt(), 1, 0,
                    hidden.toInt(), hiddenExt.toInt(), archive.toInt(), directory.toInt(), priority.toInt(),
                    QDateTime::fromString(dateadded), QDateTime::fromString(datecreated),
                    QDateTime::fromString(datemodified), attributes.value("checksum"),
                    attributes.value("comment"));
        }

        DoValidate();
    } else if (qName == "datatrack") {
        QString size = attributes.value("size");
        QString item_count = attributes.value("item_count");
        QString node_count = attributes.value("node_count");

        int nDiskType = -1;
        nDiskType = attributes.value("projecttype").toInt();
        SetProjectTypeFromXML(nDiskType);

        SetData(QDataItem::DataTrack, QString(""), attributes.value("name"),
                size.toInt(), item_count.toInt(), node_count.toInt());
        bool folded = (attributes.value("folded") != "no");
        setExpanded(!folded);
    }
}

void QDataItem::SetProjectTypeFromXML(int projectTypeXML)
{
    if(projectTypeXML==-1)return;
    switch(projectTypeXML){
    case 0:
        project_type = RuleManager::TYPE_PROJECT_EXPLORE;
        break;
    case 1:
        project_type = RuleManager::TYPE_PROJECT_OPEN;
        break;
    case 2:
        project_type = RuleManager::TYPE_PROJECT_AUDIOCD;
        break;
    case 3:
        project_type = RuleManager::TYPE_PROJECT_MIXEDMODE;
        break;
    case 4:
        project_type = RuleManager::TYPE_PROJECT_VIDEODVD;
        break;
    case 5:
        project_type = RuleManager::TYPE_PROJECT_BLURAYVIDEO;
        break;
    case 6:
        project_type = RuleManager::TYPE_PROJECT_UDF;
        break;
    case 7:
        project_type = RuleManager::TYPE_PROJECT_ISO;
        break;
    case 8:
        project_type = RuleManager::TYPE_PROJECT_ISOUDF;
        break;
    case 9:
        project_type = RuleManager::TYPE_PROJECT_VIDEOCD;
        break;
    case 10:
        project_type = RuleManager::TYPE_PROJECT_SUPERVIDEO;
        break;
    case 11:
        project_type = RuleManager::TYPE_PROJECT_MAX;
        break;
    }
}

void QDataItem::ParseAndSetDateTime(const SFileEntry &info)
{
    SetDateAdded(ConvertDateTime(info.cCreationTime));
    SetDateCreated(ConvertDateTime(info.cCreationTime));
    SetDateModified(ConvertDateTime(info.cDateTime));
}

QString QDataItem::indent(int depth)
{
    const int IndentSize = 2;
    return QString(IndentSize * depth, ' ');
}

QString QDataItem::escapedText(const QString &str)
{
    QString result = str;
    result.replace("&", "&amp;");
    result.replace("<", "&lt;");
    result.replace(">", "&gt;");
    return result;
}

QString QDataItem::escapedAttribute(const QString &str)
{
    QString result = escapedText(str);
    result.replace("\"", "&quot;");
    result.prepend("\"");
    result.append("\"");
    return result;
}
