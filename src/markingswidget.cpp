#include "markingswidget.h"
#include "ui_markingswidget.h"

#include <QtGlobal>
#include <vector>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QTableView>
#include <QCheckBox>
#include <QLineEdit>
#include <QComboBox>
#include "markwidget.h"
#include "markersmodel.h"
#include "markdelegate.h"
#include "exportprocessor.h"
#include "markcolumns.h"

MarkingsWidget::MarkingsWidget(QWidget* parent)
	: QGroupBox(tr("Markings"), parent)
    , ui(new Ui::MarkingsWidget)
    , markersModel(nullptr)
	, markDelegate(new MarkDelegate(this))
    , currentPosition(0)
{
	ui->setupUi(this);
	ui->view->setItemDelegate(markDelegate);

	markDelegate->setObjectName("markDelegate");

	connect(markDelegate, &MarkDelegate::playClicked, this, &MarkingsWidget::onMarkPlayClicked);

	ui->ffmpegParameters->setEditable(true);
	ui->ffmpegParameters->addItem("");
	ui->ffmpegParameters->addItem("-c copy");
	ui->ffmpegParameters->addItem("-level:v 4.2 -b:v 50M");
	ui->ffmpegParameters->addItem("-c:v libx264 -preset slow -crf 18 -pix_fmt yuv420p");
	ui->ffmpegParameters->addItem("-c:v libvpx -b 50M");
}

void MarkingsWidget::setExportProcessor(ExportProcessor* expProc)
{
	markersModel = new MarkersModel(expProc, this);
	markersModel->setObjectName("markersModel");
	ui->view->setModel(markersModel);
	// The selection model changes when setting a model
	// see http://doc.qt.io/qt-4.8/qabstractitemview.html#setSelectionModel
	connect(ui->view->selectionModel(), &QItemSelectionModel::currentRowChanged, this, &MarkingsWidget::onSelectionChanged);
	connect(markersModel, &MarkersModel::dataChanged, this, &MarkingsWidget::onMarkersModelDataChanged);
}

void MarkingsWidget::setFile(QString file)
{
	markersModel->setFile(file);
}

void MarkingsWidget::setCurrentPosition(qint64 pos)
{
	currentPosition = pos;
	markDelegate->setCurrentPosition(pos);
}

void MarkingsWidget::on_add_clicked()
{
	markersModel->insertRow(markersModel->rowCount());
}

void MarkingsWidget::on_add5s_clicked()
{
	static const auto markTimespanMS = 5000;
	auto row = markersModel->rowCount();
	markersModel->insertRow(row);
	markersModel->setData(markersModel->index(row, COLS::START), qMax(qint64(0), currentPosition - markTimespanMS), Qt::EditRole);
	markersModel->setData(markersModel->index(row, COLS::END), currentPosition, Qt::EditRole);
	ui->view->scrollToBottom();
}

void MarkingsWidget::on_save_clicked()
{
	save();
}

void MarkingsWidget::save()
{
	markersModel->save();
}

void MarkingsWidget::on_load_clicked()
{
	markersModel->load();
}

void MarkingsWidget::on_concat_clicked()
{
	markersModel->exportConcat();
}

void MarkingsWidget::onSelectionChanged(const QModelIndex &current, const QModelIndex &/*previous*/)
{
	ui->outFfmpeg->setText(current.isValid() ? markersModel->getFfmpegCmd(current.row()) : QString());
}

void MarkingsWidget::on_ffmpegParameters_currentTextChanged(const QString& text)
{
	emit ffmpegParametersChanged(text);
	markersModel->setFfmpegParameters(text);
	auto index = ui->view->selectionModel()->currentIndex();
	ui->outFfmpeg->setText(index.isValid() ? markersModel->getFfmpegCmd(index.row()) : QString());
}

void MarkingsWidget::onMarkPlayClicked(int row)
{
	const Mark& mark = markersModel->getMark(row);
	emit playRange(mark.start, mark.end);
}

void MarkingsWidget::onMarkersModelDataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight, const QVector<int> &roles)
{
	Q_UNUSED(roles);
	auto current = ui->view->selectionModel()->currentIndex();
	auto valid = current.isValid()
			&& topLeft.isValid()
			&& bottomRight.isValid()
	        && topLeft.row() <= ui->view->selectionModel()->currentIndex().row()
	        && bottomRight.row() >= current.row();
	ui->outFfmpeg->setText(valid ? markersModel->getFfmpegCmd(current.row()) : QString());
}
