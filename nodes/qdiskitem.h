#ifndef QDISKITEM_H
#define QDISKITEM_H

#include "QDataItem.h"
#include <QList>
#include "vrulemanager.h"
#include "settingspages.h"

class QDiskItem : public QDataItem
{
public:
  explicit QDiskItem(CommonTreeWidget *view);

  virtual QString ToXMLElement(int depth);
  virtual void FromXMLElement(const QString &qName, const QXmlAttributes &attributes);

  RuleManager::ProjectType getProjectType() {return project_type;}

  QString getDisktitle() {return text(0);}

  QString getArranger() {return mArranger;}
  QString getComposer() {return mComposer;}
  QString getSongWriter() {return mSongWriter;}
  QString getPerformer() {return mPerformer;}
  QString getMessage() {return mAudioMessage;}
  QString getTitle() {return mAudioTitle;}
  QString getUPCEAN() {return mUPCEAN;}
  QString getPause() {return mPause;}
  QString getBurnDevice() {return strDrive;}
  QStringList getBurnDevices() {return strDrives;}
  void clearDrives() {strDrives.clear();}
  void appendDrives(QString str) {strDrives.append(str);}

  QString getSystemId() {return mSystemId;}
  QString getVolumeSet() {return mVolumeSet;}
  QString getPublisher() {return mPublisher;}
  QString getDatapreparer() {return mDatapreparer;}
  QString getApplication() {return mApplication;}
  QString getCoprightFile() {return mCoprightFile;}
  QString getAbstractFile() {return mAbstractFile;}
  QString getBibliographicFile() {return mBibliographicFile;}
  QStringList* getFilterList() {return mFilterList;}
  bool getByDate() {return mByDate;}
  QString getDateFrom() {return mDateFrom;}
  QString getDateTo() {return mDateTo;}


  int getISOFsType() {return mISOFsType;}
  int getUseJoliet() {return mISOJoliet;}
  int getUseRockRidge() {return mISORockRidge;}
  int getAllowLowerCaseNames() {return mISOLowerCaseNames;}
  int getAllowManyDirectories() {return mISOManyDirectories;}
  int getAllowLongFileNames() {return mISOLongFilesNames;}
  int getAllowLongJolietFileNames() {return mISOLongJolietFilesNames;}
  int getUDFType() {return mUDFType;}
  int getUDFPartition() {return mUDFPartition;}
  int getUDFWriteStream() {return mUDFWriteStream;}
  int getNotWriteISO1Extension() {return mISO1Extension;}

  QDateTime getDiskDateExpiration() {return mDateExpiration;}
  QDateTime getDiskDateEffective() {return mDateEffective;}
  QDateTime getDiskDateCreation() {return mDateCreation;}
  QDateTime getDiskDateMdification() {return mDateMdification;}

  bool getIsAllowedBootDisk() {return isAllowedBootDisk;}
  bool getDoBootDisk() {return doBootDisk;}
  QString getBootDiskFile() {return mBootDiskFile;}
  QString getBootDeveloperID() {return mBootDeveloperID;}
  QString getBootSectors() {return mBootSectors;}
  QString getBootLoadSegment() {return mBootLoadSegment;}
  int getBootEmulationType() {return mBootEmulationType;}
  int getBootPlatformID() {return mBootPlatformID;}

  void setArranger(QString str) {mArranger = str;}
  void setComposer(QString str) {mComposer = str;}
  void setSongWriter(QString str) {mSongWriter = str;}
  void setPerformer(QString str) {mPerformer = str;}
  void setMessage(QString str) {mAudioMessage = str;}
  void setTitle(QString str) {mAudioTitle = str;}
  void setUPCEAN(QString str) {mUPCEAN = str;}
  void setPause(QString str) {mPause = str;}
  void setBurnDevice(QString str) {strDrive = str;}

  void setSystemId(QString str) {mSystemId = str;}
  void setVolumeSet(QString str) {mVolumeSet = str;}
  void setPublisher(QString str) {mPublisher = str;}
  void setDatapreparer(QString str) {mDatapreparer = str;}
  void setApplication(QString str) {mApplication = str;}
  void setCoprightFile(QString str) {mCoprightFile = str;}
  void setAbstractFile(QString str) {mAbstractFile = str;}
  void setBibliographicFile(QString str) {mBibliographicFile = str;}
  void setFilterList(QStringList *list);
  void setByDate(bool bChecked) {mByDate = bChecked;}
  void setDateFrom(QString str) {mDateFrom = str;}
  void setDateTo(QString str) {mDateTo = str;}

  void setIsAllowedBootDisk(bool value) {isAllowedBootDisk=value;}
  void setDoBootDisk(bool value) {doBootDisk=value;}
  void setBootDiskFile(QString str) {mBootDiskFile=str;}
  void setBootDeveloperID(QString str) {mBootDeveloperID=str;}
  void setBootSectors(QString str) {mBootSectors=str;}
  void setBootLoadSegment(QString str) {mBootLoadSegment=str;}
  void setBootEmulationType(int value) {mBootEmulationType=value;}
  void setBootPlatformID(int value) {mBootPlatformID=value;}

