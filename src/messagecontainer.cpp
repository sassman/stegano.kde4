#include "messagecontainer.h"
#include <qdatastream.h>

MessageContainer::MessageContainer() : Version(0x02), Terminator(0xff) {

}

MessageContainer::~MessageContainer() {

}

void MessageContainer::setText(const QString& text) {
	this->textBlock = text;
}

QByteArray MessageContainer::getBytes() {
	QByteArray bytes;
	QDataStream ms(bytes);
	ms	<< (char) this->Version
		<< this->textBlock.toUtf8()
		<< (char) this->Terminator;
	return bytes;
}

