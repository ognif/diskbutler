#include "zfileiconprovider.h"
#include <QFileInfo>
#include <QIcon>

ZFileIconProvider::ZFileIconProvider() :
  QFileIconProvider()
{
}

QIcon ZFileIconProvider::icon(const QFileInfo &info) const
{
  QString path = info.fileName();
  if (".." == path) {
    return QIcon(":/images/up.png");
  } else {
    return QFileIconProvider::icon(info);
  }
}
