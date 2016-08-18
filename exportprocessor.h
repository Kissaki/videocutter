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
	static QString getFfmpegExtractArgs(const Mark& mark, QString inPath, bool copy);
	static QString getFfmpegConcatArgs(const std::vector<Mark>& marks, QString inPath);

	QProcess* extractProcess;

	explicit ExportProcessor(QObject *parent = 0);

	void exportMark(QString inPath, const Mark& mark, bool copy);
	void exportConcat(QString inPath, const std::vector<Mark> marks);

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

	static QString getCmdTimeArgs(const Mark& mark);
	static QString getCmdInArgs(QString inPath, const Mark& mark);
	static QString getCmdInArgs(QString inPath, const std::vector<Mark> marks);
};

#endif // EXPORTPROCESSOR_H
