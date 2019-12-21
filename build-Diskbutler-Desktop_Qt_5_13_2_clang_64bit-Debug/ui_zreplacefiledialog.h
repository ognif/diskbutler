/********************************************************************************
** Form generated from reading UI file 'zreplacefiledialog.ui'
**
** Created by: Qt User Interface Compiler version 5.13.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_ZREPLACEFILEDIALOG_H
#define UI_ZREPLACEFILEDIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QLabel>

QT_BEGIN_NAMESPACE

class Ui_ZReplaceFileDialog
{
public:
    QDialogButtonBox *buttonBox;
    QLabel *label;
    QCheckBox *checkBox;

    void setupUi(QDialog *ZReplaceFileDialog)
    {
        if (ZReplaceFileDialog->objectName().isEmpty())
            ZReplaceFileDialog->setObjectName(QString::fromUtf8("ZReplaceFileDialog"));
        ZReplaceFileDialog->resize(381, 149);
        buttonBox = new QDialogButtonBox(ZReplaceFileDialog);
        buttonBox->setObjectName(QString::fromUtf8("buttonBox"));
        buttonBox->setGeometry(QRect(210, 84, 151, 32));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);
        label = new QLabel(ZReplaceFileDialog);
        label->setObjectName(QString::fromUtf8("label"));
        label->setGeometry(QRect(20, 14, 341, 61));
        label->setWordWrap(true);
        checkBox = new QCheckBox(ZReplaceFileDialog);
        checkBox->setObjectName(QString::fromUtf8("checkBox"));
        checkBox->setGeometry(QRect(20, 90, 171, 20));

        retranslateUi(ZReplaceFileDialog);
        QObject::connect(buttonBox, SIGNAL(accepted()), ZReplaceFileDialog, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), ZReplaceFileDialog, SLOT(reject()));

        QMetaObject::connectSlotsByName(ZReplaceFileDialog);
    } // setupUi

    void retranslateUi(QDialog *ZReplaceFileDialog)
    {
        ZReplaceFileDialog->setWindowTitle(QCoreApplication::translate("ZReplaceFileDialog", "Dialog", nullptr));
        label->setText(QString());
        checkBox->setText(QCoreApplication::translate("ZReplaceFileDialog", "Same action for following files", nullptr));
    } // retranslateUi

};

namespace Ui {
    class ZReplaceFileDialog: public Ui_ZReplaceFileDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_ZREPLACEFILEDIALOG_H
