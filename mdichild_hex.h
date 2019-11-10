#ifndef MDICHILD_HEX_H
#define MDICHILD_HEX_H

#include <QMainWindow>
#include <QtWidgets>
#include <QListView>
#include <QSplitter>
#include <QLabel>
#include "mdichild_base.h"
#include "FoxSDKBurningLib.h"
#include "vrulemanager.h"
#include "QHexView.h"

typedef std::vector<char> SectorData_t;

class MdiChildHex : public MdiChildBase
{
    Q_OBJECT

private:
    QDoubleSpinBox *editSectors;
    QPushButton *firstButton;
    QPushButton *nextButton;
    QPushButton *backButton;
    QPushButton *lastButton;
    QPushButton *updateButton;
    QPushButton *saveButton;

    void setControls(bool isFull);

public:
    MdiChildHex(QWidget* parent, const QString &device);
    RuleManager::ProjectType GetProjectType() {return mProjectType;}


    QString getBurnDrive() {return strBurnDrive;}
    void setBurnDrive(QString strValue) {
      strBurnDrive=strValue;
      QString wTitle = tr("Device Info");
      wTitle += " (";
      wTitle += strBurnDrive;
      wTitle += ")";
      setWindowTitle(wTitle);
    }

private slots:
    void setFirstSector();
    void setLastSector();
    void nextSector();
    void prevSector();
    void updateHex();
    void saveBlock();

protected:
    QHexView *hexView;
    RuleManager::ProjectType mProjectType;

    QString strBurnDrive;
    void readDeviceInfo();

    int32 ReadSector(int32 sectorIndex) const;

    double maxSectors;
    double currentSector;
    int bufferSize;

};

#endif
