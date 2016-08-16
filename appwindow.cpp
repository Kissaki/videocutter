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
#include <QJsonDocument>
#include <QJsonArray>
#include <QProcess>
#include <QSystemTrayIcon>
#include <QTextEdit>
#include "markingswidget.h"

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
	, outFfmpeg(new QLineEdit(this))
	, outExtract(new QPushButton(tr("Extract"), this))
	, markinsWidget(new MarkingsWidget(this))
	, markLabel(new QLabel(tr("Marker"), this))
	, markStart(new QLabel(this))
	, markEnd(new QLabel(this))
	, markSetStart(new QPushButton(tr("Start"), this))
	, markSetEnd(new QPushButton(tr("End"), this))
	, markingsSave(new QPushButton(tr("Save Markings"), this))
	, markingsLoad(new QPushButton(tr("Load Markings"), this))
	, extractProcess(new QProcess(this))
	, log(nullptr)
{
	openFile->setObjectName("openFile");
	sliderZoom->setObjectName("sliderZoom");
	sliderZoomRHS->setObjectName("sliderZoomRHS");
	sliderTime->setObjectName("sliderTime");
	player.setObjectName("player");
	markSetStart->setObjectName("markSetStart");
	markSetEnd->setObjectName("markSetEnd");
	markingsSave->setObjectName("markingsSave");
	markingsLoad->setObjectName("markingsLoad");
	outExtract->setObjectName("outExtract");
	extractProcess->setObjectName("extractProcess");

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

	auto layMarker = new QHBoxLayout();
	layMarker->addWidget(markLabel, 0, Qt::AlignLeft);
	layMarker->addWidget(markStart, 0, Qt::AlignLeft);
	layMarker->addWidget(markEnd, 0, Qt::AlignLeft);
	layMarker->addWidget(markSetStart, 0, Qt::AlignLeft);
	layMarker->addWidget(markSetEnd, 0, Qt::AlignLeft);
	layMarker->addStretch(1);
	layBottom->addLayout(layMarker);

	auto layMarkings = new QHBoxLayout();
	layMarkings->addWidget(markingsSave);
	layMarkings->addWidget(markingsLoad);
	layMarkings->addStretch(1);
	layBottom->addLayout(layMarkings);

	auto layOut = new QHBoxLayout();
	layOut->addWidget(outFfmpeg, 1);
	layOut->addWidget(outExtract);
	layBottom->addLayout(layOut);

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

void AppWindow::updateOut()
{
	updateOut(true);
}

QString AppWindow::getFfmpegExtractArgs(bool copy)
{
	int startMS = markStart->text().toInt();
	int endMS = markEnd->text().toInt();
	double startS = startMS / 1000.0;
	double endS = endMS / 1000.0;
	QString inPath = currentFile;
	QString outPath = inPath + "_" + QString::number(startMS) + "-" + QString::number(endMS) + ".mp4";
	QString c = copy ? "-c copy" : "";
	// https://ffmpeg.org/ffmpeg.html#Main-options
	// https://ffmpeg.org/ffmpeg-utils.html#time-duration-syntax
	QString out("-i \"%1\" -ss %2 -to %3 %4 \"%9\"");
	out = out
			.arg(inPath)
			.arg(QString::number(startS, 'f', 3))
			.arg(QString::number(endS, 'f', 3))
			.arg(c)
			.arg(outPath);
	return out;
}

void AppWindow::updateOut(bool copy)
{
	QString out("ffmpeg " + getFfmpegExtractArgs(copy));
	outFfmpeg->setText(out);
}

void AppWindow::on_markSetStart_clicked()
{
	auto val = sliderTime->value();
	auto v = QString::number(val);
	markStart->setText(v);

	Mark m;
	m.start = val;

	if (markEnd->text().isEmpty() || markEnd->text().toInt() < val)
	{
		markEnd->setText(v);
		m.end = val;
	}

	markings.append(m);

	updateOut();
}

void AppWindow::on_markSetEnd_clicked()
{
	auto v = QString::number(sliderTime->value());
	markEnd->setText(v);
	if (markStart->text().toInt() > sliderTime->value())
	{
		markStart->setText(v);
	}

	updateOut();
}

void AppWindow::on_outExtract_clicked()
{
	if (extractProcess->state() != QProcess::NotRunning)
	{
		qWarning() << "Extraction still running...";
		return;
	}
	qDebug() << "Starting extraction...";
	tray->showMessage("testtitle0", "Beginning extraction...");

	log = new QTextEdit(this);
	auto w = new QDialog(this);
	auto l = new QHBoxLayout(w);
	l->addWidget(log);
	w->setLayout(l);
	w->show();
	extractProcess->start("ffmpeg " + getFfmpegExtractArgs(true));
}

void AppWindow::on_markingsSave_clicked()
{
	QFile f(currentFile + ".markings.json");
	f.open(QIODevice::WriteOnly);
	QJsonArray o;
	markings.write(o);
	QJsonDocument json(o);
	f.write(json.toJson());
}

void AppWindow::on_markingsLoad_clicked()
{
	QFile f(currentFile + ".markings.json");
	f.open(QIODevice::ReadOnly);
	auto data = f.readAll();
	f.close();

	auto json = QJsonDocument::fromJson(data);
	markings.read(json.array());
}

void AppWindow::on_extractProcess_readyReadStandardError()
{
	auto data = extractProcess->readAllStandardError();
	log->append(data);
}

void AppWindow::on_extractProcess_finished(int /*exitCode*/, QProcess::ExitStatus /*exitStatus*/)
{
	tray->showMessage("Extraciton finished", "The extraction process finished.");
}

void AppWindow::on_extractProcess_readyReadStandardOutput()
{
	auto data = extractProcess->readAllStandardError();
	qWarning() << data;
}
