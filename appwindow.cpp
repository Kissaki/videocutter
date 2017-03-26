#include "appwindow.h"
#include <QDoubleSpinBox>
#include <QFileDialog>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QMediaPlayer>
#include <QMediaPlaylist>
#include <QProcess>
#include <QPushButton>
#include <QSlider>
#include <QSpinBox>
#include <QSystemTrayIcon>
#include <QTextEdit>
#include <QVideoWidget>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QMimeData>
#include "markingswidget.h"
#include "markersmodel.h"
#include "exportprocessor.h"

AppWindow::AppWindow(QSystemTrayIcon* tray, QWidget *parent)
	: QWidget(parent)
	, skipDistance(5000)
	, tray(tray)
	, player(this)
	, videoWidget(new QVideoWidget(this))
	, sliderZoom(new QSlider(Qt::Horizontal, this))
	, sliderZoomRHS(new QSlider(Qt::Horizontal, this))
	, sliderTime(new QSlider(Qt::Horizontal, this))
	, openFile(new QPushButton("Open"))
	, duration(new QLabel("0", this))
	, timeLow(new QSpinBox(this))
	, timeHigh(new QSpinBox(this))
	, timeCurrent(new QSpinBox(this))
	, playbackSpeed(new QDoubleSpinBox(this))
	, uiNotifyRate(new QSpinBox(this))
	, playerVolume(new QSlider(Qt::Horizontal, this))
	, playerPlayPause(new QPushButton("⏯", this))
	, playerSkipToStart(new QPushButton("⏮", this))
	, playerSkipBackward(new QPushButton("⏪", this))
	, playerSkipForward(new QPushButton("⏩", this))
	, markinsWidget(new MarkingsWidget(new ExportProcessor(this), this))
{
	openFile->setObjectName("openFile");
	timeLow->setObjectName("timeLow");
	timeHigh->setObjectName("timeHigh");
	timeCurrent->setObjectName("timeCurrent");
	sliderZoom->setObjectName("sliderZoom");
	sliderZoomRHS->setObjectName("sliderZoomRHS");
	sliderTime->setObjectName("sliderTime");
	player.setObjectName("player");
	playbackSpeed->setObjectName("playbackSpeed");
	uiNotifyRate->setObjectName("uiNotifyRate");
	playerVolume->setObjectName("playerVolume");
	playerPlayPause->setObjectName("playerPlayPause");
	playerSkipToStart->setObjectName("playerSkipToStart");
	playerSkipBackward->setObjectName("playerSkipBackward");
	playerSkipForward->setObjectName("playerSkipForward");
	markinsWidget->setObjectName("markinsWidget");

	setupLayout();

	const int TIME_STEPSIZE = 100;
	timeLow->setSingleStep(TIME_STEPSIZE);
	timeHigh->setSingleStep(TIME_STEPSIZE);
	timeCurrent->setSingleStep(TIME_STEPSIZE);

	setupMediaPlayer();
	resetPlayerControls();
	playbackSpeed->setSingleStep(0.5);
	// With higher values than 4.0 QMediaPlayer simply sets it to 1.0 instead. https://bugreports.qt.io/browse/QTBUG-55354
	// According to documentation of QMediaPlayer, negative values should work, but do not. In case that's format or system specific, allow negative numbers for now.
	playbackSpeed->setRange(-4.0, 4.0);
	playbackSpeed->setValue(1.0);
	playerVolume->setRange(0, 100);
	playerVolume->setValue(player.volume());
	uiNotifyRate->setRange(100, 1000);
	uiNotifyRate->setSingleStep(100);
	uiNotifyRate->setValue(1000);

	timeLow->setRange(0, 0);
	timeHigh->setRange(0, 0);
	timeCurrent->setRange(0, 0);

	setAcceptDrops(true);

	QMetaObject::connectSlotsByName(this);
	connect(this, &AppWindow::currentFileChanged, this, &AppWindow::onCurrentFileChanged);
	connect(markinsWidget, &MarkingsWidget::playRange, this, &AppWindow::onPlayRange);
	connect(&player, &QMediaPlayer::stateChanged, this, &AppWindow::updateTimeLabels);
}

void AppWindow::onPlayRange(int timeStartMS, int timeEndMS)
{
	player.pause();
	sliderZoom->setValue(timeStartMS);
	sliderZoomRHS->setValue(timeEndMS);
	player.setPosition(timeStartMS);
	player.play();
}

void AppWindow::on_markinsWidget_ffmpegParametersChanged(QString parameters)
{

}

