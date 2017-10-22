#ifndef APPWINDOW_H
#define APPWINDOW_H

#include <QMainWindow>
#include <QVideoWidget>
#include <QFile>
#include <QProcess>
#include "markings.h"

namespace Ui { class AppWindow; }
class QMediaPlayer;
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
class QSpinBox;
class QDoubleSpinBox;

class AppWindow : public QMainWindow
{
	Q_OBJECT
public:
	explicit AppWindow(QSystemTrayIcon* tray, QWidget *parent = 0);

	/**
	 * @brief setFilepath opens the file with path newFile
	 * @param newFile
	 */
	void setFilepath(const QString& newFile);

signals:

public slots:

private slots:
	void on_openFile_clicked();
	void on_timeLow_valueChanged(int v);
	void on_timeHigh_valueChanged(int v);
	void on_timeCurrent_valueChanged(int v);
	void on_sliderZoomLHS_valueChanged(int v);
	void on_sliderZoomRHS_valueChanged(int v);
	void on_player_durationChanged(qint64 d);
	void on_player_positionChanged(qint64 p);
	void updateTimeLabels();
	void on_sliderTime_valueChanged(int v);
	void on_sliderTime_rangeChanged(int min, int max);
	void on_playbackSpeed_valueChanged(double v);
	void on_uiNotifyRate_valueChanged(int v);
	void on_player_playbackRateChanged(qreal rate);
	void on_player_notifyIntervalChanged(int ms);
	void on_playerVolume_valueChanged(int v);
	void on_playerPlayPause_clicked();
	void on_playerSkipToStart_clicked();
	void on_playerSkipBackward_clicked();
	void on_playerSkipForward_clicked();
	void onPlayRange(int timeStartMS, int timeEndMS);
	void on_markinsWidget_ffmpegParametersChanged(QString parameters);
	void on_exportProcessor_statusInfo(QString s);
	void on_exportProcessor_finished(QString target, int sizeMB);

private:
	Ui::AppWindow* ui;
	int skipDistance;
	QSystemTrayIcon* tray;

	QString currentFile;
	QMediaPlayer* player;

	ExportProcessor* exportProcessor;

	void setupMediaPlayer();
	void resetPlayerControls();

	void dragEnterEvent(QDragEnterEvent *event) override;
	void dropEvent(QDropEvent *event) override;
	void keyPressEvent(QKeyEvent *key) override;
};

#endif // APPWINDOW_H
