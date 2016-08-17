#ifndef MARKWIDGET_H
#define MARKWIDGET_H

#include <QWidget>

class QPushButton;
class QLineEdit;
class QIntValidator;

class MarkWidget : public QWidget
{
	Q_OBJECT
public:
	explicit MarkWidget(QWidget *parent = 0);

signals:

public slots:

private slots:
	void on_setFromButton_clicked();
	void on_setToButton_clicked();

private:
	QLineEdit* from;
	QIntValidator* fromValidator;
	QLineEdit* to;
	QIntValidator* toValidator;
	QPushButton* setFromButton;
	QPushButton* setToButton;
	QPushButton* removeButton;
};

#endif // MARKWIDGET_H
