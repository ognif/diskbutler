#include <QtGui>

#include "configdialog.h"
#include "settingspages.h"

QSettings ConfigurationPage::mSettings("IFoerster", "Diskbutler");

ConfigurationPage::ConfigurationPage(QWidget *parent)
    : QWidget(parent)
{
    QGroupBox *configGroup = new QGroupBox(tr("Diskbutler UI Language"));

    QLabel *serverLabel = new QLabel(tr("Language:"));
    serverCombo = new QComboBox;
    serverCombo->blockSignals(false);
    serverCombo->addItem(tr("Qt (English)"),0);
    serverCombo->addItem(tr("Qt (German)"),1031);

    //QSettings settings("IFoerster", "Diskbutler");
    int tmpLang = mSettings.value("language",0).toInt();

    for(int i=0;i<serverCombo->count();i++){
        if(serverCombo->itemData(i).toInt()==tmpLang){
            serverCombo->setCurrentIndex(i);
            break;
        }
    }

    QHBoxLayout *serverLayout = new QHBoxLayout;
    serverLayout->addWidget(serverLabel);
    serverLayout->addWidget(serverCombo);

    QVBoxLayout *configLayout = new QVBoxLayout;
    configLayout->addLayout(serverLayout);
    configGroup->setLayout(configLayout);

    // Add general configuration
    QGroupBox *generalGroup = new QGroupBox(tr(""));
    QVBoxLayout *vboxLayout = new QVBoxLayout;

    QHBoxLayout *hboxSecLayout = new QHBoxLayout;

    mAddAudioToAudioTrack =  new QRadioButton(tr("Add Audio to Audiotrack"), this);
    mAddAudioToAudioTrack->setChecked(mSettings.value("AudioToAudioTrack", true).toBool());
    mAddAudioToDataTrackToo = new QCheckBox(tr("Add to Datatrack, too "), this);
    mAddAudioToDataTrackToo->setChecked(mSettings.value("AudioToDataTrackToo", false).toBool());
    hboxSecLayout->addWidget(mAddAudioToDataTrackToo);
    hboxSecLayout->insertSpacing(0, 16);
    mAddAudioToDataTrack = new QRadioButton(tr("Add Audio to Datatrack"), this);
    mAddAudioToDataTrack->setChecked(mSettings.value("AudioToDataTrack", !mAddAudioToAudioTrack->isChecked()).toBool());

    connect(mAddAudioToAudioTrack, SIGNAL(clicked(bool)), this, SLOT(on_checkAddAudioToAudioTrack_clicked(bool)));
    connect(mAddAudioToDataTrack, SIGNAL(clicked(bool)), this, SLOT(on_checkAddAudioToDataTrack_clicked(bool)));

    mCreateDataTrackAuto = new QCheckBox(tr("Create data track if not exists"), this);
    mCreateDataTrackAuto->setChecked(mSettings.value("CreateDataTrackAuto", true).toBool());
    mCreateAndShowChecksum = new QCheckBox(tr("Create and show Checksum"), this);
    mCreateAndShowChecksum->setChecked(mSettings.value("CreateAndShowChecksum", true).toBool());
    connect(mCreateAndShowChecksum, SIGNAL(clicked(bool)), this, SLOT(on_checkCreateAndShowChecksum_clicked(bool)));

    QHBoxLayout *checksumHBoxLayout = new QHBoxLayout;
    QLabel *noChecksumFileSizeLabel = new QLabel(tr("No checksum filesize > "));
    mChecksumFileSizeEdit = new QLineEdit(tr(""), this);
    mChecksumFileSizeEdit->setFixedWidth(50);
    mChecksumFileSizeEdit->setText(mSettings.value("NoChecksumFileSize", tr("32")).toString());
    QRegExp regx("[0-9]+$");
    QValidator *validator = new QRegExpValidator(regx, mChecksumFileSizeEdit);
    mChecksumFileSizeEdit->setValidator( validator );
    QLabel *noChecksumFileSizeMBLabel = new QLabel(tr("MB"));
    checksumHBoxLayout->addWidget(noChecksumFileSizeLabel, 0, Qt::AlignLeft);
    checksumHBoxLayout->addWidget(mChecksumFileSizeEdit);
    checksumHBoxLayout->addWidget(noChecksumFileSizeMBLabel);
    checksumHBoxLayout->setContentsMargins(16, -1, 100, -1);

    vboxLayout->addWidget(mAddAudioToAudioTrack);
    vboxLayout->addLayout(hboxSecLayout);
    vboxLayout->addWidget(mAddAudioToDataTrack);
    vboxLayout->addWidget(mCreateDataTrackAuto);
    vboxLayout->addWidget(mCreateAndShowChecksum);
    vboxLayout->addLayout(checksumHBoxLayout);
    generalGroup->setLayout(vboxLayout);

    fsCombo = new QComboBox;
    fsCombo->blockSignals(false);
    fsCombo->addItem(tr("ISO"),1);
    fsCombo->addItem(tr("UDF"), 2);
    fsCombo->addItem(tr("ISO & UDF"), 3);
    int tmpFSType = mSettings.value(tr("FSsyncType"),0).toInt();

    for (int i=0; i<fsCombo->count(); i++) {
        if (fsCombo->itemData(i).toInt()==tmpFSType) {
            fsCombo->setCurrentIndex(i);
            break;
        }
    }
    QGroupBox *fsGroup = new QGroupBox(tr("File System Sync"));
    QHBoxLayout *fsLayout = new QHBoxLayout();
    QLabel *fsLabel = new QLabel(tr("Remove File System on FSSync:"));
    fsLayout->addWidget(fsLabel);
    fsLayout->addWidget(fsCombo);
    fsGroup->setLayout(fsLayout);

    mDefaultProjectCombo = new QComboBox;
    mDefaultProjectCombo->blockSignals(false);
    mDefaultProjectCombo->addItem(tr("None"), -1);
    for (int i = RuleManager::TYPE_PROJECT_OPEN; i < RuleManager::TYPE_PROJECT_MAX; i++) {
      if (RuleManager::TYPE_PROJECT_EXPLORE == i) continue;
      mDefaultProjectCombo->addItem(RuleManager::GetProjectTypeStr((RuleManager::ProjectType)i), i);
    }
    int tmpDefualtType = mSettings.value(tr("DefaultProject"),RuleManager::TYPE_PROJECT_OPEN).toInt();

    for (int i=0; i<mDefaultProjectCombo->count(); i++) {
        if (mDefaultProjectCombo->itemData(i).toInt()==tmpDefualtType) {
            mDefaultProjectCombo->setCurrentIndex(i);
            break;
        }
    }
    QGroupBox *defaultProjectGroup = new QGroupBox(tr("Project Create On Startup"));
    QHBoxLayout *defaultProjectLayout = new QHBoxLayout();
    QLabel *defaultProjectLabel = new QLabel(tr("Projects:"));
    defaultProjectLayout->addWidget(defaultProjectLabel);
    defaultProjectLayout->addWidget(mDefaultProjectCombo);
    defaultProjectGroup->setLayout(defaultProjectLayout);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(configGroup);
    mainLayout->addWidget(generalGroup);
    mainLayout->addWidget(fsGroup);
    mainLayout->addWidget(defaultProjectGroup);
    mainLayout->addStretch(1);
    setLayout(mainLayout);
}

