#ifndef FILEPATHMANAGER_H
#define FILEPATHMANAGER_H


#include "qtpropertybrowser.h"
#include <QMap>

class FilePathManager : public QtAbstractPropertyManager
{
    Q_OBJECT
public:
    FilePathManager(QObject *parent = 0)
        : QtAbstractPropertyManager(parent)
            { }

    QString value(const QtProperty *property) const;
    QString filter(const QtProperty *property) const;

public slots:
    void setValue(QtProperty *property, const QString &val);
    void setFilter(QtProperty *property, const QString &fil);
signals:
    void valueChanged(QtProperty *property, const QString &val);
    void filterChanged(QtProperty *property, const QString &fil);
protected:
    virtual QString valueText(const QtProperty *property) const { return value(property); }
    virtual void initializeProperty(QtProperty *property) { theValues[property] = Data(); }
    virtual void uninitializeProperty(QtProperty *property) { theValues.remove(property); }
private:

    struct Data
    {
        QString value;
        QString filter;
    };

    QMap<const QtProperty *, Data> theValues;
};

#endif // FILEPATHMANAGER_H
