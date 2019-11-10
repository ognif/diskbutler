#ifndef QAUDIOTRACKITEM_H
#define QAUDIOTRACKITEM_H

#include "QDataItem.h"

class QAudioTrackItem : public QDataItem
{
  //Q_OBJECT
public:
  explicit QAudioTrackItem(QDataItem *parent = 0);
  virtual QString ToXMLElement(int depth);
  virtual void FromXMLElement(const QString &qName, const QXmlAttributes &attributes);

  QString getArranger() {return mArranger;}
  QString getComposer() {return mComposer;}
  QString getSongWriter() {return mSongWriter;}
  QString getPerformer() {return mPerformer;}
  QString getMessage() {return mAudioMessage;}
  QString getTitle() {return mAudioTitle;}
  QString getUPCEAN() {return mUPCEAN;}
  int getPause() {return mPause;}
  bool getPauseInFrames() {return bPauseInFrames;}

  void setArranger(QString str) {mArranger = str;}
  void setComposer(QString str) {mComposer = str;}
  void setSongWriter(QString str) {mSongWriter = str;}
  void setPerformer(QString str) {mPerformer = str;}
  void setMessage(QString str) {mAudioMessage = str;}
  void setTitle(QString str) {mAudioTitle = str;}
  void setUPCEAN(QString str) {mUPCEAN = str;}
  void setPause(int value) {mPause = value;}
  void setPauseInFrames(bool value) {bPauseInFrames = value;}
  void resetAll();

  std::vector<int32> getIndexes() {return mIndexes;}
  void addIndex(int minutes, int seconds, int frames);
  void removeIndex(int i);
  const int32* getPointerIndexes();
  int32 getIndexesLength();
signals:

public slots:

private:
  QString mArranger;
  QString mComposer;
  QString mSongWriter;
  QString mPerformer;
  QString mAudioMessage;
  QString mAudioTitle;
  QString mUPCEAN;
  int mPause;
  bool bPauseInFrames;
  //const int32* mIndexes;
  //int32 mIndexesLength;
  std::vector<int32> mIndexes;
};

#endif // QAUDIOTRACKITEM_H
