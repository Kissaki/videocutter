#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMediaPlayer>
#include <QMediaPlaylist>
#include <QVideoWidget>
#include <QPushButton>

MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::MainWindow)
{
//	ui->setupUi(this);

	auto player = new QMediaPlayer;
	videoWidget = new QVideoWidget;
	auto b = new QPushButton("test");

	player->setVideoOutput(videoWidget);

	//layout()->addWidget(videoWidget);
//	ui->horizontalLayout->addWidget(videoWidget);
//	auto l = dynamic_cast<QHBoxLayout*>(layout());
//	l->insertWidget(1, videoWidget);

//	videoWidget->show();

	auto playlist = new QMediaPlaylist(player);
	playlist->addMedia(QUrl::fromLocalFile("E:/work/video/capture/plays/Overwatch/2016_07_15_21_54_03-ses.mp4"));
	playlist->setCurrentIndex(0);

	QHBoxLayout* l = new QHBoxLayout;
//	l->addWidget(videoWidget);
	l->addWidget(b);
	setLayout(l);

//	player->play();
}

MainWindow::~MainWindow()
{
	delete ui;
}

void MainWindow::fileList_dragEnterEvent() {
}

void MainWindow::fileList_clicked(QModelIndex) {
}
