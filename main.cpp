#include <QApplication>
#include <QSystemTrayIcon>
#include "appwindow.h"

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);

	QSystemTrayIcon tray;
	AppWindow m(&tray);
	m.show();

//	tray.setIcon(QIcon::);
//	tray.show();

	return a.exec();
}
