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
	void on_markSetStart_clicked();
	void on_markSetEnd_clicked();
	void on_markingsSave_clicked();
	void on_markingsLoad_clicked();
	void on_outExtract_clicked();
	void on_extractProcess_readyReadStandardError();
	void on_extractProcess_readyReadStandardOutput();
	void on_extractProcess_finished(int exitCode, QProcess::ExitStatus exitStatus);

private:
	QSystemTrayIcon* tray;
	QString currentFile;
	QPushButton* openFile;
	QMediaPlayer player;
	QVideoWidget* videoWidget;
	QSlider* sliderZoom;
	QSlider* sliderZoomRHS;
	QSlider* sliderTime;
	QLabel* duration;
	QLabel* timeLow;
	QLabel* timeHigh;
	QLabel* timeCurrent;
	QLineEdit* outFfmpeg;
	QPushButton* outExtract;
	QLabel* markLabel;
	QLabel* markStart;
	QLabel* markEnd;
	QPushButton* markSetStart;
	QPushButton* markSetEnd;
	QPushButton* markingsSave;
	QPushButton* markingsLoad;
	Markings markings;
	QProcess* extractProcess;
	QTextEdit* log;

	void setupLayout();
	QBoxLayout* setupLayoutTop();
	QBoxLayout* setupLayoutFileInfo();
	QBoxLayout* setupLayoutBottom();
	void setupMediaPlayer();
	void updateOut();
	void updateOut(bool copy);
	QString getFfmpegExtractArgs(bool copy);
};

#endif // APPWINDOW_H
