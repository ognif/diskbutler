#include <QString>
#include <QDateTime>
#include <QCryptographicHash>
#include <QFile>
#include "mainwindow.h"
#include "FoxSDKExport.h"

extern QString humanReadableSize(const qint64 src_size, qint64 *dst_size = NULL);
extern QString humanReadableSector(const qint64 src_size);
extern QDateTime ConvertDateTime(const SFileDateTime &dateTime);
extern QByteArray CalcChecksum(const QString &filePath);
extern wchar_t* convertToFoxValue(QString strValue);

extern int32 MSFToInt32(const int minutes, const int seconds, const int frames);
extern QString MSFToStr(const int minutes, const int seconds, const int frames);
extern QString MSFInt32ToStr(int32 index);
extern int32 MSFStrToInt32(QString str);
extern bool fileExists(QString strFile);
extern MainWindow* getMainWindow();
