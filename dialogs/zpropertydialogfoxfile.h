#ifndef ZPROPERTYDIALOGFOXFILE_H
#define ZPROPERTYDIALOGFOXFILE_H

#include <QDialog>
#include "nodes/QDataItem.h"
#include "zallocation_table_model.h"
#include "zinfotablemodel.h"

namespace Ui {
class ZPropertyDialogFoxFile;
}

class ZPropertyDialogFoxFile : public QDialog
{
  Q_OBJECT

public:
  explicit ZPropertyDialogFoxFile(QWidget *parent, QDataItem *qItem);
  ~ZPropertyDialogFoxFile();

private:
  void addEntry(QString location, QString length);
  void addEntryInfo(QString location, QString length);

private:
  Ui::ZPropertyDialogFoxFile *ui;
  QDataItem *mItem;
  ZAllocationTableModel *mTableModel;
  ZInfoTableModel *mInfoModel;
  void fillFiles();
  void fillFolders();
  void fillDisk();
  void fillSession();
  void fillTrack();
  void fillISO();
  void fillUDF();
  void fillAudioTrack();
};

#endif // ZPROPERTYDIALOGFOXFILE_H
