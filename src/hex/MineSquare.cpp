#include <QPainter>
#include "MineSquare.h"

MineSquare::MineSquare(int i, int j, QWidget *parent) :
    QPushButton(parent) ,
    mRowIndex(i) ,
    mColIndex(j) ,
    mined(false)
{
    setFixedSize(30,30);
    //setWindowOpacity(0.5);
}

