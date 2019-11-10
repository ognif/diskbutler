#include "zimport_strategy_nosubfolder.h"
#include <QDir>

ZImportStrategyNoSubfolder::ZImportStrategyNoSubfolder(const QString &path)
  : ZImportStrategy(path, ZImportStrategy::TYPE_NOSUBFOLDER)
{
}

ZImportStrategy::ImportError ZImportStrategyNoSubfolder::Check(const QString &root, const QString &path)
{
  ImportError res = ZImportStrategy::ERROR_NO;
  QFileInfo entry(root+path);
  if (entry.isDir()) {
    QString relative_path = entry.canonicalPath();
    relative_path.remove(root);
    //if (relative_path != mPath)
    if (0 != QString::compare(relative_path, mPath, Qt::CaseInsensitive))
      return ZImportStrategy::ERROR_NO;
    res = ERROR_SUBFOLDER_NOT_ALLOWED;
    mErr = QString("Error: subfolder '%1' is not allowed in '%2'\n")
        .arg(entry.fileName()).arg(mPath);
  }
  return res;
}

ZImportStrategy::ImportError ZImportStrategyNoSubfolder::Check2(const QString &folder_name, const QString &path)
{
  ImportError res = ZImportStrategy::ERROR_NO;
  QFileInfo entry(path);
  //if (mPath.endsWith(folder_name)
  if (mPath.endsWith(folder_name, Qt::CaseInsensitive)
      && (path.indexOf("New Folder") != -1 || entry.isDir())) {
    res = ERROR_SUBFOLDER_NOT_ALLOWED;
    mErr = QString("Error: not allowed to add subfolder\n");
  }
  return res;
}
