#include "markingswidget.h"

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

MarkingsWidget::MarkingsWidget(ExportProcessor* expProc, QWidget* parent)
	: QGroupBox(tr("Markings"), parent)
	, view(new QTableView)
	, add(new QPushButton(tr("Add"), this))
	, save(new QPushButton(tr("Save"), this))
	, load(new QPushButton(tr("Load"), this))
	, copy(new QCheckBox(tr("copy"), this))
	, outFfmpeg(new QLineEdit(this))
	, markersModel(new MarkersModel(expProc, this))
	, markDelegate(new MarkDelegate(this))
{
	add->setObjectName("add");
	save->setObjectName("save");
	load->setObjectName("load");
	copy->setObjectName("copy");
	markersModel->setObjectName("markersModel");

	view->setModel(markersModel);
	view->setItemDelegate(markDelegate);
	copy->setToolTip(tr("Lossless but unprecise copying of video data"));

	auto l = new QVBoxLayout(this);

	auto layActions = new QHBoxLayout;
	layActions->addWidget(add);
	layActions->addWidget(save);
	layActions->addWidget(load);
	layActions->addWidget(copy);
	layActions->addWidget(outFfmpeg, 1);

	l->addWidget(view);
	l->addLayout(layActions);

	QMetaObject::connectSlotsByName(this);
	connect(view->selectionModel(), &QItemSelectionModel::currentRowChanged, this, &MarkingsWidget::onSelectionChanged);
}

void MarkingsWidget::setFile(QString file)
{
	markersModel->setFile(file);
}

//FIXME
Mark MarkingsWidget::getSelectedMark() const
{
	return Mark();
}

void MarkingsWidget::setCurrentPosition(qint64 pos)
{
	markDelegate->setCurrentPosition(pos);
}

void MarkingsWidget::on_add_clicked()
{
	markersModel->insertRow(markersModel->rowCount());
}

void MarkingsWidget::on_save_clicked()
{
	markersModel->save();
}

void MarkingsWidget::on_load_clicked()
{
	markersModel->load();
}

void MarkingsWidget::on_copy_stateChanged(int state)
{
	markersModel->setCopy(state == Qt::Checked);
}

void MarkingsWidget::onSelectionChanged(const QModelIndex &current, const QModelIndex &previous)
{
	auto copyC = copy->checkState() == Qt::Checked;
	outFfmpeg->setText(markersModel->getFfmpegCmd(current.row(), copyC));
}
