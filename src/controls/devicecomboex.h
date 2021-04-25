#ifndef DEVICECOMBOEX_H
#define DEVICECOMBOEX_H

#include <QWidget>
#include <QComboBox>

class deviceComboEx : public QComboBox
{
    Q_OBJECT
public:
    explicit deviceComboEx(QWidget *parent = nullptr);
    void showPopup();
};

#endif // DEVICECOMBOEX_H
