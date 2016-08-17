#ifndef MARKERSMODEL_H
#define MARKERSMODEL_H

#include <QAbstractTableModel>
#include <QFile>
#include "markings.h"

class ExportProcessor;

class MarkersModel : public QAbstractTableModel
{
	Q_OBJECT
public:
	MarkersModel(ExportProcessor* expProc, QObject* parent=nullptr);

	void setFile(QString file);
	void save();
	void load();
	void exportMark(int rowIndex);
	void setCopy(bool copy);
	QString getFfmpegCmd(int row, bool copy);

	int rowCount(const QModelIndex &parent=QModelIndex()) const;
	int columnCount(const QModelIndex &parent=QModelIndex()) const;
	QVariant data(const QModelIndex &index, int role=Qt::DisplayRole) const;
	QVariant headerData(int section, Qt::Orientation orientation, int role) const;
	bool setData(const QModelIndex &index, const QVariant &value, int role);
	Qt::ItemFlags flags(const QModelIndex &index) const;
	bool insertRows(int row, int count, const QModelIndex &parent);
	bool removeRows(int row, int count, const QModelIndex &parent);

private:
	QString currentFile;
	bool copyData;
	Markings m;
	ExportProcessor* exportProcessor;

	QSharedPointer<QFile> getNewCurrentStorageFile();
};

#endif // MARKERSMODEL_H