void ConfigurationPage::saveSettings()
{
    //QSettings settings("IFoerster", "Diskbutler");
    mSettings.setValue("language",serverCombo->itemData(serverCombo->currentIndex()));
    mSettings.setValue("AudioToAudioTrack", mAddAudioToAudioTrack->isChecked());
    mSettings.setValue("AudioToDataTrackToo", mAddAudioToDataTrackToo->isChecked());
    mSettings.setValue("AudioToDataTrack", mAddAudioToDataTrack->isChecked());
    mSettings.setValue("CreateDataTrackAuto", mCreateDataTrackAuto->isChecked());
    mSettings.setValue("CreateAndShowChecksum", mCreateAndShowChecksum->isChecked());
    mSettings.setValue("NoChecksumFileSize", mChecksumFileSizeEdit->text());
    mSettings.setValue("FSsyncType", fsCombo->itemData(fsCombo->currentIndex()));
    mSettings.setValue("DefaultProject", mDefaultProjectCombo->itemData(mDefaultProjectCombo->currentIndex()));
    mSettings.sync();
}

void ConfigurationPage::on_checkAddAudioToDataTrack_clicked(bool checked)
{
  if(checked) {
    mAddAudioToAudioTrack->setChecked(false);
  } else {
    mAddAudioToAudioTrack->setChecked(true);
  }
}

