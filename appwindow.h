#ifndef APPWINDOW_H
#define APPWINDOW_H

#include <QWidget>
#include <QMediaPlayer>
#include <QVideoWidget>
#include <QFile>
#include <QProcess>
#include "markings.h"

class QBoxLayout;
class QSlider;
class QLabel;
class QPushButton;
class QLineEdit;
class QSystemTrayIcon;
class QTextEdit;
class MarkingsWidget;
class MarkersModel;
class ExportProcessor;

class AppWindow : public QWidget
{
	Q_OBJECT
public:
	explicit AppWindow(QSystemTrayIcon* tray, QWidget *parent = 0);

signals:
	void currentFileChanged(QString& newFile);

public slots:

private slots:
	void on_openFile_clicked();
	void onCurrentFileChanged(QString& newFile);
	void on_sliderZoom_valueChanged(int v);
	void on_sliderZoomRHS_valueChanged(int v);
	void on_player_durationChanged(qint64 d);
	void on_player_positionChanged(qint64 p);
	void on_sliderTime_valueChanged(int v);
	void on_sliderTime_rangeChanged(int min, int max);

private:
	QSystemTrayIcon* tray;

	QString currentFile;
	QMediaPlayer player;

	QVideoWidget* videoWidget;
	QSlider* sliderZoom;
	QSlider* sliderZoomRHS;
	QSlider* sliderTime;
	QPushButton* openFile;
	QLabel* duration;
	QLabel* timeLow;
	QLabel* timeHigh;
	QLabel* timeCurrent;
	MarkingsWidget* markinsWidget;

	void setupLayout();
	QBoxLayout* setupLayoutTop();
	QBoxLayout* setupLayoutFileInfo();
	QBoxLayout* setupLayoutBottom();
	void setupMediaPlayer();
};

#endif // APPWINDOW_H
