/********************************************************************************
** Form generated from reading UI file 'zpropertydialogfoxfile.ui'
**
** Created by: Qt User Interface Compiler version 5.13.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_ZPROPERTYDIALOGFOXFILE_H
#define UI_ZPROPERTYDIALOGFOXFILE_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QTableView>

QT_BEGIN_NAMESPACE

class Ui_ZPropertyDialogFoxFile
{
public:
    QDialogButtonBox *buttonBox;
    QGroupBox *groupBoxGeneral;
    QTableView *infoView;
    QGroupBox *groupBoxTable;
    QTableView *tableView;

    void setupUi(QDialog *ZPropertyDialogFoxFile)
    {
        if (ZPropertyDialogFoxFile->objectName().isEmpty())
            ZPropertyDialogFoxFile->setObjectName(QString::fromUtf8("ZPropertyDialogFoxFile"));
        ZPropertyDialogFoxFile->resize(367, 422);
        buttonBox = new QDialogButtonBox(ZPropertyDialogFoxFile);
        buttonBox->setObjectName(QString::fromUtf8("buttonBox"));
        buttonBox->setGeometry(QRect(180, 385, 171, 32));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);
        groupBoxGeneral = new QGroupBox(ZPropertyDialogFoxFile);
        groupBoxGeneral->setObjectName(QString::fromUtf8("groupBoxGeneral"));
        groupBoxGeneral->setGeometry(QRect(20, 10, 331, 220));
        infoView = new QTableView(groupBoxGeneral);
        infoView->setObjectName(QString::fromUtf8("infoView"));
        infoView->setGeometry(QRect(15, 20, 301, 190));
        groupBoxTable = new QGroupBox(ZPropertyDialogFoxFile);
        groupBoxTable->setObjectName(QString::fromUtf8("groupBoxTable"));
        groupBoxTable->setGeometry(QRect(20, 240, 331, 141));
        tableView = new QTableView(groupBoxTable);
        tableView->setObjectName(QString::fromUtf8("tableView"));
        tableView->setGeometry(QRect(15, 20, 301, 111));

        retranslateUi(ZPropertyDialogFoxFile);
        QObject::connect(buttonBox, SIGNAL(accepted()), ZPropertyDialogFoxFile, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), ZPropertyDialogFoxFile, SLOT(reject()));

        QMetaObject::connectSlotsByName(ZPropertyDialogFoxFile);
    } // setupUi

    void retranslateUi(QDialog *ZPropertyDialogFoxFile)
    {
        ZPropertyDialogFoxFile->setWindowTitle(QCoreApplication::translate("ZPropertyDialogFoxFile", "Dialog", nullptr));
        groupBoxGeneral->setTitle(QCoreApplication::translate("ZPropertyDialogFoxFile", "Information", nullptr));
        groupBoxTable->setTitle(QCoreApplication::translate("ZPropertyDialogFoxFile", "Allocation Table", nullptr));
    } // retranslateUi

};

namespace Ui {
    class ZPropertyDialogFoxFile: public Ui_ZPropertyDialogFoxFile {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_ZPROPERTYDIALOGFOXFILE_H
