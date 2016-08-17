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

	setupLayout();

	setupMediaPlayer();
	player.setVideoOutput(videoWidget);

	QMetaObject::connectSlotsByName(this);
	connect(this, &AppWindow::currentFileChanged, this, &AppWindow::onCurrentFileChanged);

	emit currentFileChanged(QString::fromLatin1("E:/work/video/capture/plays/Overwatch/2016_07_15_21_54_03-ses.mp4"));
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

	auto layTime = new QHBoxLayout();
	layTime->addWidget(timeLow, 0, Qt::AlignLeft);
	layTime->addWidget(timeCurrent, 0, Qt::AlignLeft);
	layTime->addWidget(timeHigh, 0, Qt::AlignLeft);
	layTime->addStretch(1);
	layBottom->addLayout(layTime);

	layBottom->addWidget(markinsWidget);

	return layBottom;
}

void AppWindow::setupMediaPlayer()
{
	auto playlist = new QMediaPlaylist(&player);
	player.setPlaylist(playlist);
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
//	if (position >= sliderTime->maximum() && player.state() == QMediaPlayer::PlayingState)
//	{
//		player.pause();
//	}
	if (!sliderTime->isSliderDown())
	{
		sliderTime->setValue(position);
	}
	markinsWidget->setCurrentPosition(position);
	timeCurrent->setText(QString::number(position));
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
