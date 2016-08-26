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

class MarkingsWidget : public QGroupBox
{
	Q_OBJECT
public:
	explicit MarkingsWidget(ExportProcessor* expProc, QWidget *parent = 0);

	void setFile(QString file);
	void setCurrentPosition(qint64 pos);

signals:
	void playRange(int timeStartMS, int timeEndMS);

public slots:

private slots:
	void on_add_clicked();
	void on_add5s_clicked();
	void on_save_clicked();
	void on_load_clicked();
	void on_concat_clicked();
	void on_copy_stateChanged(int state);
	void onSelectionChanged(const QModelIndex &current, const QModelIndex &previous);
	void onMarkPlayClicked(int row);

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
	// losless but unprecise copy of video data?
	QCheckBox* copy;
	QLineEdit* outFfmpeg;
};

#endif // MARKINGSWIDGET_H
