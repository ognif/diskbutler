#ifndef ZPROJECT_VIDEODVD_H
#define ZPROJECT_VIDEODVD_H

#include "CommonTreeWidget.h"

class ZProjectVideoDVD : public CommonTreeWidget
{
  Q_OBJECT
public:
  ZProjectVideoDVD(QWidget *parent = 0);

protected:
  void contextMenuEvent(QContextMenuEvent *event);
};

#endif // ZPROJECT_VIDEODVD_H
