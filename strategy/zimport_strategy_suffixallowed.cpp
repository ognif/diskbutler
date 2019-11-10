#include "zimport_strategy_suffixallowed.h"
#include <QDir>

ZImportStrategySuffixAllowed::ZImportStrategySuffixAllowed(const QString &path, const QStringList &suffix_list)
  : ZImportStrategy(path, ZImportStrategy::TYPE_SUFFIXALLOWED)
  , mSuffixList(suffix_list)
{
}

ZImportStrategy::ImportError ZImportStrategySuffixAllowed::Check(const QString &root, const QString &path)
{
  ImportError res = ZImportStrategy::ERROR_NO;
  QFileInfo entry(root+path);
  if (entry.isFile()) {
    QString relative_path = entry.canonicalPath();
    relative_path.remove(root);
    //if (relative_path != mPath)
    if (0 != QString::compare(relative_path, mPath, Qt::CaseInsensitive))
      return ZImportStrategy::ERROR_NO;
    bool found = false;
    for (int i = 0; i < mSuffixList.size(); i++) {
      if (entry.suffix() == mSuffixList[i]) {
        found = true;
        break;
      }
    }
    if (!found) {
      res = ERROR_SUFFIX_NOT_ALLOWED;
      mErr = QString("Error: '%1' is not allowed in '%2'\n")
          .arg(entry.fileName()).arg(mPath);
    }
  }
  return res;
}

ZImportStrategy::ImportError ZImportStrategySuffixAllowed::Check2(const QString &folder_name, const QString &path)
{
  ImportError res = ZImportStrategy::ERROR_NO;
  if (mPath.endsWith(folder_name, Qt::CaseInsensitive) && -1 == path.indexOf("New Folder")) {
    QFileInfo entry(path);
    if (entry.isFile()) {
      bool found = false;
      for (int i = 0; i < mSuffixList.size(); i++) {
        if (entry.suffix() == mSuffixList[i]) {
          found = true;
          break;
        }
      }
      if (!found) {
        res = ERROR_SUFFIX_NOT_ALLOWED;
        mErr = QString("Error: '%1' is not allowed in '%2'\n")
            .arg(entry.fileName()).arg(mPath);
      }
    }
  }
  return res;
}
