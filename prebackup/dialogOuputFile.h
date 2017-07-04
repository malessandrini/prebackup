/*
Prebackup
Copyright (C) 2017  Michele Alessandrini

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License version 3
as published by the Free Software Foundation.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/


#ifndef DIALOGOUTPUTFILE_H
#define DIALOGOUTPUTFILE_H


#include <QDialog>

class QPlainTextEdit;
class QDialogButtonBox;


class DialogOutputFile : public QDialog {
	Q_OBJECT
public:
	DialogOutputFile(QWidget *parent, QString const &title, QString const &label, QStringList const &items,
		QString const &warning, QString const &setting);
private:
	QPlainTextEdit *textBox;
	QDialogButtonBox *bbox;
	const QString settingName;
private slots:
	void saveFile();
};


#endif // DIALOGLISTCHOOSE_H
