#include <QtWidgets>
#include <QRegularExpression>

#include "zplaylistloader.h"
#include "utils_common.h"

ZPlaylistLoader::ZPlaylistLoader(CommonTreeWidget *tree)
  : mTree(tree)
{
  mItemList.reserve(16);
}

void ZPlaylistLoader::Load(const QString &playlistPath)
{
  parse(playlistPath);
  updateTree(playlistPath);
}

bool ZPlaylistLoader::parse(const QString &playlistPath)
{
  QFile file(playlistPath);
  if (!file.open(QFile::ReadOnly | QFile::Text))
    return false;

  QTextStream in(&file);
  QString all = in.readAll();
  if (playlistPath.endsWith(".m3u")) {
    parseM3U(playlistPath, all);
  } else if (playlistPath.endsWith(".pls")) {
    parsePLS(playlistPath, all);
  }
  return true;
}

void ZPlaylistLoader::parseM3U(const QString &playlistPath, QString &all)
{
  QFileInfo fileinfo(playlistPath);
  QString baseDir = fileinfo.absoluteDir().path();
  int ext = all.indexOf("#EXTM3U");
  if (ext != -1)
    all = all.mid(ext+7);
  QRegularExpression re("(#EXTINF:([-]*\\d+),(.+)\n|)((http://|[a-zA-Z]:|\\w)(.+))", QRegularExpression::CaseInsensitiveOption);
  QRegularExpressionMatchIterator i = re.globalMatch(all);
  while (i.hasNext()) {
    QRegularExpressionMatch match = i.next();
    QString type = match.captured(5);
    int seconds = -1;
    QString title = "";
    if (match.captured(1).length() > 0) {
      seconds = match.captured(2).toInt();
      title = match.captured(3);
    }
    ZPlaylistItem item;
    if (type.startsWith("http:")) {
      item.set(match.captured(4), title, seconds, ZPlaylistItem::HTTP);
    } else if (type.contains(':')) {
      item.set(match.captured(4), title, seconds, ZPlaylistItem::NORM);
    } else {
      item.set(baseDir + "/" + match.captured(4), title, seconds, ZPlaylistItem::NORM);
    }
    //mItemList.push_back(item);
    addItem(item);
  }
}

void ZPlaylistLoader::parsePLS(const QString &playlistPath, QString &all)
{
  QFileInfo fileinfo(playlistPath);
  QString baseDir = fileinfo.absoluteDir().path();
  QRegularExpression re("File(\\d+)=((http://|[a-zA-Z]:|\\w)(.+))\nTitle\\1=(.+)\nLength\\1=([-]*\\d+)", QRegularExpression::CaseInsensitiveOption);
  QRegularExpressionMatchIterator i = re.globalMatch(all);
  while (i.hasNext()) {
    QRegularExpressionMatch match = i.next();
    QString type = match.captured(3);
    int seconds = match.captured(6).toInt();
    QString title = match.captured(5);
    ZPlaylistItem item;
    if (type.startsWith("http:")) {
      item.set(match.captured(2), title, seconds, ZPlaylistItem::HTTP);
    } else if (type.contains(':')) {
      item.set(match.captured(2), title, seconds, ZPlaylistItem::NORM);
    } else {
      item.set(baseDir + "/" + match.captured(2), title, seconds, ZPlaylistItem::NORM);
    }
    //mItemList.push_back(item);
    addItem(item);
  }
}

void ZPlaylistLoader::addItem(const ZPlaylistItem &item)
{
  if (ZPlaylistItem::NORM == item.mType
      && (item.mPath.endsWith(".m3u") || item.mPath.endsWith("pls"))) {
    parse(item.mPath);
  } else {
    mItemList.push_back(item);
  }
}

void ZPlaylistLoader::updateTree(const QString &playlistPath)
{
  if (RuleManager::TYPE_PROJECT_AUDIOCD != mTree->GetProjectType()
      && RuleManager::TYPE_PROJECT_OPEN != mTree->GetProjectType()
      && RuleManager::TYPE_PROJECT_MIXEDMODE != mTree->GetProjectType()) {
    // import play list file self
    mTree->InsertItem(playlistPath);
  }

  foreach (ZPlaylistItem item, mItemList) {
    QFileInfo fileInfo(item.mPath);

    // Test status bar
    /*
    if (!fileInfo.isFile() && getMainWindow() != NULL) {
      getMainWindow()->addStatusBarIcon(QPixmap(":/res/deletesmall.png"), "invalid audio: " + item.mPath);
    }
    if (ZPlaylistItem::HTTP == item.mType && getMainWindow() != NULL) {
      getMainWindow()->addStatusBarIcon(QPixmap(":/res/closeapp.png"), "not support http audio: " + item.mPath);
    }
    */
    // Test status bar end

    if (ZPlaylistItem::HTTP == item.mType || !fileInfo.isFile()) {
      continue;
    }
    mTree->InsertPlaylistItem(item.mPath);
  }

  mItemList.clear();
  // Test status bar
  //getMainWindow()->removeStatusBarIcon(0);
  // Test status bar end
}
