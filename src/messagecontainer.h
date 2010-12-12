#ifndef MESSAGECONTAINER_H
#define MESSAGECONTAINER_H
#include <QString>

class MessageContainer {

private:
	QString textBlock;
	const char Version;
	const char Terminator;

public:
	MessageContainer();
	virtual ~MessageContainer();

	void setText(const QString& text);

	virtual QByteArray getBytes();
};

#endif // MESSAGECONTAINER_H
