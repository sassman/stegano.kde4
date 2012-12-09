#include "messagecontainer.h"
#include <QDataStream>

MessageContainerBase::MessageContainerBase() :
    Version(0),
    Terminator(0) 
{ }

MessageContainerBase::~MessageContainerBase() { }

void MessageContainerBase::setText(const QString& text) {
    this->textBlock = text;
}

QByteArray MessageContainerBase::bytes() {
    QByteArray bytes;
    QDataStream ms(&bytes, QIODevice::WriteOnly);
    this->bytesBuffer = this->textBlock.toUtf8();
    ms  << (byte) this->Version;
    ms.writeRawData(this->bytesBuffer.constData(), this->bytesBuffer.size());
    ms << (byte) this->Terminator;
    ms.device()->close();
    return bytes;
}

uint MessageContainerBase::length() {
    return this->bytes().length();
}

void MessageContainerBase::setBytes(const QByteArray& bytes) {
    
    if(!this->isValidFormat(bytes)) {
        return;
    }
    
    emit bytesChanged(this->bytesBuffer);
    emit textChanged(this->textBlock);
}

MessageContainerV0::MessageContainerV0()    { }
MessageContainerV0::~MessageContainerV0()   { }

bool MessageContainerV0::isValidFormat(const QByteArray& bytes) {
    this->bytesBuffer = bytes;
    this->textBlock = QString::fromUtf8(this->bytesBuffer);
    return true;
}

MessageContainerV1::MessageContainerV1() : 
    MessageContainerBase(0x01, 0xff) 
{ }

MessageContainerV1::~MessageContainerV1() { }

bool MessageContainerV1::isValidFormat(const QByteArray& bytes) {

    if(bytes.length() <= 1) {
        return false;
    }
    this->textBlock = QString();
    QDataStream ms(bytes);
    byte terminator = 0, version = 0;
    uint read = 0, length = bytes.length() - 1 /* versoin byte */ - 1 /* terminator byte */;
    char* data;
    
    ms>>version;
    // ms>>length;
    if(length < 1) {
        return false;
    }
    data = new char[length+1];
    read = ms.readRawData(data, length);
    //ms.readBytes(data, length);
    if(!data || read < 1) {
        if ( data ) {
            delete[] data;
        }
        return false;
    }
    data[length] = '\0';
    ms>>terminator;
    
    if (version == this->Version /*&& terminator == this->Terminator*/ ) {
        // data was allocated and must deleted later
        this->bytesBuffer = QByteArray(data, length);
        delete[] data;
        this->textBlock = QString::fromUtf8(this->bytesBuffer);
        return true;
    }
    return false;
}
