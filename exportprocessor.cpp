#include "exportprocessor.h"

#include <QLineEdit>
#include <QDebug>
#include <QFile>
#include <QHBoxLayout>
#include <QTextEdit>
#include "markings.h"

ExportProcessor::ExportProcessor(QObject *parent)
	: QObject(parent)
	, extractProcess(new QProcess(this))
{
	extractProcess->setObjectName("extractProcess");

	QMetaObject::connectSlotsByName(this);
}

void ExportProcessor::setFfmpegParameters(const QString& parameters)
{
	ffmpegParameters = parameters;
}

void ExportProcessor::exportMark(const QString& inPath, const Mark& mark)
{
	if (extractProcess->state() != QProcess::NotRunning)
	{
		qWarning() << "Extraction still running...";
		return;
	}
	qDebug() << "Starting extraction...";

	auto cmd = "ffmpeg -y " + getFfmpegExtractArgs(mark, inPath);
	emit starting(cmd);
	extractProcess->start(cmd);
}

void ExportProcessor::exportConcat(const QString& inPath, const std::vector<Mark> marks)
{
	if (extractProcess->state() != QProcess::NotRunning)
	{
		qWarning() << "Extraction still running...";
		return;
	}
	qDebug() << "Starting extraction...";
//	tray->showMessage("testtitle0", "Beginning extraction...");

	auto cmd = "ffmpeg -y " + getFfmpegConcatArgs(marks, inPath);
	emit starting(cmd);
	extractProcess->start(cmd);
}

bool ExportProcessor::isActive()
{
	return extractProcess->state() != QProcess::NotRunning;
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

QString ExportProcessor::getFfmpegExtractArgs(const Mark& mark, const QString& sourceFilePath)
{
	outPath = sourceFilePath + "_" + QString::number(mark.start) + "-" + QString::number(mark.end) + ".mp4";
	// https://ffmpeg.org/ffmpeg.html#Main-options
	return QString("%1 %4 \"%9\"")
			.arg(getCmdInArgs(sourceFilePath, mark))
			.arg(ffmpegParameters)
			.arg(outPath)
			;
}

QString ExportProcessor::getFfmpegConcatArgs(const std::vector<Mark>& marks, QString inPath)
{
	auto filter = QString("-filter_complex concat=n=%2:a=1")
			.arg(QString::number(marks.size()));
	auto options = QString("%1 %2")
			.arg(filter)
			.arg(ffmpegParameters);
	QString outPath = inPath + "_concat-" + QString::number(marks.size()) + ".mp4";
	// https://ffmpeg.org/ffmpeg.html#Main-options
	return QString("%1 %2 \"%9\"")
			.arg(getCmdInArgs(inPath, marks))
			.arg(options)
			.arg(outPath)
			;
}

void ExportProcessor::abort()
{
	extractProcess->terminate();
}

void ExportProcessor::on_extractProcess_readyReadStandardError()
{
	auto data = extractProcess->readAllStandardError();
	emit statusInfo(data);
}

void ExportProcessor::on_extractProcess_finished(int exitCode, QProcess::ExitStatus exitStatus)
{
	qDebug() << "Extraction finished" << exitCode << exitStatus;
	emit finished(outPath, QFile(outPath).size());
}

void ExportProcessor::on_extractProcess_readyReadStandardOutput()
{
	auto data = extractProcess->readAllStandardError();
	qWarning() << data;
}
