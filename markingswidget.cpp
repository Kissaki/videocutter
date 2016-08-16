#include "markingswidget.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>

MarkingsWidget::MarkingsWidget(QWidget *parent)
	: QWidget(parent)
	, add(new QPushButton(tr("+"), this))
	, save(new QPushButton(tr("Save"), this))
	, load(new QPushButton(tr("Load"), this))
{
	add->setObjectName("add");
	save->setObjectName("save");
	load->setObjectName("load");

	auto l = new QVBoxLayout(this);

	auto layActions = new QHBoxLayout;
	layActions->addWidget(add);
	layActions->addWidget(save);
	layActions->addWidget(load);
	layActions->addStretch();

	l->addLayout(layActions);
}
