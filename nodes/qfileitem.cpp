#include "qfileitem.h"
#include <QApplication>
#include <QFileInfo>
#include <QFileIconProvider>
#include <QTemporaryFile>
#include <QDir>

QFileItem* QFileItem::create(QDataItem *parent, const HSESSION hSession, const SFileEntry& entry)
{
  return new QFileItem(parent, hSession, entry);
}

QFileItem::QFileItem(QDataItem *parent, const HSESSION hSession, const SFileEntry& entry)
  : QDataItem(parent)
  , mInfo(entry)
{
  SetType(QDataItem::File);
  SetDefaultIcon();
  ParseGenericInfo();
  ParseAllocationTable(hSession);
}

QIcon fileExtensionIcon(const QString & extension) {
    static QFileIconProvider iconProvider;

    QIcon icon;

    QTemporaryFile tmpFile(QDir::tempPath() + QDir::separator() +
QCoreApplication::applicationName() + "_XXXXXX." + extension);
    tmpFile.setAutoRemove(false);

    if (tmpFile.open()) {
        QString fileName = tmpFile.fileName();
        tmpFile.write(QByteArray());
        tmpFile.close();

        icon = iconProvider.icon(QFileInfo(fileName));

        tmpFile.remove();
    }


    return icon;
}

void QFileItem::ParseGenericInfo()
{

#if defined (WIN32)
    SetName(QString::fromUtf16((const ushort*)mInfo.lpszFileName));
#else
    SetName(QString::fromUtf8(mInfo.lpszFileName));
#endif

  SetText0(GetName());

  QFileInfo fileinfo(GetName());
  QIcon icon = fileExtensionIcon(fileinfo.suffix());
  setIcon(0, icon);

  //info.lba = m_info.nAddress;
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

  SetDataItemCount(1);
}

void QFileItem::ParseAllocationTable(const HSESSION hSession)
{
  //const SFileAllocationTable *allocTable = ((FileNode*)m_treeNode)->getAllocationTable();
  int32 tableSize = 0;
  TCHAR *pBuf = new TCHAR[256];
#if defined (WIN32)
      GetFullPath().fromUtf16(pBuf);//QString::fromWCharArray(pBuf);
#else
      GetFullPath().fromUtf8(pBuf);//QString::fromWCharArray(pBuf);
#endif
  pBuf[GetFullPath().length()] = _T('\0');

  int32 errCode = ::GetFileAllocationTable(hSession, pBuf, NULL, &tableSize);
  if (errCode != BS_SDK_ERROR_MORE_SPACE_NEEDED)
    return;
  SFileAllocationTable *allocTable = (SFileAllocationTable*)malloc(tableSize);

  errCode = ::GetFileAllocationTable(hSession, pBuf, allocTable, &tableSize);
  if (errCode == BS_SDK_ERROR_NO) {
    for (int i = 0; i < allocTable->NumExtents; i++) {
      mLocationVector.push_back(allocTable->Extents[i].Location);
      mLengthVector.push_back(allocTable->Extents[i].Length);
    }
  }
  delete pBuf;
  free(allocTable);
}

QString QFileItem::createAttributeString()
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