  void setISOFsType(int value) {mISOFsType=value;}
  void setUseJoliet(int value) {mISOJoliet=value;}
  void setUseRockRidge(int value) {mISORockRidge=value;}
  void setAllowLowerCaseNames(int value) {mISOLowerCaseNames=value;}
  void setAllowManyDirectories(int value) {mISOManyDirectories=value;}
  void setAllowLongFileNames(int value) {mISOLongFilesNames=value;}
  void setAllowLongJolietFileNames(int value) {mISOLongJolietFilesNames=value;}
  void setUDFType(int value) {mUDFType=value;}
  void setUDFPartition(int value) {mUDFPartition=value;}
  void setUDFWriteStream(int value) {mUDFWriteStream=value;}
  void setNotWriteISO1Extension(int value) {mISO1Extension=value;}



  bool isFeatureSimulate() {return isAllowedSimulate;}
  bool isFeatureOPC() {return isAllowedOPC;}
  bool isFeatureFinishDisk() {return isAllowedFinishDisk;}
  bool isFeatureAVCHD() {return isAllowedAVCHD;}
  bool isFeaturePadDataTrack() {return isAllowedPadDataTrack;}

  bool getFeatureOPC(){return useOPC;}
  bool getFeatureAVCHD(){return useAVCDH;}
  bool getFeatureSimulate(){return useSimulate;}
  bool getFeatureFinishDisk(){return useFinishDisk;}
  bool getFeatureEject(){return useEjectAfterBurn;}
  bool getFeatureBurnProof(){return useBurnProof;}
  bool getFeatureVerify(){return useVerify;}
  bool getFeatureAutoErase(){return useAutoErase;}
  bool getFeaturePadDataTrack(){return usePadDataTrack;}

  void setFeatureOPC(bool bState){useOPC=bState;}
  void setFeatureAVCHD(bool bState){useAVCDH=bState;}
  void setFeatureSimulate(bool bState){useSimulate=bState;}
  void setFeatureFinishDisk(bool bState){useFinishDisk=bState;}
  void setFeatureEject(bool bState){useEjectAfterBurn=bState;}
  void setFeatureBurnProof(bool bState){useBurnProof=bState;}
  void setFeatureVerify(bool bState){useVerify=bState;}
  void setFeatureAutoErase(bool bState){useAutoErase=bState;}
  void setFeaturePadDataTrack(bool bState){usePadDataTrack=bState;}

  void setDiskDateExpiration(QDateTime date) {mDateExpiration = date;}
  void setDiskDateEffective(QDateTime date) {mDateEffective = date;}
  void setDiskDateCreation(QDateTime date) {mDateCreation = date;}
  void setDateMdification(QDateTime date) {mDateMdification = date;}



  bool isFilterSuffix(QString suffix);
private:
  RuleManager::ProjectType project_type;
  //disk
  QDateTime mDateExpiration;
  QDateTime mDateEffective;
  QDateTime mDateCreation;
  QDateTime mDateMdification;




  //filter
  QStringList *mFilterList;
  //CD Text
  QString mArranger;
  QString mComposer;
  QString mSongWriter;
  QString mPerformer;
  QString mAudioMessage;
  QString mAudioTitle;
  QString mUPCEAN;
  QString mPause;
  //ISO Ex
  QString mSystemId;
  QString mVolumeSet;
  QString mPublisher;
  QString mDatapreparer;
  QString mApplication;
  QString mCoprightFile;
  QString mAbstractFile;
  QString mBibliographicFile;
  bool mByDate;
  QString mDateFrom;
  QString mDateTo;

  //we need a bool for "isAllowed" and a value for the feature/setting

  bool isAllowedBootDisk;
  bool doBootDisk;
  QString mBootDiskFile;
  QString mBootDeveloperID;
  QString mBootSectors;
  QString mBootLoadSegment;
  int mBootEmulationType;
  int mBootPlatformID;

  int nCopies;
  int nSpeed;

  bool isAllowedSimulate;
  bool useSimulate;

  bool useBurnProof;
  bool useEjectAfterBurn;

  bool isAllowedOPC;
  bool useOPC;

  bool isAllowedFinishDisk;
  bool useFinishDisk;

  bool isAllowedVerify;
  bool useVerify;

  bool useAutoErase;

  bool isAllowedPadDataTrack;
  bool usePadDataTrack;

  bool isAllowedAVCHD;
  bool useAVCDH;

  bool isAllowedUDF;
  bool isAllowedISO;

  int mISO1Extension;
  int mISOFsType;
  int mISOJoliet;
  int mISORockRidge;
  int mISOLowerCaseNames;
  int mISOManyDirectories;
  int mISOLongFilesNames;
  int mISOLongJolietFilesNames;
  int mUDFType;
  int mUDFPartition;
  int mUDFWriteStream;

  QString strDrive;
  QStringList strDrives;

  QString encodeFilterList(QStringList *list);
  void parseFilterList(const QString str);
};

#endif // QDISKITEM_H
