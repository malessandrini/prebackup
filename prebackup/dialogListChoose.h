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


#ifndef DIALOGLISTCHOOSE_H
#define DIALOGLISTCHOOSE_H


#include <QDialog>

class QListWidget;
class QListWidgetItem;
class QDialogButtonBox;


class DialogListChoose : public QDialog {
	Q_OBJECT
public:
	DialogListChoose(QWidget *parent, QString const &title, QString const &label, QStringList const &items);
	QString getResult() const { return result; }
private:
	QListWidget *list;
	QDialogButtonBox *bbox;
	QString result;
private slots:
	void selectionChanged();
	void doubleClicked(QListWidgetItem*);
};


#endif // DIALOGLISTCHOOSE_H
