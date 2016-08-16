#ifndef MARKINGSWIDGET_H
#define MARKINGSWIDGET_H

#include <QWidget>

class QPushButton;

class MarkingsWidget : public QWidget
{
	Q_OBJECT
public:
	explicit MarkingsWidget(QWidget *parent = 0);

signals:

public slots:

private:
	QPushButton* add;
	QPushButton* save;
	QPushButton* load;
};

#endif // MARKINGSWIDGET_H
