#include "mainwindow.h"
#include <QApplication>
#include <QSystemTrayIcon>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	MainWindow w;
	w.show();

//	QSystemTrayIcon tray;
//	tray.setIcon(QIcon::);
//	tray.show();

	return a.exec();
}
