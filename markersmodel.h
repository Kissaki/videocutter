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
	void exportConcat();
	void setCopy(bool copy);
	QString getFfmpegCmd(int row, bool copy);
	Mark getMark(int row) { return m.at(row); }

	int rowCount(const QModelIndex &parent=QModelIndex()) const override;
	int columnCount(const QModelIndex &parent=QModelIndex()) const override;
	QVariant data(const QModelIndex &index, int role=Qt::DisplayRole) const override;
	QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
	bool setData(const QModelIndex &index, const QVariant &value, int role) override;
	Qt::ItemFlags flags(const QModelIndex &index) const override;
	bool insertRows(int row, int count, const QModelIndex &parent) override;
	bool removeRows(int row, int count, const QModelIndex &parent) override;

private:
	QString currentFile;
	bool copyData;
	Markings m;
	ExportProcessor* exportProcessor;

	QSharedPointer<QFile> getNewCurrentStorageFile();
};

#endif // MARKERSMODEL_H