/*
 * Uses:
 * * Identifying interesting scenes in the videofile and marking them
 * * Extracting marked scenes
 *
 * +---------------------------+
 * |      layTop               |
 * | layFileInfo | videoWidget |
 * |             |             |
 * +---------------------------+
 * |          layBottom        |
 * |     layTimeRange          |
 * | layTimeControls | markinsWidget           |
 * +---------------------------+
 */
void AppWindow::setupLayout()
{
	setMinimumSize(1280, 720);

	auto layMain = new QVBoxLayout();

	auto layTop = setupLayoutTop();
	auto layBottom = setupLayoutBottom();

	layMain->addLayout(layTop, 1);
	layMain->addLayout(layBottom);

	setLayout(layMain);
}

QBoxLayout* AppWindow::setupLayoutTop()
{
	auto layTop = new QHBoxLayout();

	auto layFileInfo = setupLayoutFileInfo();

	layTop->addLayout(layFileInfo);
	// Stretch
	layTop->addWidget(videoWidget, 1);

	return layTop;
}

QBoxLayout* AppWindow::setupLayoutFileInfo()
{
	auto layFileInfo = new QVBoxLayout();
	layFileInfo->addWidget(openFile, 0, Qt::AlignTop);
	layFileInfo->addWidget(duration, 0, Qt::AlignTop);
	layFileInfo->addStretch(1);

	return layFileInfo;
}

QBoxLayout* AppWindow::setupLayoutBottom()
{
	auto layBottom = new QVBoxLayout();

	auto layTimeRange = new QHBoxLayout();
	layTimeRange->addWidget(sliderZoom);
	layTimeRange->addWidget(sliderZoomRHS);
	layBottom->addLayout(layTimeRange);
	layBottom->addWidget(sliderTime);

	auto layTimeControls = new QHBoxLayout();
	auto layTime = new QHBoxLayout();
	auto gbTime = new QGroupBox(tr("Time [ms]"), this);
	layTime->addWidget(new QLabel(tr("Range Beginning"), gbTime));
	layTime->addWidget(timeLow);
	layTime->addWidget(new QLabel(tr("Current Time"), gbTime));
	layTime->addWidget(timeCurrent);
	layTime->addWidget(new QLabel(tr("Range End"), gbTime));
	layTime->addWidget(timeHigh);
	layTime->addStretch(1);
	gbTime->setLayout(layTime);
	layTimeControls->addWidget(gbTime);
	auto gbControls = new QGroupBox("Playback", this);
	auto layControls = new QHBoxLayout();
	layControls->addWidget(playerSkipToStart);
	layControls->addWidget(playerSkipBackward);
	layControls->addWidget(playerPlayPause);
	layControls->addWidget(playerSkipForward);
	layControls->addWidget(new QLabel(tr("Speed"), gbControls));
	layControls->addWidget(playbackSpeed);
	layControls->addWidget(new QLabel(tr("Volume"), gbControls));
	playerVolume->setMaximumWidth(60);
	layControls->addWidget(playerVolume);
	layControls->addWidget(new QLabel(tr("UI Updaterate [ms]"), gbControls));
	layControls->addWidget(uiNotifyRate);
	gbControls->setLayout(layControls);
	layTimeControls->addWidget(gbControls);
	layTimeControls->addStretch();
	layBottom->addLayout(layTimeControls);

	layBottom->addWidget(markinsWidget);

	return layBottom;
}

void AppWindow::setupMediaPlayer()
{
	auto playlist = new QMediaPlaylist(&player);
	playlist->setPlaybackMode(QMediaPlaylist::CurrentItemInLoop);
	player.setPlaylist(playlist);
	player.setVideoOutput(videoWidget);
}

void AppWindow::resetPlayerControls()
{
	duration->setText(QString().sprintf("%d", 0));
	timeLow->setRange(0, 0);
	timeHigh->setRange(0, 0);
	timeCurrent->setRange(0, 0);
	sliderZoom->setMaximum(0);
	sliderZoomRHS->setMaximum(0);
	sliderTime->setRange(0, 0);
}

void AppWindow::on_openFile_clicked()
{
	auto newFilePath = QFileDialog::getOpenFileName(
				this,
				tr("Open Image"),
				QString(),
				tr("Video Files (*.mp4 *.mkv *.avi *.wmv)"));
	if (!newFilePath.isEmpty())
	{
		emit currentFileChanged(newFilePath);
	}
}

void AppWindow::onCurrentFileChanged(QString& newFile)
{
	qDebug() << "opening file" << newFile;

	resetPlayerControls();

	currentFile = newFile;
	markinsWidget->setFile(currentFile);
	player.playlist()->clear();
	player.playlist()->addMedia(QUrl::fromLocalFile(newFile));
	player.playlist()->setCurrentIndex(1);

	player.play();
}

