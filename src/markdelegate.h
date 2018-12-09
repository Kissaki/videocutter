#ifndef MARKDELEGATE_H
#define MARKDELEGATE_H

#include <QStyledItemDelegate>
class ExportProcessor;

class MarkDelegate : public QStyledItemDelegate
{
	Q_OBJECT
public:
	MarkDelegate(QObject *parent = Q_NULLPTR);

	 void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
	 bool editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index);
	 void setCurrentPosition(qint64 pos) { currentPosition = pos; }

signals:
	 void playClicked(int row);

private:
	 qint64 currentPosition;
};

#endif // MARKDELEGATE_H
