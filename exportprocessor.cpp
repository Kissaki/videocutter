#include "exportprocessor.h"

#include <QLineEdit>
#include <QDebug>
#include <QDialog>
#include <QHBoxLayout>
#include <QTextEdit>
#include "markings.h"

ExportProcessor::ExportProcessor(QObject *parent)
	: QObject(parent)
	, extractProcess(new QProcess(this))
	, logWidget(nullptr)
{
	extractProcess->setObjectName("extractProcess");

	QMetaObject::connectSlotsByName(this);
}

void ExportProcessor::exportMark(QString inPath, const Mark& mark, bool copy)
{
	if (extractProcess->state() != QProcess::NotRunning)
	{
		qWarning() << "Extraction still running...";
		return;
	}
	qDebug() << "Starting extraction...";
//	tray->showMessage("testtitle0", "Beginning extraction...");

	auto w = new QDialog();
	logWidget = new QTextEdit(w);
	auto l = new QHBoxLayout(w);
	l->addWidget(logWidget);
	w->setLayout(l);
	w->show();

	extractProcess->start("ffmpeg " + getFfmpegExtractArgs(mark, inPath, copy));
}

QString ExportProcessor::getFfmpegExtractArgs(const Mark& mark, QString inPath, bool copy)
{
	int startMS = mark.start;
	int endMS = mark.end;
	double startS = startMS / 1000.0;
	double endS = endMS / 1000.0;
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

void ExportProcessor::on_extractProcess_readyReadStandardError()
{
	auto data = extractProcess->readAllStandardError();
	logWidget->append(data);
	emit log(data);
}

void ExportProcessor::on_extractProcess_finished(int /*exitCode*/, QProcess::ExitStatus /*exitStatus*/)
{
	//TODO
//	tray->showMessage("Extraciton finished", "The extraction process finished.");
}

void ExportProcessor::on_extractProcess_readyReadStandardOutput()
{
	auto data = extractProcess->readAllStandardError();
	qWarning() << data;
}
