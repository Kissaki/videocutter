#include "markingswidget.h"

#include <QtGlobal>
#include <vector>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QTableView>
#include <QCheckBox>
#include <QLineEdit>
#include "markwidget.h"
#include "markersmodel.h"
#include "markdelegate.h"
#include "exportprocessor.h"
#include "markcolumns.h"

MarkingsWidget::MarkingsWidget(ExportProcessor* expProc, QWidget* parent)
	: QGroupBox(tr("Markings"), parent)
	, markersModel(new MarkersModel(expProc, this))
	, markDelegate(new MarkDelegate(this))
	, currentPosition(0)
	, view(new QTableView)
	, add(new QPushButton(tr("Add"), this))
	, add5s(new QPushButton(tr("Add [5s]"), this))
	, save(new QPushButton(tr("Save"), this))
	, load(new QPushButton(tr("Load"), this))
	, concat(new QPushButton(tr("Concat"), this))
	, copy(new QCheckBox(tr("Copy"), this))
	, outFfmpeg(new QLineEdit(this))
{
	add->setObjectName("add");
	add5s->setObjectName("add5s");
	save->setObjectName("save");
	load->setObjectName("load");
	copy->setObjectName("copy");
	concat->setObjectName("concat");
	markersModel->setObjectName("markersModel");

	view->setModel(markersModel);
	view->setItemDelegate(markDelegate);
	copy->setToolTip(tr("Lossless copying of video data but with unprecisely cut beginning"));

	auto l = new QVBoxLayout(this);

	auto layActions = new QHBoxLayout;
	layActions->addWidget(add);
	layActions->addWidget(add5s);
	layActions->addWidget(save);
	layActions->addWidget(load);
	layActions->addWidget(copy);
	layActions->addWidget(outFfmpeg, 1);
	layActions->addWidget(concat);

	l->addWidget(view);
	l->addLayout(layActions);

	QMetaObject::connectSlotsByName(this);
	connect(view->selectionModel(), &QItemSelectionModel::currentRowChanged, this, &MarkingsWidget::onSelectionChanged);
	connect(markDelegate, &MarkDelegate::playClicked, this, &MarkingsWidget::onMarkPlayClicked);
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
	view->scrollToBottom();
}

void MarkingsWidget::on_save_clicked()
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

void MarkingsWidget::on_copy_stateChanged(int state)
{
	markersModel->setCopy(state == Qt::Checked);
}

void MarkingsWidget::onSelectionChanged(const QModelIndex &current, const QModelIndex &/*previous*/)
{
	auto copyC = copy->checkState() == Qt::Checked;
	outFfmpeg->setText(markersModel->getFfmpegCmd(current.row(), copyC));
}

void MarkingsWidget::onMarkPlayClicked(int row)
{
	Mark& mark = markersModel->getMark(row);
	emit playRange(mark.start, mark.end);
}
