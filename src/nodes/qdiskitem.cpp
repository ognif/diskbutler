/*
 *  DiskButler - a powerful CD/DVD/BD recording software tool for Linux, macOS and Windows.
 *  Copyright (c) 2021 Ingo Foerster (pixbytesl@gmail.com).
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License 3 as published by
 *  the Free Software Foundation; either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License 3 for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include "qdiskitem.h"
#include <QtWidgets>
#include "CommonTreeWidget.h"


QDiskItem::QDiskItem(CommonTreeWidget *view)
  : QDataItem(view)
  , mArranger("")
  , mComposer("")
  , mSongWriter("")
  , mPerformer("")
  , mAudioMessage("")
  , mAudioTitle("")
  , mUPCEAN("")
  , mPause("")
  , mSystemId("")
  , mVolumeSet("")
  , mPublisher("")
  , mDatapreparer("")
  , mApplication("")
  , mCoprightFile("")
  , mAbstractFile("")
  , mBibliographicFile("")
  , mByDate(false)
  , isAllowedBootDisk(false)
  , doBootDisk(false)
  , mBootDiskFile("")
  , mBootDeveloperID("FoxSuite")
  , mBootSectors("1")
  , mBootLoadSegment("7C0")
  , mBootEmulationType(2)
  , mBootPlatformID(0)
  , useDiskDates(0)
  , isAllowedSimulate(false)
  , useSimulate(false)
  , useBurnProof(true)
  , useEjectAfterBurn(true)
  , isAllowedOPC(true)
  , useOPC(true)
  , isAllowedFinishDisk(false)
  , useFinishDisk(false)
  , isAllowedVerify(true)
  , useVerify(false)
  , useAutoErase(true)
  , usePadDataTrack(false)
  , isAllowedPadDataTrack(false)
  , isAllowedAVCHD(false)
  , useAVCDH(false)
  , mISO1Extension(0)
  , mISOFsType(2)
  , mISOJoliet(1)
  , mISORockRidge(0)
  , mISOLowerCaseNames(0)
  , mISOManyDirectories(1)
  , mISOLongFilesNames(1)
  , mISOLongJolietFilesNames(1)
  , mUDFType(0)
  , mUDFPartition(0)
  , mUDFWriteStream(0)
  , strDrive("")

{
  project_type = view->GetProjectType();
  SetType(QDataItem::Disk);
  mFilterList = new QStringList();
  mDateFrom = QDate::currentDate().toString();
  mDateTo = QDate::currentDate().toString();

  //We do not have created a project in this state. And we do not have added any file.
  //So we can only took this information out the sttings page. Not yet implemented.
  mSystemId = ConfigurationPage::mSettings.value("systemId","").toString();
  mVolumeSet = ConfigurationPage::mSettings.value("volumeSet","").toString();
  mPublisher = ConfigurationPage::mSettings.value("publisher","").toString();
  mDatapreparer = ConfigurationPage::mSettings.value("datapreparer","").toString();
  mApplication = ConfigurationPage::mSettings.value("app","").toString();
  mCoprightFile = ConfigurationPage::mSettings.value("corightfile","").toString();
  mAbstractFile = ConfigurationPage::mSettings.value("abstractfile","").toString();
  mBibliographicFile = ConfigurationPage::mSettings.value("bibliographicfile","").toString();

  //Get Features from project
  isAllowedOPC = RuleManager::IsOptionAllowed(project_type, RuleManager::OPTION_SETTINGS_OPC);
  isAllowedFinishDisk = RuleManager::IsOptionAllowed(project_type, RuleManager::OPTION_SETTINGS_FINALIZE);
  isAllowedVerify = RuleManager::IsOptionAllowed(project_type, RuleManager::OPTION_SETTINGS_VERIFY);
  isAllowedPadDataTrack = RuleManager::IsOptionAllowed(project_type, RuleManager::OPTION_SETTINGS_PADDATATRACKS);
  isAllowedAVCHD = RuleManager::IsOptionAllowed(project_type, RuleManager::OPTION_SPECIALS_AVCHD);
  isAllowedSimulate = RuleManager::IsOptionAllowed(project_type, RuleManager::OPTION_SETTINGS_SIMULATE);

  //Is allowed UDF or ISO
  isAllowedUDF  = RuleManager::IsOptionAllowed(project_type, RuleManager::OPTION_FILESYSTEMS_UDF);
  isAllowedISO  = RuleManager::IsOptionAllowed(project_type, RuleManager::OPTION_FILESYSTEMS_ISO9660);

  //Joliet is a "Default" so we enable it automatically if allowed.
  mISOJoliet  = RuleManager::IsOptionAllowed(project_type, RuleManager::OPTION_FILESYSTEMS_JOLIET);

  strDrive = ConfigurationPage::mSettings.value("CurrentBurnDevice","").toString();

  mDateExpiration = QDateTime::currentDateTime();
  mDateEffective = QDateTime::currentDateTime();
  mDateCreation = QDateTime::currentDateTime();
  mDateMdification = QDateTime::currentDateTime();

}

QString QDiskItem::ToXMLElement(int depth)
{
  QString out = "";

  //QMessageBox::information(NULL,"Information",QString::number(depth));
  //No Semikolon is allowed, The semikolon is the final line
  int iProjectType = (int)project_type;

  out += indent(depth)
      + "<disk name=" + escapedAttribute(text(0))
      + " projecttype=" + escapedAttribute(QString::number(iProjectType))
      + " filter=" + escapedAttribute(encodeFilterList(mFilterList))
      + " bydate=" + escapedAttribute(mByDate?"true":"false")
      + " datefrom=" + escapedAttribute(mDateFrom)
      + " dateto=" + escapedAttribute(mDateTo)
      + " dateExpiration=" + escapedAttribute(mDateExpiration.toString("yyyy-MM-dd hh:mm:ss"))
      + " dateEffective=" + escapedAttribute(mDateEffective.toString("yyyy-MM-dd hh:mm:ss"))
      + " dateCreation=" + escapedAttribute(mDateCreation.toString("yyyy-MM-dd hh:mm:ss"))
      + " dateMdification=" + escapedAttribute(mDateMdification.toString("yyyy-MM-dd hh:mm:ss"))
      + ">\n";

  QString options = "";
  options += indent(depth+1)
      + "<Options optionSimulate=" + escapedAttribute(useSimulate?"true":"false")
      + " optionEject=" + escapedAttribute(useEjectAfterBurn?"true":"false")
      + " optionOPC=" + escapedAttribute(useOPC?"true":"false")
      + " optionFinalize=" + escapedAttribute(useFinishDisk?"true":"false")
      + " optionVerify=" + escapedAttribute(useVerify?"true":"false")
      + " optionAutoErase=" + escapedAttribute(useAutoErase?"true":"false")
      + " optionPadDataTrack=" + escapedAttribute(usePadDataTrack?"true":"false")
      + " optionAVCHD=" + escapedAttribute(useAVCDH?"true":"false")
      + " optionBurnProof=" + escapedAttribute(useBurnProof?"true":"false")
      + "/>\n";

  QString fileSystem = "";
  fileSystem += indent(depth+1)
      + "<FileSystem fsISOFsType=" + escapedAttribute(QString::number(mISOFsType))
      + " fsISOJoliet=" + escapedAttribute(QString::number(mISOJoliet))
      + " fsISORockRidge=" + escapedAttribute(QString::number(mISORockRidge))
      + " fsISOLowerCaseNames=" + escapedAttribute(QString::number(mISOLowerCaseNames))
      + " fsISOManyDirectories=" + escapedAttribute(QString::number(mISOManyDirectories))
      + " fsISOLongFilesNames=" + escapedAttribute(QString::number(mISOLongFilesNames))
      + " fsISOLongJolietFilesNames=" + escapedAttribute(QString::number(mISOLongJolietFilesNames))
      + " fsUDFType=" + escapedAttribute(QString::number(mUDFType))
      + " fsUDFPartition=" + escapedAttribute(QString::number(mUDFPartition))
      + " fsUDFWriteStream=" + escapedAttribute(QString::number(mUDFWriteStream))
      + " fsISO1Extendsion=" + escapedAttribute(QString::number(mISO1Extension))
      + "/>\n";

  QString isoExtension = "";
  isoExtension += indent(depth+1)
      + "<IsoExtension systemid=" + escapedAttribute(mSystemId)
      + " volumeset=" + escapedAttribute(mVolumeSet)
      + " publisher=" + escapedAttribute(mPublisher)
      + " datapreparer=" + escapedAttribute(mDatapreparer)
      + " application=" + escapedAttribute(mApplication)
      + " coprightfile=" + escapedAttribute(mCoprightFile)
      + " abstractfile=" + escapedAttribute(mAbstractFile)
      + " bibliographicfile=" + escapedAttribute(mBibliographicFile)
      + "/>\n";

  QString boot = "";
  boot += indent(depth+1)
      + "<Boot bootUse=" + escapedAttribute(doBootDisk?"true":"false")
      + " bootDiskFile=" + escapedAttribute(mBootDiskFile)
      + " bootDeveloperID=" + escapedAttribute(mBootDeveloperID)
      + " bootSectors=" + escapedAttribute(mBootSectors)
      + " bootLoadSegment=" + escapedAttribute(mBootLoadSegment)
      + " bootEmulationType=" + escapedAttribute(QString::number(mBootEmulationType))
      + " bootPlatformID=" + escapedAttribute(QString::number(mBootPlatformID))
      + "/>\n";

  QString diskCDText = "";
  diskCDText += indent(depth+1)
      + "<DiskCDText arranger=" + escapedAttribute(mArranger)
      + " composer=" + escapedAttribute(mComposer)
      + " songwriter=" + escapedAttribute(mSongWriter)
      + " performer=" + escapedAttribute(mPerformer)
      + " message=" + escapedAttribute(mAudioMessage)
      + " title=" + escapedAttribute(mAudioTitle)
      + " upcean=" + escapedAttribute(mUPCEAN)
      + " pause=" + escapedAttribute(mPause)
      + "/>\n";

     /*Options we use from rules
    isAllowedUDF
    isAllowedISO
    isAllowedAVCHD
    isAllowedPadDataTrack
    isAllowedVerify
    isAllowedFinishDisk
    isAllowedOPC
    isAllowedSimulate
    isAllowedBootDisk
    */

  out += options + fileSystem + isoExtension + boot + diskCDText + indent(depth) + "</disk>\n";
  return out;
}

