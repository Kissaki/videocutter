#include "markingswidget.h"

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
	, outFfmpeg(new QLineEdit(this))
	, ffmpegParameters(new QComboBox(this))
{
	view->setModel(markersModel);
	view->setItemDelegate(markDelegate);

	setupLayout();

	add->setObjectName("add");
	add5s->setObjectName("add5s");
	save->setObjectName("save");
	load->setObjectName("load");
	concat->setObjectName("concat");
	markersModel->setObjectName("markersModel");
	markDelegate->setObjectName("markDelegate");
	ffmpegParameters->setObjectName("ffmpegParameters");

	QMetaObject::connectSlotsByName(this);
	connect(view->selectionModel(), &QItemSelectionModel::currentRowChanged, this, &MarkingsWidget::onSelectionChanged);
	connect(markDelegate, &MarkDelegate::playClicked, this, &MarkingsWidget::onMarkPlayClicked);

	ffmpegParameters->setEditable(true);
	ffmpegParameters->addItem("-c copy");
	ffmpegParameters->addItem("-level:v 4.2 -b:v 50M");
	ffmpegParameters->addItem("-c:v libx264 -preset slow -crf 18 -pix_fmt yuv420p");
	ffmpegParameters->addItem("-c:v libvpx -b 50M");
}

void MarkingsWidget::setupLayout()
{
	auto l = new QVBoxLayout(this);

	auto layActions = new QHBoxLayout;
	layActions->addWidget(add);
	layActions->addWidget(add5s);
	layActions->addSpacing(4);
	layActions->addWidget(save);
	layActions->addWidget(load);
	layActions->addSpacing(4);

	auto cmd = new QGroupBox(tr("Command"), this);
	auto layExport = new QHBoxLayout;
	layExport->addWidget(ffmpegParameters);
	layExport->addWidget(outFfmpeg, 1);
	layExport->addWidget(concat);
	cmd->setLayout(layExport);
	layActions->addWidget(cmd);

	l->addWidget(view);
	l->addLayout(layActions);
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

void MarkingsWidget::onSelectionChanged(const QModelIndex &current, const QModelIndex &/*previous*/)
{
	outFfmpeg->setText(markersModel->getFfmpegCmd(current.row()));
}

void MarkingsWidget::on_ffmpegParameters_currentTextChanged(const QString& text)
{
	emit ffmpegParametersChanged(text);
	markersModel->setFfmpegParameters(text);
	auto index = view->selectionModel()->currentIndex();
	outFfmpeg->setText(index.isValid() ? markersModel->getFfmpegCmd(index.row()) : QString());
}

void MarkingsWidget::onMarkPlayClicked(int row)
{
	const Mark& mark = markersModel->getMark(row);
	emit playRange(mark.start, mark.end);
}

void MarkingsWidget::on_markersModel_dataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight, const QVector<int> &roles)
{
	Q_UNUSED(roles);
	auto current = view->selectionModel()->currentIndex();
	auto valid = current.isValid()
			&& topLeft.isValid()
			&& bottomRight.isValid()
			&& topLeft.row() <= view->selectionModel()->currentIndex().row()
			&& bottomRight.row() >= current.row();
	outFfmpeg->setText(valid ? markersModel->getFfmpegCmd(current.row()) : QString());
}
