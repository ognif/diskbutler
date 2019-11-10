#ifndef MDICHILD_BASE_H
#define MDICHILD_BASE_H

#include <QMainWindow>
#include "vrulemanager.h"

class MdiChildBase : public QMainWindow
{
  Q_OBJECT

public:
  MdiChildBase();
  virtual ~MdiChildBase() {}
  virtual QString updateStatus() {return "";}
  virtual QString getListViewRootPath() const {return "";}
  virtual RuleManager::ProjectType GetProjectType() {return RuleManager::TYPE_PROJECT_OPEN;}
  virtual QString getBurnDrive() {return "";}
  virtual void setBurnDrive(QString strValue) {}
};

#endif
