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

	auto cmd = "ffmpeg -y " + getFfmpegExtractArgs(mark, inPath, copy);
	logWidget->append(cmd);
	extractProcess->start(cmd);
}

void ExportProcessor::exportConcat(QString inPath, const std::vector<Mark> marks)
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

	auto cmd = "ffmpeg -y " + getFfmpegConcatArgs(marks, inPath);
	logWidget->append(cmd);
	extractProcess->start(cmd);
}

// https://ffmpeg.org/ffmpeg-utils.html#time-duration-syntax
QString ExportProcessor::getCmdTimeArgs(const Mark& mark)
{
	int startMS = mark.start;
	int endMS = mark.end;
	int durationMS = endMS - startMS;
	double startS = startMS / 1000.0;
	double durationS = durationMS / 1000.0;
	return QString("-ss %1 -t %2")
			.arg(QString::number(startS, 'f', 3))
			.arg(QString::number(durationS, 'f', 3))
			;
}

QString ExportProcessor::getCmdInArgs(QString inPath, const Mark& mark)
{
	return QString("%1 -i \"%2\"")
			.arg(getCmdTimeArgs(mark))
			.arg(inPath)
			;
}

QString ExportProcessor::getCmdInArgs(QString inPath, const std::vector<Mark> marks)
{
	QString out;
	for (auto mark : marks)
	{
		out += getCmdInArgs(inPath, mark) + " ";
	}
	return out;
}

QString ExportProcessor::getFfmpegExtractArgs(const Mark& mark, QString inPath, bool copy)
{
	QString outPath = inPath + "_" + QString::number(mark.start) + "-" + QString::number(mark.end) + ".mp4";
	QString c = copy ? "-c copy" : "";
	// https://ffmpeg.org/ffmpeg.html#Main-options
	return QString("%1 %4 \"%9\"")
			.arg(getCmdInArgs(inPath, mark))
			.arg(c)
			.arg(outPath)
			;
}

QString ExportProcessor::getFfmpegConcatArgs(const std::vector<Mark>& marks, QString inPath)
{
	//TODO: Make quality configurable
	auto quality = QString("-level:v 4.2 -b:v 50M");
	auto filter = QString("-filter_complex concat=n=%2:a=1")
			.arg(QString::number(marks.size()));
	auto options = QString("%1 %2").arg(filter)
			.arg(quality);
	QString outPath = inPath + "_concat-" + QString::number(marks.size()) + ".mp4";
	// https://ffmpeg.org/ffmpeg.html#Main-options
	return QString("%1 %2 \"%9\"")
			.arg(getCmdInArgs(inPath, marks))
			.arg(options)
			.arg(outPath)
			;
}

void ExportProcessor::on_extractProcess_readyReadStandardError()
{
	auto data = extractProcess->readAllStandardError();
	logWidget->append(data);
	emit log(data);
}

void ExportProcessor::on_extractProcess_finished(int exitCode, QProcess::ExitStatus exitStatus)
{
	//TODO
//	tray->showMessage("Extraciton finished", "The extraction process finished.");
	qDebug() << "Extraction finished" << exitCode << exitStatus;
}

void ExportProcessor::on_extractProcess_readyReadStandardOutput()
{
	auto data = extractProcess->readAllStandardError();
	qWarning() << data;
}
