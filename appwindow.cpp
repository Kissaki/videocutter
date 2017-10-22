#include "appwindow.h"
#include "ui_appwindow.h"

#include <QDoubleSpinBox>
#include <QFile>
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
#include <QMediaPlayer>

#include "markings.h"
#include "dialogs/filenamedialog.h"
#include "markingswidget.h"
#include "exportprocessor.h"

AppWindow::AppWindow(QSystemTrayIcon* tray, QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::AppWindow)
	, skipDistance(5000)
	, tray(tray)
    , player(new QMediaPlayer(this))
    , exportProcessor(new ExportProcessor(this))
{
	player->setObjectName("player");
	exportProcessor->setObjectName("exportProcessor");
	ui->setupUi(this);

	ui->markinsWidget->setExportProcessor(exportProcessor);

	const int TIME_STEPSIZE = 100;
	ui->timeLow->setSingleStep(TIME_STEPSIZE);
	ui->timeHigh->setSingleStep(TIME_STEPSIZE);
	ui->timeCurrent->setSingleStep(TIME_STEPSIZE);

	setupMediaPlayer();
	resetPlayerControls();
	ui->playbackSpeed->setSingleStep(0.5);
	// With higher values than 4.0 QMediaPlayer simply sets it to 1.0 instead. https://bugreports.qt.io/browse/QTBUG-55354
	// According to documentation of QMediaPlayer, negative values should work, but do not. In case that's format or system specific, allow negative numbers for now.
	ui->playbackSpeed->setRange(-4.0, 4.0);
	ui->playbackSpeed->setValue(1.0);
	ui->playerVolume->setRange(0, 100);
	ui->playerVolume->setValue(player->volume());
	ui->uiNotifyRate->setRange(100, 1000);
	ui->uiNotifyRate->setSingleStep(100);
	ui->uiNotifyRate->setValue(1000);

	ui->timeLow->setRange(0, 0);
	ui->timeHigh->setRange(0, 0);
	ui->timeCurrent->setRange(0, 0);

	setAcceptDrops(true);

	connect(ui->markinsWidget, &MarkingsWidget::playRange, this, &AppWindow::onPlayRange);
	connect(player, &QMediaPlayer::stateChanged, this, &AppWindow::updateTimeLabels);
}

void AppWindow::setFilepath(const QString& newFile)
{
	qDebug() << "opening file" << newFile;

	player->stop();
	resetPlayerControls();

	currentFile = newFile;
	ui->markinsWidget->setFile(currentFile);
	player->playlist()->clear();
	player->playlist()->addMedia(QUrl::fromLocalFile(newFile));
	player->playlist()->setCurrentIndex(1);

	player->play();
}

void AppWindow::onPlayRange(int timeStartMS, int timeEndMS)
{
	player->pause();
	ui->sliderZoomLHS->setValue(timeStartMS);
	ui->sliderZoomRHS->setValue(timeEndMS);
	player->setPosition(timeStartMS);
	player->play();
}

void AppWindow::on_markinsWidget_ffmpegParametersChanged(QString parameters)
{

}

void AppWindow::setupMediaPlayer()
{
	auto playlist = new QMediaPlaylist(player);
	playlist->setPlaybackMode(QMediaPlaylist::CurrentItemInLoop);
	player->setPlaylist(playlist);
	player->setVideoOutput(ui->videoWidget);
}

void AppWindow::resetPlayerControls()
{
	ui->duration->setText(QString().sprintf("%d", 0));
	ui->timeLow->setRange(0, 0);
	ui->timeHigh->setRange(0, 0);
	ui->timeCurrent->setRange(0, 0);
	ui->sliderZoomLHS->setMaximum(0);
	ui->sliderZoomRHS->setMaximum(0);
	ui->sliderTime->setRange(0, 0);
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
		setFilepath(newFilePath);
	}
}

void AppWindow::on_exportProcessor_statusInfo(QString s)
{
	ui->statusbar->showMessage(s);
}

void AppWindow::on_exportProcessor_finished(QString target, int sizeMB)
{
	ui->statusbar->showMessage(QString("Finished %1 (%2MB)").arg(target).arg(sizeMB));
}

void AppWindow::on_timeLow_valueChanged(int v)
{
	ui->sliderZoomLHS->setValue(v);

	ui->timeHigh->setMinimum(v);
	ui->timeCurrent->setMinimum(ui->timeLow->value());
}

