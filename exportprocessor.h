#ifndef EXPORTPROCESSOR_H
#define EXPORTPROCESSOR_H

#include <QObject>
#include <QProcess>
#include <QTextEdit>

struct Mark;

class ExportProcessor : public QObject
{
	Q_OBJECT
public:
	QProcess* extractProcess;

	explicit ExportProcessor(QObject *parent = 0);

	QString getFfmpegExtractArgs(const Mark& mark, const QString& inPath);
	QString getFfmpegConcatArgs(const std::vector<Mark>& marks, QString inPath);
	void setFfmpegParameters(const QString& parameters);
	void exportMark(const QString& inPath, const Mark& mark);
	void exportConcat(const QString& inPath, const std::vector<Mark> marks);

signals:
	void log(QString s) const;

public slots:

private slots:
	void on_extractProcess_readyReadStandardError();
	void on_extractProcess_readyReadStandardOutput();
	void on_extractProcess_finished(int exitCode, QProcess::ExitStatus exitStatus);

private:
	ExportProcessor* exportProcessor;
	QTextEdit* logWidget;
	QString ffmpegParameters;

	static QString getCmdTimeArgs(const Mark& mark);
	static QString getCmdInArgs(QString inPath, const Mark& mark);
	static QString getCmdInArgs(QString inPath, const std::vector<Mark> marks);
};

#endif // EXPORTPROCESSOR_H
