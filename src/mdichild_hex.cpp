#include <QtWidgets>
#include "mdichild_hex.h"
#include "FoxSDKBurningLib.h"
#include "mainwindow.h"
#include "settingspages.h"

MdiChildHex::MdiChildHex(QWidget* parent, const QString &device)
    :strBurnDrive(device){
    setAttribute(Qt::WA_DeleteOnClose);
    mProjectType = RuleManager::TYPE_PROJECT_DEVICEINFO;

    hexView = new QHexView();

    QString wTitle = tr("Hex ");
    wTitle += " (";
    wTitle += strBurnDrive;
    wTitle += ")";

    setWindowTitle(wTitle);

    QVBoxLayout *dlgSplitter = new QVBoxLayout(this);
    QHBoxLayout * topNav = new QHBoxLayout(this);

    QLabel *labelTopNav = new QLabel(this);
    labelTopNav->setText(tr("Sector: "));

    editSectors = new QDoubleSpinBox;
    editSectors->setMinimumWidth(150);
    editSectors->setMaximumWidth(150);
    editSectors->setRange(0, 2048.0);
    editSectors->setSingleStep(1.0);
    editSectors->setValue(0.0);
    editSectors->setDecimals(0); //No need to abstract int64 spinbox
    connect(editSectors, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
        [=](double d){ ReadSector(d);});

    QSpacerItem *item = new QSpacerItem(1,1, QSizePolicy::Expanding, QSizePolicy::Fixed);

    QFont font;
    font.setFamily("Font Awesome 5 Free");
    font.setPixelSize(16);

    updateButton = new QPushButton();
    updateButton->setFont(font);
    updateButton->setToolTip(tr("Read again the selected drive"));
    updateButton->setText("\uf2f1");
    connect(updateButton, SIGNAL(clicked()), this, SLOT(updateHex()));

    firstButton = new QPushButton();
    firstButton->setFont(font);
    firstButton->setToolTip(tr("Jump to first sector"));
    firstButton->setText("\uf100");
    connect(firstButton, SIGNAL(clicked()), this, SLOT(setFirstSector()));

    nextButton = new QPushButton();
    nextButton->setFont(font);
    nextButton->setToolTip(tr("Go to the next sector"));
    nextButton->setText("\uf105");
    connect(nextButton, SIGNAL(clicked()), this, SLOT(nextSector()));

    backButton = new QPushButton();
    backButton->setFont(font);
    backButton->setToolTip(tr("Go to the previous sector"));
    backButton->setText("\uf104");
    connect(backButton, SIGNAL(clicked()), this, SLOT(prevSector()));

    lastButton = new QPushButton();
    lastButton->setFont(font);
    lastButton->setToolTip(tr("Jump to the last sector"));
    lastButton->setText("\uf101");
    connect(lastButton, SIGNAL(clicked()), this, SLOT(setLastSector()));

    saveButton = new QPushButton();
    saveButton->setFont(font);
    saveButton->setToolTip(tr("Save sector to text file"));
    saveButton->setText("\uf0c7");
    connect(saveButton, SIGNAL(clicked()), this, SLOT(saveBlock()));

    topNav->addWidget(labelTopNav);
    topNav->addWidget(editSectors);
    topNav->addWidget(updateButton);
    topNav->addWidget(saveButton);

    topNav->addSpacerItem(item);
    topNav->addWidget(firstButton);
    topNav->addWidget(backButton);
    topNav->addWidget(nextButton);
    topNav->addWidget(lastButton);

    QWidget * wdgSplitter = new QWidget(this);

    topNav->setContentsMargins(1,8,1,0);
    dlgSplitter->setContentsMargins(0,0,0,0);
    dlgSplitter->setSpacing(0);
    dlgSplitter->setMargin(0);

    dlgSplitter->addLayout(topNav);
    dlgSplitter->addWidget(hexView);
    wdgSplitter->setLayout(dlgSplitter);

    setCentralWidget(wdgSplitter);

    updateHex();

}

int32 MdiChildHex::ReadSector(int32 sectorIndex) const
{
    char *buf = new char[2352];
    QByteArray mdata;

    qDebug("readSector %d",sectorIndex);

    int32 ret = ::ReadSectors(BS_READ_DEVICE, sectorIndex, 1, BS_IMG_ISO, buf, bufferSize);

    if (ret != BS_SDK_ERROR_NO){

        return ret;
    }

    QByteArray byteArray = QByteArray::fromRawData(buf, bufferSize);

    hexView->clear();

    hexView->setData(new QHexView::DataStorageArray(byteArray));


    //delete[] buf;

    return ret;
}

void MdiChildHex::setControls(bool isFull)
{
    editSectors->setEnabled(!isFull);
    firstButton->setEnabled(!isFull);
    nextButton->setEnabled(!isFull);
    backButton->setEnabled(!isFull);
    lastButton->setEnabled(!isFull);
    saveButton->setEnabled(!isFull);
}

void MdiChildHex::readDeviceInfo()
{

}

void MdiChildHex::saveBlock()
{
    QString fileName = QFileDialog::getSaveFileName(
                    this,
                    tr("Save Sector "),
                    QDir::currentPath(),
                    tr("Text File (*.txt)") );

      if (fileName.isEmpty()){
          QMessageBox::information(this, tr("Information"),
                                   tr("No file selected"));
          return;
      }

    hexView->saveTofile(fileName);
}

void MdiChildHex::setFirstSector()
{
    editSectors->setValue(0);
}

void MdiChildHex::setLastSector()
{
    editSectors->setValue(maxSectors);

}

void MdiChildHex::nextSector()
{
    double cSector = editSectors->value();
    cSector++;
    editSectors->setValue(cSector);
}

void MdiChildHex::prevSector()
{
    double cSector = editSectors->value();
    cSector--;
    editSectors->setValue(cSector);
}

void MdiChildHex::updateHex()
{
    //OK, the UI was set, now we need to read the sectors to bytearray.
    int32 ret = ::SetReadDevice(strBurnDrive.at(0).toLatin1());
    if (ret != BS_SDK_ERROR_NO){
        QMessageBox::information(this, tr("Information"),
                                 tr("FoxSDK Error Device"));
        return;
    }

    //We need here threading function with isDeviceReady call.

    bool bIsEmptyDisk = false;
    SMediumInfo mi;
    ret = ::GetMediumInformation(BS_READ_DEVICE, &mi);

    if (ret != BS_SDK_ERROR_NO){
        QMessageBox::information(this, tr("Information"),
                                 tr("FoxSDK Error Get Mediainfo"));
        return;
    }

    if(mi.nMediumStatus==BS_MS_EMPTY_DISK){
        bIsEmptyDisk = true;
    }

    setControls(bIsEmptyDisk);

    if(bIsEmptyDisk==false){
        bufferSize = 2048;
        if(mi.nExtendedMediumType==BS_EMT_CD_AUDIO){
            bufferSize = 2352;
        }
        //Disable all controls. Default text possible for control?
        maxSectors = mi.dMediumUsedSize / bufferSize;
        editSectors->setRange(0, maxSectors);
        editSectors->setValue(0);

        ret = ReadSector(0);
        if (ret != BS_SDK_ERROR_NO){
            QMessageBox::information(this, tr("Information"),
                                 tr("FoxSDK Error Read Sector"));
        }
    }

}