void AppWindow::on_sliderZoomLHS_valueChanged(int v)
{
	ui->timeLow->setValue(v);

	ui->sliderZoomRHS->setMinimum(v);
	ui->sliderTime->setMinimum(v);
}

void AppWindow::on_timeHigh_valueChanged(int v)
{
	ui->sliderZoomRHS->setValue(v);

	ui->timeLow->setMaximum(v);
	ui->timeCurrent->setMaximum(v);
}

void AppWindow::on_sliderZoomRHS_valueChanged(int v)
{
	ui->timeHigh->setValue(v);

	ui->sliderZoomLHS->setMaximum(v);
	ui->sliderTime->setMaximum(v);
}

void AppWindow::on_player_durationChanged(qint64 d)
{
	ui->timeLow->setRange(0, d);
	ui->timeHigh->setRange(0, d);
	ui->timeCurrent->setRange(0, d);

	ui->sliderZoomRHS->setMaximum(d);
	ui->timeHigh->setValue(d);
	ui->sliderZoomRHS->setValue(d);

	ui->duration->setText(QString::number(d));
}

void AppWindow::on_player_positionChanged(qint64 position)
{
	if (!ui->sliderTime->isSliderDown())
	{
		ui->sliderTime->setValue(position);
	}
	ui->markinsWidget->setCurrentPosition(position);
	ui->timeCurrent->setValue(position);

	if (ui->sliderZoomRHS->value() > 0 && position >= ui->sliderZoomRHS->value() && player->state() == QMediaPlayer::PlayingState)
	{
		player->setPosition(ui->sliderZoomLHS->value());
	}
}

void AppWindow::updateTimeLabels()
{
	ui->markinsWidget->setCurrentPosition(player->position());
	ui->timeCurrent->setValue(player->position());
}

void AppWindow::on_timeCurrent_valueChanged(int v)
{
	// We handle user input or player position change.
	// In case of user input, we set the player position.
	if (player->position() != v)
	{
		player->setPosition(v);
	}
}

// Use cases:
// * tracking/user seeking
// * player playback (incremental)
void AppWindow::on_sliderTime_valueChanged(int v)
{
	if (ui->sliderTime->isSliderDown())
	{
		player->setPosition(v);
	}
}

void AppWindow::on_sliderTime_rangeChanged(int min, int max)
{
	auto v = ui->sliderTime->value();
	if (v < min)
	{
		ui->sliderTime->setValue(min);
	} else if (v > max)
	{
		ui->sliderTime->setValue(max);
	}
	ui->timeLow->setValue(min);
	ui->timeHigh->setValue(max);
}

void AppWindow::on_playbackSpeed_valueChanged(double v)
{
	player->setPlaybackRate(v);
}

void AppWindow::on_player_playbackRateChanged(qreal rate)
{
	ui->playbackSpeed->setValue(rate);
}

void AppWindow::on_player_notifyIntervalChanged(int ms)
{
	ui->uiNotifyRate->setValue(ms);
}

void AppWindow::on_playerVolume_valueChanged(int v)
{
	player->setVolume(v);
}

void AppWindow::on_playerPlayPause_clicked()
{
	if (player->state() == QMediaPlayer::PlayingState)
	{
		player->pause();
	}
	else
	{
		player->play();
	}
}

void AppWindow::on_playerSkipToStart_clicked()
{
	player->setPosition(ui->timeLow->value());
}

void AppWindow::on_playerSkipBackward_clicked()
{
	player->setPosition(player->position() - skipDistance);
}

void AppWindow::on_playerSkipForward_clicked()
{
	player->setPosition(player->position() + skipDistance);
}

void AppWindow::on_uiNotifyRate_valueChanged(int v)
{
	player->setNotifyInterval(v);
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
		setFilepath(p);
	}
}

void AppWindow::keyPressEvent(QKeyEvent *key)
{
	if (key->key() == Qt::Key_F2)
	{
		FilenameDialog d(currentFile, this);
		if (d.exec() == QDialog::Accepted) {
			QFile f(currentFile);
			auto newPath = d.getNewFilename();
			qDebug() << "Renaming " << currentFile << " to " << newPath;
			f.rename(newPath);
			setFilepath(newPath);
		}
	}
}
