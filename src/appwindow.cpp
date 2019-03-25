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
#include <QMessageBox>

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
	ui->uiNotifyRate->setValue(DEFAULT_UI_NOTIFY_RATE);

	ui->timeLow->setRange(0, 0);
	ui->timeHigh->setRange(0, 0);
	ui->timeCurrent->setRange(0, 0);

	setAcceptDrops(true);

	connect(ui->markinsWidget, &MarkingsWidget::playRange, this, &AppWindow::onPlayRange);
	connect(player, &QMediaPlayer::stateChanged, this, &AppWindow::updateTimeLabels);

	// Default volume
	ui->playerVolume->setValue(60);
}

void AppWindow::closeFile()
{
	qDebug() << "closing file…";
	player->stop();
	resetPlayerControls();
	currentFile = QString();
	ui->markinsWidget->setFile(currentFile);
	player->playlist()->clear();
}

void AppWindow::openFile(const QString& newFile)
{
	closeFile();

	qDebug() << "opening file" << newFile;

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
		openFile(p);
	}
}

void AppWindow::keyPressEvent(QKeyEvent *key)
{
	if (key->key() == Qt::Key_F2)
	{
		FilenameDialog d(currentFile, this);
		if (d.exec() == QDialog::Accepted) {
			auto oldPath = currentFile;

			closeFile();

			QFileInfo fi(oldPath);
			QString newPath = fi.canonicalPath() + QDir::separator() + d.getNewFilename();
			QFile f(oldPath);
			bool success = f.rename(newPath);
			qDebug() << "Renaming " << oldPath << " to " << newPath << "; Success: " << success;
			openFile(newPath);
		}
	}
	else if (key->key() == Qt::Key_Delete)
	{
		QFileInfo fi(currentFile);
		QString title = tr("File Removal");
		QString text = tr("Are you sure you want to delete the file %1?").arg(fi.fileName());
		auto res = QMessageBox::question(this, title, text, QMessageBox::Yes|QMessageBox::Cancel);
		if (res == QMessageBox::Yes)
		{
			auto oldPath = currentFile;
			openFile(QString());
			QFile f(oldPath);
			bool success = f.remove();
			qDebug() << "Removing file " << oldPath << "; Success: " << success;
		}
	}
}

void AppWindow::closeEvent(QCloseEvent *event)
{
	ui->markinsWidget->save();

	if (exportProcessor->isActive())
	{
		auto userChoice = QMessageBox::question(this, tr("Export in progress"), tr("An export is still in progress. Are you sure you want to close the application now instead of waiting for the export to finish?"), QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
		if (userChoice == QMessageBox::No)
		{
			event->ignore();
			return;
		}
		exportProcessor->abort();
	}

	QMainWindow::closeEvent(event);
}

void AppWindow::on_actionDelete_Next_triggered()
{
	auto fileToDelete = currentFile;
	if (!loadNextFile()) {
		closeFile();
	}
	deleteFile(fileToDelete);
}

void AppWindow::deleteFile(const QString& path)
{
	if (path.isEmpty())
	{
		qDebug() << "Delete called with no file active.";
		return;
	}
	if (!QFile::remove(path)) {
		qDebug() << "Failed to remove file " << path;
	}
}

void AppWindow::on_action_Open_triggered()
{
	auto newFilePath = QFileDialog::getOpenFileName(
	            this,
	            tr("Open video file"),
	            QString(),
	            tr("Video Files (*.mp4 *.mkv *.avi *.wmv)"));
	if (!newFilePath.isEmpty())
	{
		openFile(newFilePath);
	}
}

void AppWindow::on_actionDelete_triggered()
{
	auto fileToDelete = currentFile;
	closeFile();
	deleteFile(fileToDelete);
}

bool AppWindow::loadNextFile()
{
	auto fi = new QFileInfo(currentFile);
	auto files = fi->dir().entryInfoList(QDir::Filter::Files);
	auto found = false;
	for (QFileInfo f : files) {
		if (found) {
			openFile(f.absoluteFilePath());
			return true;
		}
		if (f.absoluteFilePath() == currentFile) {
			found = true;
		}
	}
	return false;
}

void AppWindow::on_actionNext_triggered()
{
	auto fi = new QFileInfo(currentFile);
	auto files = fi->dir().entryInfoList(QDir::Filter::Files);
	auto found = false;
	for (QFileInfo f : files) {
		if (found) {
			openFile(f.absoluteFilePath());
			return;
		}
		if (f.absoluteFilePath() == currentFile) {
			found = true;
		}
	}
}

void AppWindow::on_actionPrevious_triggered()
{
	auto fi = new QFileInfo(currentFile);
	auto files = fi->dir().entryInfoList(QDir::Filter::Files);

	auto hasPrevious = false;
	QFileInfo previous;
	for (QFileInfo f : files) {
		if (f.absoluteFilePath() == currentFile) {
			if (hasPrevious){
				openFile(previous.absoluteFilePath());
				return;
			} else if (!files.isEmpty()) {
				openFile(files.last().absoluteFilePath());
				return;
			}
			// No previous file to load. Reload existing one.
			openFile(currentFile);
			return;
		}
		hasPrevious = true;
		previous = f;
	}
	// No previous file to load. Reload existing one.
	openFile(currentFile);
	return;
}
