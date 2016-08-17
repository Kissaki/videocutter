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
	Mark getSelectedMark() const;
	void setCurrentPosition(qint64 pos);

signals:

public slots:

private slots:
	void on_add_clicked();
	void on_save_clicked();
	void on_load_clicked();
	void on_copy_stateChanged(int state);
	void onSelectionChanged(const QModelIndex &current, const QModelIndex &previous);

private:
	MarkersModel* markersModel;
	MarkDelegate* markDelegate;

	QTableView* view;
	QPushButton* add;
	QPushButton* save;
	QPushButton* load;
	// losless but unprecise copy of video data?
	QCheckBox* copy;
	QLineEdit* outFfmpeg;
};

#endif // MARKINGSWIDGET_H
