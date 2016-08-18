#include "appwindow.h"
#include <QHBoxLayout>
#include <QPushButton>
#include <QVideoWidget>
#include <QSlider>
#include <QMediaPlayer>
#include <QMediaPlaylist>
#include <QFileDialog>
#include <QLabel>
#include <QLineEdit>
#include <QProcess>
#include <QSystemTrayIcon>
#include <QTextEdit>
#include <QDoubleSpinBox>
#include "markingswidget.h"
#include "markersmodel.h"
#include "exportprocessor.h"

AppWindow::AppWindow(QSystemTrayIcon* tray, QWidget *parent)
	: QWidget(parent)
	, tray(tray)
	, duration(new QLabel("0", this))
	, timeLow(new QLabel("0", this))
	, timeHigh(new QLabel("0", this))
	, timeCurrent(new QLabel("0", this))
	, sliderZoom(new QSlider(Qt::Horizontal, this))
	, sliderZoomRHS(new QSlider(Qt::Horizontal, this))
	, sliderTime(new QSlider(Qt::Horizontal, this))
	, playbackSpeed(new QDoubleSpinBox(this))
	, playerVolume(new QSlider(Qt::Horizontal, this))
	, playerPlayPause(new QPushButton("⏯", this))
	, videoWidget(new QVideoWidget(this))
	, openFile(new QPushButton("Open"))
	, player(this)
	, markinsWidget(new MarkingsWidget(new ExportProcessor(this), this))
{
	openFile->setObjectName("openFile");
	sliderZoom->setObjectName("sliderZoom");
	sliderZoomRHS->setObjectName("sliderZoomRHS");
	sliderTime->setObjectName("sliderTime");
	player.setObjectName("player");
	playbackSpeed->setObjectName("playbackSpeed");
	playerVolume->setObjectName("playerVolume");
	playerPlayPause->setObjectName("playerPlayPause");

	setupLayout();

	setupMediaPlayer();
	player.setVideoOutput(videoWidget);
	playbackSpeed->setSingleStep(0.5);
	// With higher values than 4.0 QMediaPlayer simply sets it to 1.0 instead. https://bugreports.qt.io/browse/QTBUG-55354
	// According to documentation of QMediaPlayer, negative values should work, but do not. In case that's format or system specific, allow negative numbers for now.
	playbackSpeed->setRange(-4.0, 4.0);
	playbackSpeed->setValue(player.playbackRate());
	playerVolume->setRange(0, 100);
	playerVolume->setValue(player.volume());

	QMetaObject::connectSlotsByName(this);
	connect(this, &AppWindow::currentFileChanged, this, &AppWindow::onCurrentFileChanged);
	connect(markinsWidget, &MarkingsWidget::playRange, this, &AppWindow::onPlayRange);
}

void AppWindow::onPlayRange(int timeStartMS, int timeEndMS)
{
	player.pause();
	sliderZoom->setValue(timeStartMS);
	sliderZoomRHS->setValue(timeEndMS);
	player.setPosition(timeStartMS);
	player.play();
}

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
	layBottom->addWidget(sliderZoom);
	layBottom->addWidget(sliderZoomRHS);
	layBottom->addWidget(sliderTime);

	auto layTimeControls = new QHBoxLayout();
	auto layTime = new QHBoxLayout();
	auto gbTime = new QGroupBox(tr("Time [ms]"), this);
	layTime->addWidget(new QLabel(tr("Range Beginning"), gbTime));
	layTime->addWidget(timeLow, 0, Qt::AlignLeft);
	layTime->addWidget(new QLabel(tr("Current Time"), gbTime));
	layTime->addWidget(timeCurrent, 0, Qt::AlignLeft);
	layTime->addWidget(new QLabel(tr("Range End"), gbTime));
	layTime->addWidget(timeHigh, 0, Qt::AlignLeft);
	layTime->addStretch(1);
	gbTime->setLayout(layTime);
	layTimeControls->addWidget(gbTime);
	auto gbControls = new QGroupBox("Playback", this);
	auto layControls = new QHBoxLayout();
	layControls->addWidget(playerPlayPause);
	layControls->addWidget(new QLabel(tr("Speed"), gbControls));
	layControls->addWidget(playbackSpeed);
	layControls->addWidget(new QLabel(tr("Volume"), gbControls));
	playerVolume->setMaximumWidth(60);
	layControls->addWidget(playerVolume);
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
	player.setNotifyInterval(100);
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
	currentFile = newFile;
	markinsWidget->setFile(currentFile);
	player.playlist()->clear();
	player.playlist()->addMedia(QUrl::fromLocalFile(newFile));
	player.playlist()->setCurrentIndex(1);

	sliderZoom->setMaximum(player.duration());
	sliderZoom->setValue(0);
	sliderZoomRHS->setMaximum(player.duration());
	sliderZoomRHS->setValue(sliderZoomRHS->maximum());
	sliderTime->setMinimum(sliderZoom->value());
	sliderTime->setMaximum(sliderZoomRHS->value());
	duration->setText(QString().sprintf("%d", player.duration()));

	player.play();
}

void AppWindow::on_sliderZoom_valueChanged(int v)
{
	sliderZoomRHS->setMinimum(v);
	sliderTime->setMinimum(v);
}

void AppWindow::on_sliderZoomRHS_valueChanged(int v)
{
	sliderZoom->setMaximum(v);
	sliderTime->setMaximum(v);
}

void AppWindow::on_player_durationChanged(qint64 d)
{
	sliderZoomRHS->setMaximum(d);
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
	timeCurrent->setText(QString::number(position));

	if (sliderZoomRHS->value() > 0 && position >= sliderZoomRHS->value() && player.state() == QMediaPlayer::PlayingState)
	{
		player.setPosition(sliderZoom->value());
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
	timeLow->setText(QString::number(min));
	timeHigh->setText(QString::number(max));
}

void AppWindow::on_playbackSpeed_valueChanged(double v)
{
	player.setPlaybackRate(v);
}

void AppWindow::on_player_playbackRateChanged(qreal rate)
{
	playbackSpeed->setValue(rate);
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
