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
	explicit ExportProcessor(QObject *parent = 0);

	QString getFfmpegExtractArgs(const Mark& mark, const QString& inPath, const QString& targetFilePath);
	QString getFfmpegConcatArgs(const std::vector<Mark>& marks, QString inPath);
	QString getExtractTargetPath(const QString& sourceFilePath, const Mark& mark);
	void setFfmpegParameters(const QString& parameters);
	void exportMark(const QString& inPath, const Mark& mark);
	void exportConcat(const QString& inPath, const std::vector<Mark> marks);
	bool isActive();

signals:
	void starting(QString command);
	void statusInfo(QString s);
	void finished(QString target, int sizeMB);

public slots:
	void abort();

private slots:
	void on_extractProcess_readyReadStandardError();
	void on_extractProcess_readyReadStandardOutput();
	void on_extractProcess_finished(int exitCode, QProcess::ExitStatus exitStatus);

private:
	QProcess* extractProcess;
	QString ffmpegParameters;
	QString outPath;

	static QString getCmdTimeArgs(const Mark& mark);
	static QString getCmdInArgs(QString inPath, const Mark& mark);
	static QString getCmdInArgs(QString inPath, const std::vector<Mark> marks);
};

#endif // EXPORTPROCESSOR_H