void ConfigurationPage::on_checkAddAudioToAudioTrack_clicked(bool checked)
{
  if(checked) {
    mAddAudioToDataTrack->setChecked(false);
  } else {
    mAddAudioToDataTrack->setChecked(true);
  }
}

void ConfigurationPage::on_checkCreateAndShowChecksum_clicked(bool checked)
{
  mChecksumFileSizeEdit->setEnabled(checked);
}

DiskLabelPage::DiskLabelPage(QWidget *parent)
    : QWidget(parent)
{
    QGroupBox *labelGroup = new QGroupBox(tr("Volume ID / Label"));

    QVBoxLayout *vLabelLayout = new QVBoxLayout;
    QHBoxLayout *simpleLine1 = new QHBoxLayout;
    QHBoxLayout *simpleLine2 = new QHBoxLayout;
    QHBoxLayout *customLine1 = new QHBoxLayout;
    QHBoxLayout *customLine2 = new QHBoxLayout;
    QHBoxLayout *customLine3 = new QHBoxLayout;
    QHBoxLayout *customLine4 = new QHBoxLayout;

    mLabelSimple =  new QRadioButton(tr("Simple"), this);
    mLabelSimple->setChecked(ConfigurationPage::mSettings.value("LabelIsSimple", true).toBool());
    connect(mLabelSimple, SIGNAL(clicked(bool)), this, SLOT(on_mLabelSimple_clicked(bool)));

    simpleLine1->addWidget(mLabelSimple);
    vLabelLayout->addLayout(simpleLine1);

    mDiskSimpleName = new QLineEdit(ConfigurationPage::mSettings.value("DefaultDiskName", tr("New")).toString(), this);
    simpleLine2->addWidget(mDiskSimpleName);
    simpleLine2->insertSpacing(0, 16);
    vLabelLayout->addLayout(simpleLine2);

    mLabelCustom =  new QRadioButton(tr("Custome / Pattern"), this);
    mLabelCustom->setChecked(!ConfigurationPage::mSettings.value("LabelIsSimple", true).toBool());
    connect(mLabelCustom, SIGNAL(clicked(bool)), this, SLOT(on_mLabelCustom_clicked(bool)));
    customLine1->addWidget(mLabelCustom);
    vLabelLayout->addLayout(customLine1);

    mDiskCustomPattern = new QLineEdit(ConfigurationPage::mSettings.value("CustomDiskNamePattern", tr("%S-%D-%C")).toString(), this);
    QRegExp regx("^(?!.*(.)\\1)%[%SCD-]*$");
    QValidator *validator = new QRegExpValidator(regx, mDiskCustomPattern);
    mDiskCustomPattern->setValidator( validator );

    mDiskCustomPattern->setMaximumWidth(200);
    customLine2->addWidget(mDiskCustomPattern);
    QLabel *szDummy1 = new QLabel(tr(""));
    customLine2->addWidget(szDummy1);
    customLine2->insertSpacing(0, 16);
    vLabelLayout->addLayout(customLine2);

    mPatternDefaultString = new QLineEdit(ConfigurationPage::mSettings.value("CustomDiskNameString", tr("New")).toString(), this);
    mPatternDefaultString->setMaximumWidth(150);
    customLine3->addWidget(mPatternDefaultString);
    sPatternString = new QLabel(tr("String"));
    customLine3->addWidget(sPatternString);
    customLine3->insertSpacing(0, 16);
    vLabelLayout->addLayout(customLine3);

    mPatternDefaultNumber = new QLineEdit(ConfigurationPage::mSettings.value("CustomDiskNameNumber", tr("001")).toString(), this);
    mPatternDefaultNumber->setMaximumWidth(150);

    mPatternDefaultNumber->setValidator( new QIntValidator(0, 10000000000, this) );
    customLine4->addWidget(mPatternDefaultNumber);
    sPatternNumber = new QLabel(tr("Counting Number"));
    customLine4->addWidget(sPatternNumber);
    customLine4->insertSpacing(0, 16);
    vLabelLayout->addLayout(customLine4);

    labelGroup->setLayout(vLabelLayout);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(labelGroup);


    QLabel *sMaxSize = new QLabel(tr("The disc label has a max. length of 32 characters<br/>"));
    QLabel *sMPatterns = new QLabel(tr("The default pattern is %S-%D-%C<br/>"));
    QLabel *sHowTo = new QLabel(tr("%D = Date (yyyy-mm-dd)<br/>%S = String / Text<br/>%C = Counter (Increase Counter)"));
    mainLayout->addWidget(sMaxSize);
    mainLayout->addWidget(sMPatterns);
    mainLayout->addWidget(sHowTo);

    mainLayout->addStretch(1);
    setLayout(mainLayout);

    if(ConfigurationPage::mSettings.value("LabelIsSimple", true).toBool()==true)
    {
        switchGroup(true);
    }else{
        switchGroup(false);
    }
}

