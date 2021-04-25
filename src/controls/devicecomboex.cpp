#include "devicecomboex.h"
#include <QAbstractItemView>

deviceComboEx::deviceComboEx(QWidget * p) : QComboBox(p)
{
}

void deviceComboEx::showPopup()
{
    QComboBox::showPopup();
    QSizePolicy sp = view()->sizePolicy();
    sp.setHorizontalPolicy(QSizePolicy::MinimumExpanding);
    this->view()->setMinimumWidth(300); //change this to see that it alone sets the width
    this->view()->setSizePolicy(sp);
}
