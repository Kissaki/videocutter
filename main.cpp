#include <QApplication>
#include <QSystemTrayIcon>
#include "appwindow.h"

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);

	QSystemTrayIcon tray;
	tray.setIcon(QIcon(":/video_edit.png"));
	tray.setToolTip("MediaCutter");
	tray.show();

	AppWindow m(&tray);
	m.show();

	return a.exec();
}
