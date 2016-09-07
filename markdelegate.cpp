#include "markdelegate.h"

#include <QApplication>
#include <QDebug>
#include "markcolumns.h"
#include "exportprocessor.h"
#include "markersmodel.h"

MarkDelegate::MarkDelegate(QObject *parent)
	: QStyledItemDelegate(parent)
	, currentPosition(0)
{
}
void MarkDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
	if (index.isValid() && index.column() >= COLS::SET_START && index.column() < COLS::COLCOUNT)
	{
		QString buttonText;
		switch (index.column())
		{
		case COLS::SET_START:
			buttonText = tr("Set Begin");
			break;
		case COLS::SET_END:
			buttonText = tr("Set End");
			break;
		case COLS::REMOVE_MARK:
			buttonText = tr("Remove");
			break;
		case COLS::EXPORT:
			buttonText = tr("Export");
			break;
		case COLS::PLAY:
			buttonText = tr("Play");
			break;
		default:
			qWarning() << "Unhandled MarkDelegate::paint for column" << index.column();
			break;
		}

		QStyleOptionButton button;
		button.rect = option.rect;
		button.text = buttonText;
		button.state = QStyle::State_Enabled;
		QStyle::State_Active;

		QApplication::style()->drawControl(QStyle::CE_PushButton, &button, painter);
	}
	else
	{
		QStyledItemDelegate::paint(painter, option, index);
	}
}

bool MarkDelegate::editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index)
{
	if (index.isValid() && event->type() == QEvent::MouseButtonPress)
	{
		if (index.column() == COLS::SET_START || index.column() == COLS::SET_END)
		{
			int iCol = index.column() - 2;
			auto i = model->index(index.row(), iCol);
			model->setData(i, currentPosition);
			return true;
		}
		if (index.column() == COLS::REMOVE_MARK)
		{
			model->removeRow(index.row());
			return true;
		}
		if (index.column() == COLS::EXPORT)
		{
			dynamic_cast<MarkersModel*>(model)->exportMark(index.row());
			return true;
		}
		if (index.column() == COLS::PLAY)
		{
			emit playClicked(index.row());
			return true;
		}
	}

	return QStyledItemDelegate::editorEvent(event, model, option, index);
}
