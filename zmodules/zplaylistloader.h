#ifndef ZPLAYLISTLOADER_H
#define ZPLAYLISTLOADER_H

#include <QIcon>
#include "CommonTreeWidget.h"
#include "QDataItem.h"
#include "FoxSDKBurningLib.h"
#include <vector>

class ZPlaylistItem
{
public:
  ZPlaylistItem()
    : mPath(""), mTitle(""), mSeconds(-1), mType(NORM) {}
  ZPlaylistItem(QString path, QString title, int seconds, int type)
    : mPath(path), mTitle(title), mSeconds(seconds), mType(type) {}
  void set(QString path, QString title, int seconds, int type) {
    mPath = path;
    mTitle = title;
    mSeconds = seconds;
    mType = type;
  }

  enum TYPE {
    NORM,
    HTTP
  };

  QString mPath;
  QString mTitle;
  int mSeconds;
  int mType;
};

class ZPlaylistLoader
{
public:
  ZPlaylistLoader(CommonTreeWidget *tree);
  void Load(const QString &playlistPath);

private:
  bool parse(const QString &playlistPath);
  void parseM3U(const QString &playlistPath, QString &all);
  void parsePLS(const QString &playlistPath, QString &all);
  void addItem(const ZPlaylistItem &item);
  void updateTree(const QString &playlistPath);
private:
  CommonTreeWidget *mTree;
  std::vector<ZPlaylistItem> mItemList;
};

#endif
