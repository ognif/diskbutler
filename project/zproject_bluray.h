#ifndef ZPROJECT_BLURAY_H
#define ZPROJECT_BLURAY_H

#include "CommonTreeWidget.h"

class ZProjectBluRay : public CommonTreeWidget
{
  Q_OBJECT
public:
  ZProjectBluRay(QWidget *parent = 0);

protected:
  void contextMenuEvent(QContextMenuEvent *event);
};

#endif // ZPROJECT_BLURAY_H
