#ifndef MARKWIDGET_H
#define MARKWIDGET_H

#include <QWidget>

class QPushButton;
class QLineEdit;

class MarkWidget : public QWidget
{
	Q_OBJECT
public:
	explicit MarkWidget(QWidget *parent = 0);

signals:

public slots:

private:
	QLineEdit* from;
	QLineEdit* to;
	QPushButton* setFromButton;
	QPushButton* setToButton;
};

#endif // MARKWIDGET_H
