#ifndef CONFIGDIALOG_H
#define CONFIGDIALOG_H

#include <QDialog>
#include <QHBoxLayout>
#include <QVBoxLayout>

QT_BEGIN_NAMESPACE
class QListWidget;
class QListWidgetItem;
class QStackedWidget;
QT_END_NAMESPACE

class ConfigDialog : public QDialog
{
    Q_OBJECT

public:
    ConfigDialog();


public slots:
    void changePage(QListWidgetItem *current, QListWidgetItem *previous);
    void saveAll();
    void enableApply(int index);
    void saveApply();


private:
    void createIcons();
    void sendSaveCommand();

    QListWidget *contentsWidget;
    QStackedWidget *pagesWidget;

    QPushButton *applyButton;
    QPushButton *okButton;
    QPushButton *closeButton;
};
#endif // CONFIGDIALOG_H
