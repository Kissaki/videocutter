#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class MainWindow;
}
class QVideoWidget;

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	explicit MainWindow(QWidget *parent = 0);
	~MainWindow();

private:
	Ui::MainWindow *ui;
	QVideoWidget* videoWidget;

	void fileList_dragEnterEvent();
	void fileList_clicked(QModelIndex);
};

#endif // MAINWINDOW_H