void AppWindow::on_timeLow_valueChanged(int v)
{
	sliderZoom->setValue(v);

	timeHigh->setMinimum(v);
	timeCurrent->setMinimum(timeLow->value());
}

void AppWindow::on_sliderZoom_valueChanged(int v)
{
	timeLow->setValue(v);

	sliderZoomRHS->setMinimum(v);
	sliderTime->setMinimum(v);
}

void AppWindow::on_timeHigh_valueChanged(int v)
{
	sliderZoomRHS->setValue(v);

	timeLow->setMaximum(v);
	timeCurrent->setMaximum(v);
}

void AppWindow::on_sliderZoomRHS_valueChanged(int v)
{
	timeHigh->setValue(v);

	sliderZoom->setMaximum(v);
	sliderTime->setMaximum(v);
}

void AppWindow::on_player_durationChanged(qint64 d)
{
	timeLow->setRange(0, d);
	timeHigh->setRange(0, d);
	timeCurrent->setRange(0, d);

	sliderZoomRHS->setMaximum(d);
	timeHigh->setValue(d);
	sliderZoomRHS->setValue(d);

	duration->setText(QString::number(d));
}

void AppWindow::on_player_positionChanged(qint64 position)
{
	if (!sliderTime->isSliderDown())
	{
		sliderTime->setValue(position);
	}
	markinsWidget->setCurrentPosition(position);
	timeCurrent->setValue(position);

	if (sliderZoomRHS->value() > 0 && position >= sliderZoomRHS->value() && player.state() == QMediaPlayer::PlayingState)
	{
		player.setPosition(sliderZoom->value());
	}
}

void AppWindow::updateTimeLabels()
{
	markinsWidget->setCurrentPosition(player.position());
	timeCurrent->setValue(player.position());
}

void AppWindow::on_timeCurrent_valueChanged(int v)
{
	// We handle user input or player position change.
	// In case of user input, we set the player position.
	if (player.position() != v)
	{
		player.setPosition(v);
	}
}

// Use cases:
// * tracking/user seeking
// * player playback (incremental)
void AppWindow::on_sliderTime_valueChanged(int v)
{
	if (sliderTime->isSliderDown())
	{
		player.setPosition(v);
	}
}

void AppWindow::on_sliderTime_rangeChanged(int min, int max)
{
	auto v = sliderTime->value();
	if (v < min)
	{
		sliderTime->setValue(min);
	} else if (v > max)
	{
		sliderTime->setValue(max);
	}
	timeLow->setValue(min);
	timeHigh->setValue(max);
}

void AppWindow::on_playbackSpeed_valueChanged(double v)
{
	player.setPlaybackRate(v);
}

void AppWindow::on_player_playbackRateChanged(qreal rate)
{
	playbackSpeed->setValue(rate);
}

void AppWindow::on_player_notifyIntervalChanged(int ms)
{
	uiNotifyRate->setValue(ms);
}

void AppWindow::on_playerVolume_valueChanged(int v)
{
	player.setVolume(v);
}

void AppWindow::on_playerPlayPause_clicked()
{
	if (player.state() == QMediaPlayer::PlayingState)
	{
		player.pause();
	}
	else
	{
		player.play();
	}
}

void AppWindow::on_playerSkipToStart_clicked()
{
	player.setPosition(timeLow->value());
}

void AppWindow::on_playerSkipBackward_clicked()
{
	player.setPosition(player.position() - skipDistance);
}

void AppWindow::on_playerSkipForward_clicked()
{
	player.setPosition(player.position() + skipDistance);
}

void AppWindow::on_uiNotifyRate_valueChanged(int v)
{
	player.setNotifyInterval(v);
}

void AppWindow::dragEnterEvent(QDragEnterEvent *event)
{
	if (event->mimeData()->hasUrls()
//FIXME: Only accept video/audio formats
//			&& (event->mimeData()->hasFormat("video/*") || event->mimeData()->hasFormat("audio/*"))
			)
	{
		event->accept();
	}
}

void AppWindow::dropEvent(QDropEvent *event)
{
	auto urls = event->mimeData()->urls();
	if (urls.size() > 1)
	{
		qDebug() << "Multiple URLs passed; only opening the first one. Passed:" << urls.size();
		return;
	}
	auto p = urls.first().toLocalFile();
	if (QFile::exists(p))
	{
		emit currentFileChanged(p);
	}
}