void QDiskItem::FromXMLElement(const QString &qName, const QXmlAttributes &attributes)
{
  // Values are all QString, we need to convert to numbers or bool
  if ("disk" == qName) {
    parseFilterList(attributes.value("filter"));
    setByDate((attributes.value("bydate")=="true")?true:false);
    setDateFrom(attributes.value("datefrom"));
    setDateTo(attributes.value("dateto"));

    setDiskDateExpiration(QDateTime::fromString(attributes.value("dateExpiration"),"yyyy-MM-dd hh:mm:ss"));
    setDiskDateEffective(QDateTime::fromString(attributes.value("dateEffective"),"yyyy-MM-dd hh:mm:ss"));
    setDiskDateCreation(QDateTime::fromString(attributes.value("dateCreation"),"yyyy-MM-dd hh:mm:ss"));
    setDateMdification(QDateTime::fromString(attributes.value("dateMdification"),"yyyy-MM-dd hh:mm:ss"));
  }

  if ("Options" == qName) {
    setFeatureOPC((attributes.value("optionOPC")=="true")?true:false);
    setFeatureAVCHD((attributes.value("optionAVCHD")=="true")?true:false);
    setFeatureSimulate((attributes.value("optionSimulate")=="true")?true:false);
    setFeatureFinishDisk((attributes.value("optionFinalize")=="true")?true:false);
    setFeatureEject((attributes.value("optionEject")=="true")?true:false);
    setFeatureBurnProof((attributes.value("optionBurnProof")=="true")?true:false);
    setFeatureVerify((attributes.value("optionVerify")=="true")?true:false);
    setFeatureAutoErase((attributes.value("optionAutoErase")=="true")?true:false);
    setFeaturePadDataTrack((attributes.value("optionPadDataTrack")=="true")?true:false);
  }

  if ("FileSystem" == qName) {
    setISOFsType(attributes.value("fsISOFsType").toInt());
    setUseJoliet(attributes.value("fsISOJoliet").toInt());
    setUseRockRidge(attributes.value("fsISORockRidge").toInt());
    setAllowLowerCaseNames(attributes.value("fsISOLowerCaseNames").toInt());
    setAllowManyDirectories(attributes.value("fsISOManyDirectories").toInt());
    setAllowLongFileNames(attributes.value("fsISOLongFilesNames").toInt());
    setAllowLongJolietFileNames(attributes.value("fsISOLongJolietFilesNames").toInt());
    setUDFType(attributes.value("fsUDFType").toInt());
    setUDFPartition(attributes.value("fsUDFPartition").toInt());
    setUDFWriteStream(attributes.value("fsUDFWriteStream").toInt());
    setNotWriteISO1Extension(attributes.value("fsISO1Extendsion").toInt());
  }

  if ("IsoExtension" == qName) {
    setSystemId(attributes.value("systemid"));
    setVolumeSet(attributes.value("volumeset"));
    setPublisher(attributes.value("publisher"));
    setDatapreparer(attributes.value("datapreparer"));
    setApplication(attributes.value("application"));
    setCoprightFile(attributes.value("coprightfile"));
    setAbstractFile(attributes.value("abstractfile"));
    setBibliographicFile(attributes.value("bibliographicfile"));
  }

  if ("Boot" == qName) {
    setDoBootDisk((attributes.value("bootUse")=="true")?true:false);
    setBootDiskFile(attributes.value("bootDiskFile"));
    setBootDeveloperID(attributes.value("bootDeveloperID"));
    setBootSectors(attributes.value("bootSectors"));
    setBootLoadSegment(attributes.value("bootLoadSegment"));
    setBootEmulationType(attributes.value("bootEmulationType").toInt());
    setBootPlatformID(attributes.value("bootPlatformID").toInt());
  }

  if ("DiskCDText" == qName) {
    setArranger(attributes.value("arranger"));
    setComposer(attributes.value("composer"));
    setSongWriter(attributes.value("songwriter"));
    setPerformer(attributes.value("performer"));
    setMessage(attributes.value("message"));
    setTitle(attributes.value("title"));
    setUPCEAN(attributes.value("upcean"));
    setPause(attributes.value("pause"));
  }
}

void QDiskItem::setFilterList(QStringList *list)
{
  mFilterList->clear();
  for (int i=0; i<list->count(); i++) {
    mFilterList->insert(i, list->at(i));
  }
}

QString QDiskItem::encodeFilterList(QStringList *list)
{
  QString str = "";
  for (int i=0; i<list->count(); i++) {
    str +=list->at(i)+";";
  }
  return str;
}

void QDiskItem::parseFilterList(const QString str)
{
  int count = str.count(";");
  for (int i=0; i<count; i++) {
    mFilterList->insert(i, str.section(";", i, i));
  }
}

bool QDiskItem::isFilterSuffix(QString suffix)
{
  for (int i=0; i<mFilterList->count(); i++) {
    if (0 == suffix.compare(mFilterList->at(i)))
      return true;
  }
  return false;
}
