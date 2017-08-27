#ifndef MARKINGSWIDGET_H
#define MARKINGSWIDGET_H

#include <QGroupBox>

#include "markings.h"

class QPushButton;
class QVBoxLayout;
class QTableView;
class MarkersModel;
class MarkDelegate;
class ExportProcessor;
class QCheckBox;
class QLineEdit;
class QComboBox;
class QWidget;

class MarkingsWidget : public QGroupBox
{
	Q_OBJECT
public:
	explicit MarkingsWidget(QWidget *parent = 0);

	void setExportProcessor(ExportProcessor* expProc);
	void setFile(QString file);
	void setCurrentPosition(qint64 pos);

signals:
	void playRange(int timeStartMS, int timeEndMS);
	void ffmpegParametersChanged(QString parameters);

public slots:

private slots:
	void on_add_clicked();
	void on_add5s_clicked();
	void on_save_clicked();
	void on_load_clicked();
	void on_concat_clicked();
	void on_ffmpegParameters_currentTextChanged(const QString& text);
	void onSelectionChanged(const QModelIndex &current, const QModelIndex &previous);
	void onMarkPlayClicked(int row);
	// Has to be connected manually because the model is set after construction
	void onMarkersModelDataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight, const QVector<int> &roles);

private:
	MarkersModel* markersModel;
	MarkDelegate* markDelegate;
	qint64 currentPosition;

	QTableView* view;
	QPushButton* add;
	QPushButton* add5s;
	QPushButton* save;
	QPushButton* load;
	QPushButton* concat;
	QComboBox* ffmpegParameters;
	QLineEdit* outFfmpeg;

	void setupLayout();
};

#endif // MARKINGSWIDGET_H
