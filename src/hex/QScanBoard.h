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

#ifndef QSCANBOARD_H
#define QSCANBOARD_H

#pragma once
#include <QtWidgets>
#include <QBrush>
#include <QPen>
#include <QPixmap>

class QScanBoard : public QAbstractScrollArea
{
    Q_OBJECT

public:

    QScanBoard(unsigned int numRows, unsigned int numCols, QWidget* parent = nullptr);

    unsigned int numCols() const { return m_numCols; }
    unsigned int numRows() const { return m_numRows; }

    void setCurrentSector(int nSector);
    void setBadSector(int nSector);
    void startScan(int sectorOffset);
    void setMaxSide(int nValue);
    void saveTofile(QString file, QString drive);
    QVector<int> badSectorVector;
    QVector<int> skipSectorVector;

    bool isActive;

protected:
    void paintEvent(QPaintEvent *event);


private:

    std::size_t           m_squareWidth;
    std::size_t           m_squareHeight;
    unsigned int m_numRows;
    unsigned int m_numCols;
    int maxSide;
    int nCurrentRange;
    std::size_t           m_charWidth;
    int sOffset;

    QSize fullSize() const;

};

#endif // QSCANBOARD_H