void DiskLabelPage::on_mLabelSimple_clicked(bool checked)
{
    switchGroup(true);
}

void DiskLabelPage::on_mLabelCustom_clicked(bool checked){
    switchGroup(false);
}

void DiskLabelPage::switchGroup(bool isSimple)
{
    if(isSimple==true){
        mDiskSimpleName->setEnabled(true);
        mDiskCustomPattern->setEnabled(false);
        mPatternDefaultString->setEnabled(false);
        mPatternDefaultNumber->setEnabled(false);
        sPatternNumber->setEnabled(false);
        sPatternString->setEnabled(false);
    }else{
        mDiskSimpleName->setEnabled(false);
        mDiskCustomPattern->setEnabled(true);
        mPatternDefaultString->setEnabled(true);
        mPatternDefaultNumber->setEnabled(true);
        sPatternNumber->setEnabled(true);
        sPatternString->setEnabled(true);
    }

}

void DiskLabelPage::saveSettings()
{
    //QSettings settings("IFoerster", "Diskbutler");

    ConfigurationPage::mSettings.setValue("LabelIsSimple", mLabelSimple->isChecked()); //True = Simple
    ConfigurationPage::mSettings.setValue("DefaultDiskName", mDiskSimpleName->text());

    ConfigurationPage::mSettings.setValue("CustomDiskNamePattern", mDiskCustomPattern->text());
    ConfigurationPage::mSettings.setValue("CustomDiskNameString", mPatternDefaultString->text());
    ConfigurationPage::mSettings.setValue("CustomDiskNameNumber", mPatternDefaultNumber->text());

    ConfigurationPage::mSettings.sync();
}

MessagePage::MessagePage(QWidget *parent)
    : QWidget(parent)
{
    QGroupBox *messageGroup = new QGroupBox(tr("Show following Messages"));

    listWidget = new QListWidget(this);

        QSettings settings("IFoerster", "Diskbutler");
        myMessage1 = new QCheckBox(this);
        myMessage1->setChecked(settings.value("item1",true).toBool());
        QListWidgetItem *item1 = new QListWidgetItem();
        myMessage1->setText(tr("Show project file error"));
        listWidget->addItem(item1);
        listWidget->setItemWidget(item1,myMessage1);

        myMessage2 = new QCheckBox(this);
        myMessage2->setChecked(settings.value("item2",true).toBool());
        QListWidgetItem *item2 = new QListWidgetItem();
        myMessage2->setText(tr("Show save error"));
        listWidget->addItem(item2);
        listWidget->setItemWidget(item2,myMessage2);

        myMessage3 = new QCheckBox(this);
        myMessage3->setChecked(settings.value("warn_exceed_74min",true).toBool());
        QListWidgetItem *item3 = new QListWidgetItem();
        myMessage3->setText(tr("Warn if audio track exceed 74 minute total time."));
        listWidget->addItem(item3);
        listWidget->setItemWidget(item3,myMessage3);
        /*------------------------------------------------------------------------
         * It was planed to add this after adding items. But it is too much.
         * So we will add to another location.
         *     if(totalTime>4440){
         *      if(ConfigurationPage::mSettings.value("warn_exceed_74min",true).toBool()==true){
         *        QMessageBox::information(this, tr("Warning"),tr("The audio playtime exceed 74 Min, the normal capability of a CD media."));
         *      }
         *     }
         * -----------------------------------------------------------------------*/


    QHBoxLayout *messageHLayout = new QHBoxLayout;
    messageHLayout->addWidget(listWidget);

    QVBoxLayout *messageVLayout = new QVBoxLayout;
    messageVLayout->addLayout(messageHLayout);
    messageGroup->setLayout(messageVLayout);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(messageGroup);
    mainLayout->addStretch(1);
    setLayout(mainLayout);
}

