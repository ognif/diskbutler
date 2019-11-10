#ifndef ZIMAGELOADER_H
#define ZIMAGELOADER_H

#include <QIcon>
#include "CommonTreeWidget.h"
#include "FoxSDKBurningLib.h"

class ZImageLoader
{
public:
  ZImageLoader(CommonTreeWidget *treeWidget);
  bool Parse(const QString &imagePath, bool isImage = true);

private:
  CommonTreeWidget *mTreeWidget;
  //QIcon mFolderIcon;
  //QIcon mFileIcon;
};

#endif
