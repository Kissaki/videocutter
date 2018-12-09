#include "filenamedialog.h"
#include "ui_filenamedialog.h"

#include <QFile>
#include <QFileInfo>

FilenameDialog::FilenameDialog(QString filename, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FilenameDialog)
{
	ui->setupUi(this);
	QFileInfo fi(filename);
	auto fn = fi.fileName();
	ui->leFilename->setText(fn);
	// Place cursor before file extension dot separator.
	ui->leFilename->setCursorPosition(fn.lastIndexOf('.'));
}

FilenameDialog::~FilenameDialog()
{
	delete ui;
}

void FilenameDialog::accept()
{
	filename = ui->leFilename->text();
	if (filename.isEmpty())
	{
		QDialog::reject();
		return;
	}

	QDialog::accept();
}

QString FilenameDialog::getNewFilename()
{
	return filename;
}
