#include "filenamedialog.h"
#include "ui_filenamedialog.h"

#include <QFile>
#include <QUrl>

FilenameDialog::FilenameDialog(QString filename, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FilenameDialog),
    filename(filename)
{
	ui->setupUi(this);
//	QFile f(filename);
//	ui->leFilename->setText(f.fileName());
	QUrl u = QUrl::fromLocalFile(filename);
	ui->leFilename->setText(u.fileName());
	ui->leFilename->setCursorPosition(filename.lastIndexOf('.'));
	ui->leFilename->setCursorPosition(-1);
}

FilenameDialog::~FilenameDialog()
{
	delete ui;
}

void FilenameDialog::accept()
{
	auto url = QUrl::fromLocalFile(filename);
	filename = url.path() + ui->leFilename->text();

	QDialog::accept();
}

QString FilenameDialog::getNewFilename()
{
	return filename;
}
