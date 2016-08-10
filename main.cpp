#include <QApplication>
#include <QSystemTrayIcon>
#include "appwindow.h"

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);

	AppWindow m;
	m.show();

//	QSystemTrayIcon tray;
//	tray.setIcon(QIcon::);
//	tray.show();

	return a.exec();
}
