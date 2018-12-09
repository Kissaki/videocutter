#include "markings.h"

#include <QJsonObject>
#include <QJsonArray>

Markings::Markings()
	: QList<Mark>()
{
}

void Markings::read(const QJsonArray &json)
{
	clear();
	for (auto it = json.begin(); it != json.end(); ++it)
	{
		QJsonObject o = it->toObject();
		Mark m;
		m.start = o["start"].toInt();
		m.end = o["end"].toInt();
		append(m);
	}
}

void Markings::write(QJsonArray &json) const
{
	for (auto it = begin(); it != end(); ++it)
	{
		QJsonObject mark;
		mark.insert("start", it->start);
		mark.insert("end", it->end);
		json.append(mark);
	}
}
