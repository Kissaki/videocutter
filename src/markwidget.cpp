#include "markwidget.h"

#include <QHBoxLayout>
#include <QPushButton>
#include <QLineEdit>
#include <QIntValidator>

MarkWidget::MarkWidget(QWidget *parent)
	: QWidget(parent)
	, from(new QLineEdit(this))
	, to(new QLineEdit(this))
	, setFromButton(new QPushButton(tr("Set Start"), this))
	, setToButton(new QPushButton(tr("Set End"), this))
//	, removeButton(new QPushButton(tr("Remove"), this))
//	, fromValidator(new QIntValidator(from))
//	, toValidator(new QIntValidator(to))
{
	setFromButton->setObjectName("setFromButton");
	setToButton->setObjectName("setToButton");

	fromValidator = (new QIntValidator());
	toValidator = (new QIntValidator());
	from->setValidator(fromValidator);
//	to->setValidator(toValidator);

	auto l = new QHBoxLayout(this);

	l->addWidget(from);
	l->addWidget(to);
	l->addWidget(setFromButton);
	l->addWidget(setToButton);
//	l->addWidget(remove);
	l->addStretch();

	setLayout(l);

	QMetaObject::connectSlotsByName(this);
}

void MarkWidget::on_setFromButton_clicked()
{
}

void MarkWidget::on_setToButton_clicked()
{
}
