#include "qfolderitem.h"
#include "qfileitem.h"

QFolderItem* QFolderItem::create(QDataItem *parent, HSESSION hSession
                                 , const SFileEntry& entry
                                 , int &countFile, int &countFolder)
{
  return new QFolderItem(parent, hSession, entry, countFile, countFolder);
}

QFolderItem::QFolderItem(QDataItem *parent, HSESSION hSession
                         , const SFileEntry& entry
                         , int &countFile, int &countFolder)
  : QDataItem(parent)
  , mHSession(hSession)
  , mInfo(entry)
{
  SetType(QDataItem::Folder);
  SetDefaultIcon();
  ParseGenericInfo();
  CreateAndLoadChildren(countFile, countFolder);
}

void QFolderItem::ParseGenericInfo()
{
#if defined (WIN32)
    SetName(QString::fromUtf16((const ushort*)mInfo.lpszFileName));
#else
    SetName(QString::fromUtf8(mInfo.lpszFileName));
#endif

  SetText0(GetName());
  //info.lba = mInfo.nAddress;
  qlonglong nTemp = (qlonglong)mInfo.nFileSize;
  SetDataSize(nTemp,true);
  nTemp = (qlonglong)mInfo.nAddress;
  SetDataLBA(nTemp);
  ParseAndSetDateTime(mInfo);
#if defined (WIN32)
  SetFullPath(QString::fromUtf16((const ushort*)mInfo.lpszFilePath)
              + PATHSEPSTRING
              + QString::fromUtf16((const ushort*)mInfo.lpszFileName));
#else
  SetFullPath(QString::fromUtf8(mInfo.lpszFilePath)
              + PATHSEPSTRING
              + QString::fromUtf8(mInfo.lpszFileName));
#endif

}

void QFolderItem::CreateAndLoadChildren(int &countFile, int &countFolder)
{
  //setExpanded(true);

  HDIR hDir = 0;
#if defined (WIN32)
    int res = ::OpenDirectory(mHSession, (const TCHAR*)GetFullPath().toUtf16(), &hDir);
#else
    int res = ::OpenDirectory(mHSession, (const TCHAR*)GetFullPath().toUtf8(), &hDir);
#endif
  if (res != BS_SDK_ERROR_NO) {
    return;
  }

  countFile = 0;
  countFolder = 0;
  for (int i = 0; ; ++i) {
    SFileEntry entry;
    res = ::ReadDirectory(hDir, i, &entry);
    if (res != BS_SDK_ERROR_NO)
      break;

    if (entry.nAttrib & BS_FA_DIRECTORY) {
      int tempCountFile = 0, tempCountFolder = 0;
      QFolderItem::create(this, mHSession, entry, tempCountFile, tempCountFolder);
      countFile += tempCountFile;
      countFolder += tempCountFolder;
    } else {
      QFileItem::create(this, mHSession, entry);
      countFile += 1;
    }
  }
  SetDataItemCount(countFile);
  SetDataNodeCount(countFolder);
  countFolder += 1;
  ::CloseDirectory(hDir);
}

QString QFolderItem::createAttributeString()
{
    QString res;
    int32 attr = mInfo.nAttrib;

    if (attr & BS_FA_ARCHIVE)
    {
        res += "A ";
    }

    if (attr & BS_FA_HIDDEN)
    {
        res += "H ";
    }

    if (attr & BS_FA_ADVANCED_HIDDEN)
    {
        res += "H+ ";
    }

    if (attr & BS_FA_READONLY)
    {
        res += "R ";
    }

    if (attr & BS_FA_SYSTEM)
    {
        res += "S ";
    }

    return res.trimmed();
}