void MessagePage::saveSettings()
{
    QSettings settings("IFoerster", "Diskbutler");
    settings.setValue("item1",myMessage1->isChecked());
    settings.setValue("item2",myMessage2->isChecked());
    settings.setValue("item3",myMessage3->isChecked());
    settings.sync();

}

ISOExPage::ISOExPage(QWidget *parent)
    : QWidget(parent)
{

   QSettings settings("IFoerster", "Diskbutler");
   mEditSystemId = new QLineEdit(settings.value("systemId","").toString());
   mEditSystemId->setMinimumWidth(280);
   mEditVolumeSet = new QLineEdit(settings.value("volumeSet","").toString());
   mEditVolumeSet->setMinimumWidth(280);
   mEditPublisher = new QLineEdit(settings.value("publisher","").toString());
   mEditPublisher->setMinimumWidth(280);
   mEditDatapreparer = new QLineEdit(settings.value("datapreparer","").toString());
   mEditDatapreparer->setMinimumWidth(280);
   mEditApp = new QLineEdit(settings.value("app","").toString());
   mEditApp->setMinimumWidth(280);
   mEditCorightFile = new QLineEdit(settings.value("corightfile","").toString());
   mEditCorightFile->setMinimumWidth(280);
   mEditAbstractFile = new QLineEdit(settings.value("abstractfile","").toString());
   mEditAbstractFile->setMinimumWidth(280);
   mEditBibliographicFile = new QLineEdit(settings.value("bibliographicfile","").toString());
   mEditBibliographicFile->setMinimumWidth(280);

  //Textfelder in der Länge beschreiben
  //Labels rechts ausrichten
   //Er denkt das das so klein ist warum?

  QFormLayout *formLayout = new QFormLayout;
  formLayout->setLabelAlignment(Qt::AlignLeft | Qt::AlignVCenter);
  formLayout->addRow("System Identifier:", mEditSystemId);
  formLayout->addRow("Volume Set:", mEditVolumeSet);
  formLayout->addRow("Publisher:", mEditPublisher);
  formLayout->addRow("Datapreparer:", mEditDatapreparer);
  formLayout->addRow("Application:", mEditApp);
  formLayout->addRow("Copright File:", mEditCorightFile);
  formLayout->addRow("Abstract File:", mEditAbstractFile);
  formLayout->addRow("Bibliographic File:", mEditBibliographicFile);
  setLayout(formLayout);
}

void ISOExPage::saveSettings() {
  QSettings settings("IFoerster", "Diskbutler");

  settings.setValue("systemId",mEditSystemId->text());
  settings.setValue("volumeSet",mEditVolumeSet->text());
  settings.setValue("publisher",mEditPublisher->text());
  settings.setValue("datapreparer",mEditDatapreparer->text());
  settings.setValue("app",mEditApp->text());
  settings.setValue("corightfile",mEditCorightFile->text());
  settings.setValue("abstractfile",mEditAbstractFile->text());
  settings.setValue("bibliographicfile",mEditBibliographicFile->text());

  settings.sync();
}

