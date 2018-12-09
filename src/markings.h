#ifndef MARKINGS_H
#define MARKINGS_H

#include <QJsonObject>
#include <QList>

struct Mark
{
	int start;
	int end;
};

class Markings
		: public QList<Mark>
{
public:
	Markings();

	void read(const QJsonArray &json);
	void write(QJsonArray &json) const;
};

#endif // MARKINGS_H
