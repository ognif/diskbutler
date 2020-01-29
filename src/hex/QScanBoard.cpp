/*
 *  DiskButler - a powerful CD/DVD/BD recording software tool for Linux, macOS and Windows.
 *  Copyright (c) 20019 Ingo Foerster (pixbytesl@gmail.com).
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License 3 as published by
 *  the Free Software Foundation; either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License 3 for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include <QtWidgets>
#include "QScanBoard.h"
#include <QDebug>
#include <QPainter>
#include <QScrollBar>
#include <QSize>
#include <QPaintEvent>

QScanBoard::QScanBoard(unsigned int numRows, unsigned int numCols, QWidget* parent)
    : QAbstractScrollArea(parent)
    , m_numRows(numRows)
    , m_numCols(numCols)
    , maxSide(0)
{
    setFont(QFont("Courier", 14));

    m_squareWidth = 8;
    m_squareHeight = 8;
    sOffset = 0;
    isActive = false;

    m_charWidth = fontMetrics().width(QLatin1Char('9'));

    nCurrentRange = 0;

    //Todos
    setBackgroundRole(QPalette::Base);
    setAutoFillBackground(true);

    setFocusPolicy(Qt::StrongFocus);

    verticalScrollBar()->setValue(0);

    //VEctor clear
    badSectorVector.clear();
    skipSectorVector.clear();
}

void QScanBoard::startScan(int sectorOffset)
{
    badSectorVector.clear();
    nCurrentRange = 0;
    sOffset = sectorOffset;

    for(int i = 0; i<sectorOffset;i++){
        skipSectorVector.append(i);
    }
}

void QScanBoard::setMaxSide(int nValue)
{
    maxSide = nValue;
    viewport()->update();
}

void QScanBoard::setCurrentSector(int nSector)
{
    nCurrentRange = nSector;
    //badSectorVector.append(nSector);
    viewport()->update();
}

void QScanBoard::setBadSector(int nSector)
{
    //Wie speichern wir die BAdSectors und lesen neu?
    //Wir können einmal eine Liste durchschleifen, aber das macht bei 50millionne bei Blu-Ray keinen Sinn.
    //Daher nur die Schlechten Sektoren mitschleifen.
    badSectorVector.append(nSector);
    //indexof() Gibt den index des zuerst gefundenen Items wieder. -1 bei nicht gefunden. Wäre ja optimal für uns.
}

QSize QScanBoard::fullSize() const
{
    QSize areaSize = viewport()->size();

    int nWidth = areaSize.width();
    int columCount = nWidth / (m_squareWidth+2);
    int rowCount = maxSide / columCount;
    int allSizeHeight = rowCount * (m_squareHeight+2);

    //qDebug("Columncount: %d",columCount);
    //qDebug("Rowcount: %d",rowCount);
    //qDebug("All Height: %d",allSizeHeight);

    return QSize(nWidth, allSizeHeight);

}

void QScanBoard::paintEvent(QPaintEvent *event)
{

    QPainter painter(viewport());
    QSize areaSize = viewport()->size();
    QSize  widgetSize = fullSize();


    if(maxSide==0){
        QString dataMessage = tr("Empty disc. No data available.");
        painter.fillRect(event->rect(), this->palette().color(QPalette::Base));

        int pX = (areaSize.width()/2)-((m_charWidth*dataMessage.length())/2);
        int pY = areaSize.height()/2;

        painter.setPen(Qt::darkGray);

        painter.drawText(pX, pY, dataMessage);
        return;
    }


    //We step 1 Line by another through the widget
    //Perfect Step, one line after another
    //Page ste is the step fo a whole screen.
    verticalScrollBar()->setPageStep(m_squareHeight+2);

    //Current calculation error is 117 Lines missing.
    verticalScrollBar()->setRange(0, (widgetSize.height() - areaSize.height()) / (m_squareHeight+2) + 1);



    int firstLineIdx = verticalScrollBar() -> value();
    int lastLineIdx = firstLineIdx + areaSize.height() / (m_squareHeight+2);

    painter.fillRect(event->rect(), this->palette().color(QPalette::Base));

    int nRaster = (areaSize.width() / (m_squareWidth+2));
    int nLines = maxSide / nRaster;

    //We jsut paint what we see.

    //was haben wir hier
    //viewport 1001 * 400, 400 ist die Höhe
    /*
     * Ich muss den Viewpot virtuell abbilden umd das Bild zu malen
     * i kleine gleich firstLineIndex
     * Pagesize, ist die Höhe / squeres
     * Was dawzschen ist wird gemalt
     * Bei dem Progress.
     * Wie verfahren wir jetzt? Wir müssten den Pen setzen nach den Ereignissen.
     * QList? Nein LAut QT ist QVector schneller. Also nehmen wir QVector.
     *
     */

    for(int i = 0;i<nLines;i++){ //We will print the vertical lines

        for(int y = 0; y<nRaster;y++){ //We will paint the horizontal columns

            //Der ist ja 0 wenn i kleiner gleich 0 ist und i kleiner als 0 + height / squere

            if(i>=firstLineIdx && i < (firstLineIdx + (areaSize.height() / (m_squareHeight+2)))){
                /*
                if(i % 2 == 0){
                    painter.setBrush(QBrush(Qt::green, Qt::SolidPattern));
                }else{
                    painter.setBrush(QBrush(Qt::red, Qt::SolidPattern));
                }
                */
                painter.setPen(QPen(Qt::lightGray, 0, Qt::SolidLine, Qt::SquareCap, Qt::RoundJoin));
                painter.setBrush(QBrush(Qt::lightGray, Qt::SolidPattern));

                if((i*nRaster)+y<nCurrentRange){

                    painter.setPen(QPen(Qt::green, 0, Qt::SolidLine, Qt::SquareCap, Qt::RoundJoin));
                    painter.setBrush(QBrush(Qt::green, Qt::SolidPattern));

                    if(skipSectorVector.indexOf((i*nRaster)+y) != -1){
                        painter.setPen(QPen(Qt::cyan, 0, Qt::SolidLine, Qt::SquareCap, Qt::RoundJoin));
                        painter.setBrush(QBrush(Qt::cyan, Qt::SolidPattern));
                    }
                    //Show Bad secto at sector x
                    if(badSectorVector.indexOf((i*nRaster)+y) != -1){
                        painter.setPen(QPen(Qt::red, 0, Qt::SolidLine, Qt::SquareCap, Qt::RoundJoin));
                        painter.setBrush(QBrush(Qt::red, Qt::SolidPattern));
                    }



                }

                QRect rect(y*(m_squareWidth+2), (i-firstLineIdx)*(m_squareWidth+2), m_squareWidth, m_squareWidth);
                painter.drawRect(rect);
            }

        }

    }

    painter.setRenderHint(QPainter::Antialiasing, false);
    painter.setPen(palette().dark().color());
    painter.setBrush(Qt::NoBrush);
    painter.drawRect(QRect(0, 0, width() - 1, height() - 1));

    //This is the ensue visible function. We need to handle here the scrolling.
    //Currently no idea about scrolling and ensure visible.
    //My idea is, todisable ensure visible when manual scroll was detected.
    if(isActive && nCurrentRange > (lastLineIdx*nRaster)){

        verticalScrollBar() ->setValue(firstLineIdx + (areaSize.height() / (m_squareHeight+2))/2 );

    }


}

void QScanBoard::saveTofile(QString file, QString drive)
{
    QString writerBlock = "";

    QFile outputFile(file);
    outputFile.open(QIODevice::WriteOnly);
    QTextStream outStream(&outputFile);

    writerBlock += "Disc Scan Report \n\n";
    writerBlock += QString("Drive Name:  %1\n").arg(drive);
    writerBlock += QString("Total Sectors on disc:  %1\n").arg(QString::number(maxSide));
    writerBlock += QString("Sector Offset:  %1\n").arg(QString::number(sOffset));
    writerBlock += QString("Sector length:  %1\n").arg(QString::number(nCurrentRange-sOffset));
    writerBlock += QString("Bad sector count:  %1\n").arg(QString::number(badSectorVector.size()));

    writerBlock += "\n";

    for(auto iter = badSectorVector.begin(); iter != badSectorVector.end(); iter++){

        writerBlock += QString("Read error at sector:  %1\n").arg(QString::number(*iter));

    }
    //+= "\n";

    outStream << writerBlock;
    outputFile.close();
}