FilterPage::FilterPage(QWidget *parent)
    : QWidget(parent)
{
  QSettings settings("IFoerster", "Diskbutler");
  QGroupBox *groupByType = new QGroupBox("By Type:");
  QGroupBox *groupByDate = new QGroupBox("");

  QFont font;
  font.setFamily("Font Awesome 5 Free");
  font.setPixelSize(16);

  //by type
  QVBoxLayout *btnlayout = new QVBoxLayout;
  mButtonAdd = new QPushButton("");
  mButtonRemove = new QPushButton("");

  mButtonAdd->setFont(font);
  mButtonRemove->setFont(font);
  mButtonAdd->setText("\uf055");
  mButtonRemove->setText("\uf056");

  btnlayout->addWidget(mButtonAdd, 0, Qt::AlignBottom);
  btnlayout->addWidget(mButtonRemove, 0, Qt::AlignTop);
  //btnlayout->setGeometry(QRect(0, 0, 100, 60));

  QHBoxLayout *typeLayout = new QHBoxLayout;
  mListWidget = new QListWidget();
  typeLayout->addWidget(mListWidget, 5);
  typeLayout->addLayout(btnlayout, 1);
  typeLayout->setAlignment(btnlayout, Qt::AlignTop);
  groupByType->setLayout(typeLayout);
  int filterCount = settings.value("filtercount", 0).toInt();
  for (int i=0; i<filterCount; i++) {
    QListWidgetItem *item = new QListWidgetItem(settings.value("filteritem"+i, "").toString());
    mListWidget->addItem(item);
    item->setFlags(item->flags() | Qt::ItemIsEditable);
  }

  //by date
  mCheckByDate = new QCheckBox("By Date:");
  mCheckByDate->setChecked(settings.value("bydate", false).toBool());
  QVBoxLayout *dateLayout = new QVBoxLayout();
  QHBoxLayout *timeLayout = new QHBoxLayout;
  QLabel *labelForm = new QLabel("From:");
  mDateFrom = new QDateEdit(settings.value("datefrom").toDate());
  QLabel *labelTo = new QLabel("To:");
  mDateTo = new QDateEdit(settings.value("dateto").toDate());
  timeLayout->addWidget(labelForm);
  timeLayout->addWidget(mDateFrom);
  timeLayout->addWidget(labelTo);
  timeLayout->addWidget(mDateTo);
  dateLayout->addWidget(mCheckByDate, 1);
  dateLayout->addLayout(timeLayout, 1);
  groupByDate->setLayout(dateLayout);

  QVBoxLayout *mainlayout = new QVBoxLayout;
  mainlayout->addWidget(groupByType, 3);
  mainlayout->addWidget(groupByDate, 1);

  connect(mButtonAdd, SIGNAL(clicked()), this, SLOT(on_mButtonAdd_clicked()));
  connect(mButtonRemove, SIGNAL(clicked()), this, SLOT(on_mButtonRemove_clicked()));
  connect(mCheckByDate, SIGNAL(clicked(bool)), this, SLOT(on_checkByDate_clicked(bool)));

  on_checkByDate_clicked(mCheckByDate->isChecked());
  setLayout(mainlayout);
}

void FilterPage::saveSettings() {
  QSettings settings("IFoerster", "Diskbutler");

  settings.setValue("bydate",mCheckByDate->isChecked());
  settings.setValue("datefrom", mDateFrom->date());
  settings.setValue("dateto", mDateTo->date());

  int filterCount = mListWidget->count();
  for (int i=0; i<filterCount; i++) {
    settings.setValue("filteritem"+i, mListWidget->item(i)->text());
  }
  settings.setValue("filtercount", mListWidget->count());

  settings.sync();
}

void FilterPage::on_mButtonAdd_clicked() {
  QListWidgetItem *item = new QListWidgetItem("");
  mListWidget->insertItem(0,item);
  item->setFlags(item->flags() | Qt::ItemIsEditable);
  mListWidget->setCurrentItem(item);
  mListWidget->editItem(item);
}

void FilterPage::on_mButtonRemove_clicked() {
  mListWidget->takeItem(mListWidget->currentRow());
}

void FilterPage::on_checkByDate_clicked(bool checked) {
  mDateFrom->setEnabled(checked);
  mDateTo->setEnabled(checked);

  mListWidget->setEnabled(!checked);
  mButtonAdd->setEnabled(!checked);
  mButtonRemove->setEnabled(!checked);
}

/*
  Combobox wechsel


  */
