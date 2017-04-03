#ifndef DIALOGROOTS_H
#define DIALOGROOTS_H


#include <QDialog>

class QListWidget;
class QAction;


class DialogRoots : public QDialog {
	Q_OBJECT
public:
	DialogRoots(QWidget *parent);
private:
	QListWidget *listRoots;
	QAction *actionAdd, *actionDelete, *actionUp, *actionDown;
private slots:
	void itemAdd();
	void itemDelete();
	void itemUp();
	void itemDown();
	void selectionChanged();
	void accept() override;
};


#endif // DIALOGROOTS_H
