#ifndef MINESQUARE_H
#define MINESQUARE_H

#include <QPushButton>
#include <QMouseEvent>

class MineSquare : public QPushButton {

    Q_OBJECT
public:
    MineSquare(int i = 0, int j = 0, QWidget *parent = 0);
    void setIndex(int i, int j)
    {
        mRowIndex = i;
        mColIndex = j;
    }
    void setMined(bool r=true) { mined = r; }

private:
    int mRowIndex;
    int mColIndex;
    bool mined;
};
#endif//MINESQUARE_H
