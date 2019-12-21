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

#ifndef Q_HEX_VIEWER_H_
#define Q_HEX_VIEWER_H_

#include <QAbstractScrollArea>
#include <QByteArray>
#include <QFile>

class QHexView: public QAbstractScrollArea

{
	public:
		class DataStorage
		{
			public:
				virtual ~DataStorage() {};
				virtual QByteArray getData(std::size_t position, std::size_t length) = 0;
				virtual std::size_t size() = 0;
		};


		class DataStorageArray: public DataStorage
		{
			public:
				DataStorageArray(const QByteArray &arr);
				virtual QByteArray getData(std::size_t position, std::size_t length);
				virtual std::size_t size();
			private:
				QByteArray    m_data;
		};

		class DataStorageFile: public DataStorage
		{
			public:
				DataStorageFile(const QString &fileName);
				virtual QByteArray getData(std::size_t position, std::size_t length);
				virtual std::size_t size();
			private:
				QFile      m_file;
		};



		QHexView(QWidget *parent = 0);
		~QHexView();

	public slots:
		void setData(DataStorage *pData);
		void clear();
		void showFromOffset(std::size_t offset);
        void saveTofile(QString file);

	protected:
		void paintEvent(QPaintEvent *event);
		void keyPressEvent(QKeyEvent *event);
		void mouseMoveEvent(QMouseEvent *event);
		void mousePressEvent(QMouseEvent *event);
	private:
		DataStorage          *m_pdata;
		std::size_t           m_posAddr; 
		std::size_t           m_posHex;
		std::size_t           m_posAscii;
		std::size_t           m_charWidth;
		std::size_t           m_charHeight;


		std::size_t           m_selectBegin;
		std::size_t           m_selectEnd;
		std::size_t           m_selectInit;
		std::size_t           m_cursorPos;


		QSize fullSize() const;
		void resetSelection();
		void resetSelection(int pos);
		void setSelection(int pos);
		void ensureVisible();
		void setCursorPos(int pos);
		std::size_t cursorPos(const QPoint &position);
};

#endif
