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
class QSpinBox;
class QDoubleSpinBox;

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
	void on_timeLow_valueChanged(int v);
	void on_timeHigh_valueChanged(int v);
	void on_timeCurrent_valueChanged(int v);
	void onCurrentFileChanged(QString& newFile);
	void on_sliderZoom_valueChanged(int v);
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

private:
	int skipDistance;
	QSystemTrayIcon* tray;

	QString currentFile;
	QMediaPlayer player;

	QVideoWidget* videoWidget;
	QSlider* sliderZoom;
	QSlider* sliderZoomRHS;
	QSlider* sliderTime;
	QPushButton* openFile;
	QLabel* duration;
	QSpinBox* timeLow;
	QSpinBox* timeHigh;
	QSpinBox* timeCurrent;
	QDoubleSpinBox* playbackSpeed;
	QSpinBox* uiNotifyRate;
	QSlider* playerVolume;
	QPushButton* playerPlayPause;
	QPushButton* playerSkipToStart;
	QPushButton* playerSkipBackward;
	QPushButton* playerSkipForward;
	MarkingsWidget* markinsWidget;

	void setupLayout();
	QBoxLayout* setupLayoutTop();
	QBoxLayout* setupLayoutFileInfo();
	QBoxLayout* setupLayoutBottom();
	void setupMediaPlayer();

	void dragEnterEvent(QDragEnterEvent *event) override;
	void dropEvent(QDropEvent *event) override;
};

#endif // APPWINDOW_H
