#ifndef QTFILEEDITFACTORY_H
#define QTFILEEDITFACTORY_H

#include "qtpropertybrowser.h"
#include "qtfilepathmanager.h"

class FileEdit;

class FileEditFactory : public QtAbstractEditorFactory<FilePathManager>
{
    Q_OBJECT
public:
    FileEditFactory(QObject *parent = 0)
        : QtAbstractEditorFactory<FilePathManager>(parent)
            { }
    virtual ~FileEditFactory();
protected:
    virtual void connectPropertyManager(FilePathManager *manager);
    virtual QWidget *createEditor(FilePathManager *manager, QtProperty *property,
                QWidget *parent);
    virtual void disconnectPropertyManager(FilePathManager *manager);
private slots:
    void slotPropertyChanged(QtProperty *property, const QString &value);
    void slotFilterChanged(QtProperty *property, const QString &filter);
    void slotSetValue(const QString &value);
    void slotEditorDestroyed(QObject *object);
private:
    QMap<QtProperty *, QList<FileEdit *> > theCreatedEditors;
    QMap<FileEdit *, QtProperty *> theEditorToProperty;
};

#endif // QTFILEEDITFACTORY_H
