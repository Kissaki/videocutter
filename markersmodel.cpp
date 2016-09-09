#include "markersmodel.h"

#include <QSharedPointer>
#include <QDebug>
#include <QJsonDocument>
#include <QJsonArray>
#include "markcolumns.h"
#include "exportprocessor.h"

MarkersModel::MarkersModel(ExportProcessor* expProc, QObject* parent)
 : QAbstractTableModel(parent)
 , copyData(false)
 , exportProcessor(expProc)
{
}

void MarkersModel::setFile(QString file)
{
	if (!currentFile.isNull())
	{
		save();
	}
	currentFile = file;
	QFile f(file);
	if (f.exists())
	{
		load();
	}
}

QSharedPointer<QFile> MarkersModel::getNewCurrentStorageFile()
{
	return QSharedPointer<QFile>::create(currentFile + ".markings.json");
}

void MarkersModel::save()
{
	auto f = getNewCurrentStorageFile();
	if (f->exists())
	{
		qDebug() << "Overwriting file" << f->fileName();
	}
	f->open(QIODevice::WriteOnly);
	QJsonArray o;
	m.write(o);
	QJsonDocument json(o);
	f->write(json.toJson());
}

void MarkersModel::load()
{
	auto f = getNewCurrentStorageFile();
	f->open(QIODevice::ReadOnly);
	auto data = f->readAll();
	f->close();

	auto json = QJsonDocument::fromJson(data);
	beginResetModel();
	m.read(json.array());
	endResetModel();
}

void MarkersModel::exportMark(int rowIndex)
{
	exportProcessor->exportMark(currentFile, m.at(rowIndex), copyData);
}

void MarkersModel::exportConcat()
{
	auto c = rowCount();
	std::vector<Mark> marks;
	marks.reserve(c);
	for (auto i = 0; i < c; ++i)
	{
		auto mark = m.at(i);
		marks.push_back(mark);
	}
	qDebug() << QString::number(marks.size());
	exportProcessor->exportConcat(currentFile, marks);
}

void MarkersModel::setCopy(bool copy)
{
	copyData = copy;
}

int MarkersModel::rowCount(const QModelIndex &/*parent*/) const
{
	return m.count();
}

int MarkersModel::columnCount(const QModelIndex &/*parent*/) const
{
	return COLS::COLCOUNT;
}

QVariant MarkersModel::data(const QModelIndex &index, int role) const
{
	if (role != Qt::DisplayRole && role != Qt::EditRole)
	{
		return QVariant();
	}

	switch(index.column())
	{
	case 0:
		return m[index.row()].start;
	case 1:
		return m[index.row()].end;
	}

	return QVariant();
}

QVariant MarkersModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	if (role != Qt::DisplayRole)
	{
		return QVariant();
	}
	if (orientation == Qt::Vertical)
	{
		return QVariant();
	}

	switch (section)
	{
	case 0:
		return tr("Begin [ms]");
	case 1:
		return tr("End [ms]");
	}

	return QVariant();
}

bool MarkersModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
	if (!index.isValid() || role != Qt::EditRole)
	{
		return false;
	}

	bool ok;
	int v = value.toInt(&ok);
	if (!ok)
	{
		return false;
	}

	int i = index.row();
	if (i < 0 || i >= m.count())
	{
		return false;
	}

	auto& mark = m[i];
	switch(index.column())
	{
	case 0:
	{
		mark.start = v;
		bool adjustEnd = mark.end < mark.start;
		if (adjustEnd)
		{
			mark.end = mark.start;
		}

		emit dataChanged(index, adjustEnd ? index.sibling(index.row(), index.column() + 1) : index);
		return true;
	}
	case 1:
	{
		mark.end = v;
		bool adjustStart = mark.end < mark.start;
		if (adjustStart)
		{
			mark.start = mark.end;
		}

		emit dataChanged(index, adjustStart ? index.sibling(index.row(), index.column() - 1) : index);
		return true;
	}
	}

	return false;
}

Qt::ItemFlags MarkersModel::flags(const QModelIndex &index) const
{
	switch (index.column())
	{
	case COLS::START:
	case COLS::END:
		return QAbstractItemModel::flags(index) | Qt::ItemIsEditable;
	case COLS::SET_START:
	case COLS::SET_END:
	case COLS::REMOVE_MARK:
	case COLS::EXPORT:
	case COLS::PLAY:
		return QAbstractItemModel::flags(index) | Qt::ItemIsEnabled;
	}
	return Qt::NoItemFlags;
}

bool MarkersModel::insertRows(int row, int count, const QModelIndex &parent)
{
	if (count != 1)
	{
		return false;
	}

	beginInsertRows(parent, row, row);
	m.insert(row, Mark());
	endInsertRows();
	return true;
}

bool MarkersModel::removeRows(int row, int count, const QModelIndex &parent)
{
	if (count != 1)
	{
		return false;
	}

	beginRemoveRows(parent, row, row);
	m.removeAt(row);
	endRemoveRows();
//	emit dataChanged(index(0, 0), index(rowCount(), columnCount()));
	return true;
}

QString MarkersModel::getFfmpegCmd(int row, bool copy)
{
	return ExportProcessor::getFfmpegExtractArgs(m.at(row), currentFile, copy);
}
