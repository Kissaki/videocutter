#ifndef FILENAMEDIALOG_H
#define FILENAMEDIALOG_H

#include <QDialog>

namespace Ui {
class FilenameDialog;
}

class FilenameDialog : public QDialog
{
	Q_OBJECT

public:
	explicit FilenameDialog(QString filename, QWidget *parent = 0);
	~FilenameDialog();
	QString getNewFilename();

protected:
	virtual void accept() Q_DECL_OVERRIDE;

private:
	Ui::FilenameDialog *ui;
	QString filename;
};

#endif // FILENAMEDIALOG_H
