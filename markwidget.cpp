#include "markwidget.h"

#include <QPushButton>
#include <QLineEdit>

MarkWidget::MarkWidget(QWidget *parent)
	: QWidget(parent)
	, from(new QLineEdit(this))
	, to(new QLineEdit(this))
	, setFromButton(new QPushButton(this))
	, setToButton(new QPushButton(this))
{
}
