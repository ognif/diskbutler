/********************************************************************************
** Form generated from reading UI file 'vpropertydialog.ui'
**
** Created by: Qt User Interface Compiler version 5.13.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_VPROPERTYDIALOG_H
#define UI_VPROPERTYDIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDateTimeEdit>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_VPropertyDialog
{
public:
    QDialogButtonBox *buttonBox;
    QLabel *imageWarningDateCreated;
    QLabel *imageWarningDateModified;
    QLabel *imageWarningSize;
    QWidget *formLayoutWidget;
    QFormLayout *formLayout;
    QLabel *label;
    QLineEdit *editName;
    QLabel *label_2;
    QLabel *labelSource;
    QLabel *label_7;
    QLabel *labelPath;
    QLabel *label_11;
    QLabel *labelContent;
    QLabel *label_3;
    QDateTimeEdit *dateTimeCreated;
    QLabel *label_4;
    QDateTimeEdit *dateTimeAdded;
    QLabel *label_5;
    QDateTimeEdit *dateTimeModified;
    QLabel *label_6;
    QLabel *labelSize;
    QLabel *label_10;
    QComboBox *comboPriority;
    QLabel *headChecksum;
    QLabel *labelChecksum;
    QLabel *headComment;
    QLineEdit *editComment;
    QCheckBox *checkHiddenExt;
    QCheckBox *checkVideoStream;
    QCheckBox *checkHidden;
    QLabel *label_8;
    QLabel *labelDateChangedError;
    QLabel *labelDateCreatedError;
    QPushButton *buttonSync;

    void setupUi(QDialog *VPropertyDialog)
    {
        if (VPropertyDialog->objectName().isEmpty())
            VPropertyDialog->setObjectName(QString::fromUtf8("VPropertyDialog"));
        VPropertyDialog->resize(432, 426);
        buttonBox = new QDialogButtonBox(VPropertyDialog);
        buttonBox->setObjectName(QString::fromUtf8("buttonBox"));
        buttonBox->setGeometry(QRect(210, 390, 210, 32));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);
        imageWarningDateCreated = new QLabel(VPropertyDialog);
        imageWarningDateCreated->setObjectName(QString::fromUtf8("imageWarningDateCreated"));
        imageWarningDateCreated->setGeometry(QRect(130, 410, 16, 16));
        imageWarningDateCreated->setMinimumSize(QSize(16, 16));
        imageWarningDateCreated->setMaximumSize(QSize(16, 16));
        imageWarningDateModified = new QLabel(VPropertyDialog);
        imageWarningDateModified->setObjectName(QString::fromUtf8("imageWarningDateModified"));
        imageWarningDateModified->setGeometry(QRect(160, 410, 16, 16));
        imageWarningDateModified->setMinimumSize(QSize(16, 16));
        imageWarningDateModified->setMaximumSize(QSize(16, 16));
        imageWarningSize = new QLabel(VPropertyDialog);
        imageWarningSize->setObjectName(QString::fromUtf8("imageWarningSize"));
        imageWarningSize->setGeometry(QRect(200, 410, 16, 16));
        imageWarningSize->setMinimumSize(QSize(16, 16));
        imageWarningSize->setMaximumSize(QSize(16, 16));
        formLayoutWidget = new QWidget(VPropertyDialog);
        formLayoutWidget->setObjectName(QString::fromUtf8("formLayoutWidget"));
        formLayoutWidget->setGeometry(QRect(10, 10, 411, 372));
        QSizePolicy sizePolicy(QSizePolicy::Maximum, QSizePolicy::Preferred);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(formLayoutWidget->sizePolicy().hasHeightForWidth());
        formLayoutWidget->setSizePolicy(sizePolicy);
        formLayout = new QFormLayout(formLayoutWidget);
        formLayout->setObjectName(QString::fromUtf8("formLayout"));
        formLayout->setFieldGrowthPolicy(QFormLayout::ExpandingFieldsGrow);
        formLayout->setRowWrapPolicy(QFormLayout::WrapLongRows);
        formLayout->setContentsMargins(0, 0, 0, 0);
        label = new QLabel(formLayoutWidget);
        label->setObjectName(QString::fromUtf8("label"));
        label->setMaximumSize(QSize(16777215, 20));

        formLayout->setWidget(0, QFormLayout::LabelRole, label);

        editName = new QLineEdit(formLayoutWidget);
        editName->setObjectName(QString::fromUtf8("editName"));
        QSizePolicy sizePolicy1(QSizePolicy::Expanding, QSizePolicy::Fixed);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(editName->sizePolicy().hasHeightForWidth());
        editName->setSizePolicy(sizePolicy1);

        formLayout->setWidget(0, QFormLayout::FieldRole, editName);

        label_2 = new QLabel(formLayoutWidget);
        label_2->setObjectName(QString::fromUtf8("label_2"));
        label_2->setMaximumSize(QSize(16777215, 20));

        formLayout->setWidget(1, QFormLayout::LabelRole, label_2);

        labelSource = new QLabel(formLayoutWidget);
        labelSource->setObjectName(QString::fromUtf8("labelSource"));
        labelSource->setMinimumSize(QSize(249, 0));
        labelSource->setMaximumSize(QSize(260, 20));

        formLayout->setWidget(1, QFormLayout::FieldRole, labelSource);

        label_7 = new QLabel(formLayoutWidget);
        label_7->setObjectName(QString::fromUtf8("label_7"));
        label_7->setMaximumSize(QSize(16777215, 20));

        formLayout->setWidget(2, QFormLayout::LabelRole, label_7);

        labelPath = new QLabel(formLayoutWidget);
        labelPath->setObjectName(QString::fromUtf8("labelPath"));
        labelPath->setMinimumSize(QSize(249, 0));
        labelPath->setMaximumSize(QSize(260, 20));

        formLayout->setWidget(2, QFormLayout::FieldRole, labelPath);

        label_11 = new QLabel(formLayoutWidget);
        label_11->setObjectName(QString::fromUtf8("label_11"));
        label_11->setMaximumSize(QSize(16777215, 20));

        formLayout->setWidget(3, QFormLayout::LabelRole, label_11);

        labelContent = new QLabel(formLayoutWidget);
        labelContent->setObjectName(QString::fromUtf8("labelContent"));
        labelContent->setMaximumSize(QSize(260, 20));

        formLayout->setWidget(3, QFormLayout::FieldRole, labelContent);

        label_3 = new QLabel(formLayoutWidget);
        label_3->setObjectName(QString::fromUtf8("label_3"));
        label_3->setMinimumSize(QSize(0, 0));
        label_3->setMaximumSize(QSize(89, 20));

        formLayout->setWidget(4, QFormLayout::LabelRole, label_3);

        dateTimeCreated = new QDateTimeEdit(formLayoutWidget);
        dateTimeCreated->setObjectName(QString::fromUtf8("dateTimeCreated"));
        dateTimeCreated->setEnabled(true);
        QSizePolicy sizePolicy2(QSizePolicy::Maximum, QSizePolicy::Fixed);
        sizePolicy2.setHorizontalStretch(0);
        sizePolicy2.setVerticalStretch(0);
        sizePolicy2.setHeightForWidth(dateTimeCreated->sizePolicy().hasHeightForWidth());
        dateTimeCreated->setSizePolicy(sizePolicy2);
        dateTimeCreated->setMinimumSize(QSize(0, 0));

        formLayout->setWidget(4, QFormLayout::FieldRole, dateTimeCreated);

        label_4 = new QLabel(formLayoutWidget);
        label_4->setObjectName(QString::fromUtf8("label_4"));
        label_4->setMinimumSize(QSize(0, 0));
        label_4->setMaximumSize(QSize(16777215, 20));

        formLayout->setWidget(5, QFormLayout::LabelRole, label_4);

        dateTimeAdded = new QDateTimeEdit(formLayoutWidget);
        dateTimeAdded->setObjectName(QString::fromUtf8("dateTimeAdded"));
        dateTimeAdded->setEnabled(true);
        sizePolicy2.setHeightForWidth(dateTimeAdded->sizePolicy().hasHeightForWidth());
        dateTimeAdded->setSizePolicy(sizePolicy2);

        formLayout->setWidget(5, QFormLayout::FieldRole, dateTimeAdded);

        label_5 = new QLabel(formLayoutWidget);
        label_5->setObjectName(QString::fromUtf8("label_5"));
        label_5->setMinimumSize(QSize(0, 0));
        label_5->setMaximumSize(QSize(16777215, 20));

        formLayout->setWidget(6, QFormLayout::LabelRole, label_5);

        dateTimeModified = new QDateTimeEdit(formLayoutWidget);
        dateTimeModified->setObjectName(QString::fromUtf8("dateTimeModified"));
        dateTimeModified->setEnabled(true);
        sizePolicy2.setHeightForWidth(dateTimeModified->sizePolicy().hasHeightForWidth());
        dateTimeModified->setSizePolicy(sizePolicy2);

        formLayout->setWidget(6, QFormLayout::FieldRole, dateTimeModified);

        label_6 = new QLabel(formLayoutWidget);
        label_6->setObjectName(QString::fromUtf8("label_6"));
        label_6->setMaximumSize(QSize(16777215, 20));

        formLayout->setWidget(7, QFormLayout::LabelRole, label_6);

        labelSize = new QLabel(formLayoutWidget);
        labelSize->setObjectName(QString::fromUtf8("labelSize"));
        labelSize->setMaximumSize(QSize(16777215, 20));

        formLayout->setWidget(7, QFormLayout::FieldRole, labelSize);

        label_10 = new QLabel(formLayoutWidget);
        label_10->setObjectName(QString::fromUtf8("label_10"));
        label_10->setMaximumSize(QSize(16777215, 20));

        formLayout->setWidget(8, QFormLayout::LabelRole, label_10);

        comboPriority = new QComboBox(formLayoutWidget);
        comboPriority->addItem(QString());
        comboPriority->addItem(QString());
        comboPriority->addItem(QString());
        comboPriority->setObjectName(QString::fromUtf8("comboPriority"));
        sizePolicy2.setHeightForWidth(comboPriority->sizePolicy().hasHeightForWidth());
        comboPriority->setSizePolicy(sizePolicy2);

        formLayout->setWidget(8, QFormLayout::FieldRole, comboPriority);

        headChecksum = new QLabel(formLayoutWidget);
        headChecksum->setObjectName(QString::fromUtf8("headChecksum"));
        headChecksum->setMaximumSize(QSize(16777215, 20));

        formLayout->setWidget(9, QFormLayout::LabelRole, headChecksum);

        labelChecksum = new QLabel(formLayoutWidget);
        labelChecksum->setObjectName(QString::fromUtf8("labelChecksum"));
        labelChecksum->setMaximumSize(QSize(16777215, 20));

        formLayout->setWidget(9, QFormLayout::FieldRole, labelChecksum);

        headComment = new QLabel(formLayoutWidget);
        headComment->setObjectName(QString::fromUtf8("headComment"));
        headComment->setMaximumSize(QSize(16777215, 20));

        formLayout->setWidget(10, QFormLayout::LabelRole, headComment);

        editComment = new QLineEdit(formLayoutWidget);
        editComment->setObjectName(QString::fromUtf8("editComment"));
        sizePolicy1.setHeightForWidth(editComment->sizePolicy().hasHeightForWidth());
        editComment->setSizePolicy(sizePolicy1);
        editComment->setMaximumSize(QSize(16777215, 20));

        formLayout->setWidget(10, QFormLayout::FieldRole, editComment);

        checkHiddenExt = new QCheckBox(formLayoutWidget);
        checkHiddenExt->setObjectName(QString::fromUtf8("checkHiddenExt"));
        checkHiddenExt->setMinimumSize(QSize(0, 20));

        formLayout->setWidget(13, QFormLayout::FieldRole, checkHiddenExt);

        checkVideoStream = new QCheckBox(formLayoutWidget);
        checkVideoStream->setObjectName(QString::fromUtf8("checkVideoStream"));
        checkVideoStream->setEnabled(false);
        checkVideoStream->setMinimumSize(QSize(0, 20));

        formLayout->setWidget(12, QFormLayout::FieldRole, checkVideoStream);

        checkHidden = new QCheckBox(formLayoutWidget);
        checkHidden->setObjectName(QString::fromUtf8("checkHidden"));
        checkHidden->setMinimumSize(QSize(0, 20));

        formLayout->setWidget(11, QFormLayout::FieldRole, checkHidden);

        label_8 = new QLabel(formLayoutWidget);
        label_8->setObjectName(QString::fromUtf8("label_8"));
        label_8->setMaximumSize(QSize(16777215, 20));

        formLayout->setWidget(11, QFormLayout::LabelRole, label_8);

        labelDateChangedError = new QLabel(VPropertyDialog);
        labelDateChangedError->setObjectName(QString::fromUtf8("labelDateChangedError"));
        labelDateChangedError->setGeometry(QRect(130, 390, 91, 20));
        labelDateChangedError->setMaximumSize(QSize(260, 20));
        QPalette palette;
        QBrush brush(QColor(0, 0, 0, 255));
        brush.setStyle(Qt::SolidPattern);
        palette.setBrush(QPalette::Active, QPalette::WindowText, brush);
        palette.setBrush(QPalette::Active, QPalette::Text, brush);
        QBrush brush1(QColor(0, 0, 0, 128));
        brush1.setStyle(Qt::NoBrush);
#if QT_VERSION >= QT_VERSION_CHECK(5, 12, 0)
        palette.setBrush(QPalette::Active, QPalette::PlaceholderText, brush1);
#endif
        palette.setBrush(QPalette::Inactive, QPalette::WindowText, brush);
        palette.setBrush(QPalette::Inactive, QPalette::Text, brush);
        QBrush brush2(QColor(0, 0, 0, 128));
        brush2.setStyle(Qt::NoBrush);
#if QT_VERSION >= QT_VERSION_CHECK(5, 12, 0)
        palette.setBrush(QPalette::Inactive, QPalette::PlaceholderText, brush2);
#endif
        QBrush brush3(QColor(120, 120, 120, 255));
        brush3.setStyle(Qt::SolidPattern);
        palette.setBrush(QPalette::Disabled, QPalette::WindowText, brush3);
        palette.setBrush(QPalette::Disabled, QPalette::Text, brush);
        QBrush brush4(QColor(0, 0, 0, 128));
        brush4.setStyle(Qt::NoBrush);
#if QT_VERSION >= QT_VERSION_CHECK(5, 12, 0)
        palette.setBrush(QPalette::Disabled, QPalette::PlaceholderText, brush4);
#endif
        labelDateChangedError->setPalette(palette);
        labelDateCreatedError = new QLabel(VPropertyDialog);
        labelDateCreatedError->setObjectName(QString::fromUtf8("labelDateCreatedError"));
        labelDateCreatedError->setGeometry(QRect(130, 390, 101, 20));
        labelDateCreatedError->setMinimumSize(QSize(0, 0));
        labelDateCreatedError->setMaximumSize(QSize(260, 20));
        labelDateCreatedError->setBaseSize(QSize(0, 0));
        QPalette palette1;
        palette1.setBrush(QPalette::Active, QPalette::WindowText, brush);
        palette1.setBrush(QPalette::Active, QPalette::Text, brush);
        QBrush brush5(QColor(0, 0, 0, 128));
        brush5.setStyle(Qt::NoBrush);
#if QT_VERSION >= QT_VERSION_CHECK(5, 12, 0)
        palette1.setBrush(QPalette::Active, QPalette::PlaceholderText, brush5);
#endif
        palette1.setBrush(QPalette::Inactive, QPalette::WindowText, brush);
        palette1.setBrush(QPalette::Inactive, QPalette::Text, brush);
        QBrush brush6(QColor(0, 0, 0, 128));
        brush6.setStyle(Qt::NoBrush);
#if QT_VERSION >= QT_VERSION_CHECK(5, 12, 0)
        palette1.setBrush(QPalette::Inactive, QPalette::PlaceholderText, brush6);
#endif
        palette1.setBrush(QPalette::Disabled, QPalette::WindowText, brush3);
        palette1.setBrush(QPalette::Disabled, QPalette::Text, brush3);
        QBrush brush7(QColor(0, 0, 0, 128));
        brush7.setStyle(Qt::NoBrush);
#if QT_VERSION >= QT_VERSION_CHECK(5, 12, 0)
        palette1.setBrush(QPalette::Disabled, QPalette::PlaceholderText, brush7);
#endif
        labelDateCreatedError->setPalette(palette1);
        labelDateCreatedError->setTextFormat(Qt::AutoText);
        buttonSync = new QPushButton(VPropertyDialog);
        buttonSync->setObjectName(QString::fromUtf8("buttonSync"));
        buttonSync->setGeometry(QRect(10, 390, 117, 32));

        retranslateUi(VPropertyDialog);
        QObject::connect(buttonBox, SIGNAL(accepted()), VPropertyDialog, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), VPropertyDialog, SLOT(reject()));

        QMetaObject::connectSlotsByName(VPropertyDialog);
    } // setupUi

    void retranslateUi(QDialog *VPropertyDialog)
    {
        VPropertyDialog->setWindowTitle(QCoreApplication::translate("VPropertyDialog", "Properties", nullptr));
#if QT_CONFIG(tooltip)
        imageWarningDateCreated->setToolTip(QCoreApplication::translate("VPropertyDialog", "date is not sync as file system", nullptr));
#endif // QT_CONFIG(tooltip)
#if QT_CONFIG(statustip)
        imageWarningDateCreated->setStatusTip(QString());
#endif // QT_CONFIG(statustip)
#if QT_CONFIG(whatsthis)
        imageWarningDateCreated->setWhatsThis(QString());
#endif // QT_CONFIG(whatsthis)
        imageWarningDateCreated->setText(QCoreApplication::translate("VPropertyDialog", "TextLabel", nullptr));
#if QT_CONFIG(tooltip)
        imageWarningDateModified->setToolTip(QCoreApplication::translate("VPropertyDialog", "date is not sync as file system", nullptr));
#endif // QT_CONFIG(tooltip)
        imageWarningDateModified->setText(QCoreApplication::translate("VPropertyDialog", "TextLabel", nullptr));
#if QT_CONFIG(tooltip)
        imageWarningSize->setToolTip(QCoreApplication::translate("VPropertyDialog", "size is not sync as file system", nullptr));
#endif // QT_CONFIG(tooltip)
        imageWarningSize->setText(QCoreApplication::translate("VPropertyDialog", "TextLabel", nullptr));
        label->setText(QCoreApplication::translate("VPropertyDialog", "Name:", nullptr));
        label_2->setText(QCoreApplication::translate("VPropertyDialog", "Source:", nullptr));
        labelSource->setText(QCoreApplication::translate("VPropertyDialog", "TextLabel", nullptr));
        label_7->setText(QCoreApplication::translate("VPropertyDialog", "Path:", nullptr));
        labelPath->setText(QCoreApplication::translate("VPropertyDialog", "TextLabel", nullptr));
        label_11->setText(QCoreApplication::translate("VPropertyDialog", "Content:", nullptr));
        labelContent->setText(QCoreApplication::translate("VPropertyDialog", "TextLabel", nullptr));
        label_3->setText(QCoreApplication::translate("VPropertyDialog", "Date created:", nullptr));
        label_4->setText(QCoreApplication::translate("VPropertyDialog", "Date added:", nullptr));
        label_5->setText(QCoreApplication::translate("VPropertyDialog", "Date changed:", nullptr));
        label_6->setText(QCoreApplication::translate("VPropertyDialog", "Size:", nullptr));
        labelSize->setText(QCoreApplication::translate("VPropertyDialog", "TextLabel", nullptr));
        label_10->setText(QCoreApplication::translate("VPropertyDialog", "Priority:", nullptr));
        comboPriority->setItemText(0, QCoreApplication::translate("VPropertyDialog", "Low", nullptr));
        comboPriority->setItemText(1, QCoreApplication::translate("VPropertyDialog", "Medium", nullptr));
        comboPriority->setItemText(2, QCoreApplication::translate("VPropertyDialog", "High", nullptr));

        headChecksum->setText(QCoreApplication::translate("VPropertyDialog", "Checksum", nullptr));
        labelChecksum->setText(QCoreApplication::translate("VPropertyDialog", "TextLabel", nullptr));
        headComment->setText(QCoreApplication::translate("VPropertyDialog", "Comment", nullptr));
        checkHiddenExt->setText(QCoreApplication::translate("VPropertyDialog", "Hidden extended", nullptr));
        checkVideoStream->setText(QCoreApplication::translate("VPropertyDialog", "Video stream", nullptr));
        checkHidden->setText(QCoreApplication::translate("VPropertyDialog", "Hidden", nullptr));
        label_8->setText(QCoreApplication::translate("VPropertyDialog", "Attribute", nullptr));
        labelDateChangedError->setText(QCoreApplication::translate("VPropertyDialog", "File not found", nullptr));
        labelDateCreatedError->setText(QCoreApplication::translate("VPropertyDialog", "File not found", nullptr));
        buttonSync->setText(QCoreApplication::translate("VPropertyDialog", "Synchronize", nullptr));
    } // retranslateUi

};

namespace Ui {
    class VPropertyDialog: public Ui_VPropertyDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_VPROPERTYDIALOG_H
