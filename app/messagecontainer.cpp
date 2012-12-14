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

MessageContainerWrapper::MessageContainerWrapper(IMessageContainer* wrapee) :
    wrapee(wrapee) { }
    
MessageContainerWrapper::~MessageContainerWrapper() {
    if ( this->wrapee ) {
        delete this->wrapee;
    }
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
    data[length] = 0x00;
    ms>>terminator;
    
    if (version == this->Version && terminator == this->Terminator ) {
        // data was allocated and must deleted later
        this->bytesBuffer = QByteArray(data, length);
        delete[] data;
        
        // here we fix a bug of windows version 2 Terminators are present
        if(     this->bytesBuffer.length() > 0 
            &&  (byte)this->bytesBuffer.at(this->bytesBuffer.length()-1) == 0xff
        ){
            this->bytesBuffer = this->bytesBuffer.left(this->bytesBuffer.length()-1);
        }
        if(     this->bytesBuffer.length() > 0 
            &&  (byte)this->bytesBuffer.at(this->bytesBuffer.length()-1) == 0xff
        ) {
            this->bytesBuffer = this->bytesBuffer.left(this->bytesBuffer.length()-1);
        }
        // end of special bug fix
        this->textBlock = QString::fromUtf8(this->bytesBuffer);
        return true;
    }
    return false;
}

MessageContainerEncypted::MessageContainerEncypted(IMessageContainer* wrapee, const QByteArray& key, const QString& algorithm) : 
    MessageContainerWrapper(wrapee),
    version(0x03),
    key(key),
    algorithm(algorithm),
    init(0),
    cipher(0)
{
    this->init = new QCA::Initializer();
    if (algorithm.isEmpty()) {
        this->version = 0x03;
        this->algorithm = QString("aes128");
        this->algorithmType = this->algorithm + QString("-cbc-pkcs7");
    }
}

MessageContainerEncypted::~MessageContainerEncypted() {
    this->cipher    && delete this->cipher;
    this->init      && delete this->init;
}

bool MessageContainerEncypted::isEncryptionSupported() {

    if( !this->init ) {
        this->init = new QCA::Initializer();
    }

    if(!QCA::isSupported(this->algorithmType.toAscii().constData())) {
        qDebug("MessageContainerEncyptor::isEncryptionSupported: encryption algorithm %s with %s not supported!", 
            this->algorithm.data_ptr(), 
            this->algorithmType.data_ptr()
        );
        return false;
    }
    return true;
}

QCA::Cipher* MessageContainerEncypted::createCipher(QCA::Direction direction) {

    if(!this->isEncryptionSupported()) {
        return 0;
    }
    
    if( this->cipher ) {
        return this->cipher;
    }
    
    QCA::SymmetricKey key(this->key);
    QCA::InitializationVector iv(this->key);
    this->cipher = new QCA::Cipher(
        this->algorithm,
        QCA::Cipher::CBC,
        QCA::Cipher::PKCS7,
        direction,
        key, iv
    );
    
    return this->cipher;    
}


QByteArray MessageContainerEncypted::bytes() {
    QByteArray buf = MessageContainerWrapper::bytes();

    if(!this->isEncryptionSupported()) {
        return buf;
    }
    //buf.prepend(this->version);
    
    QCA::Cipher* cipher = this->createCipher(QCA::Encode);
    //qDebug("MessageContainerEncyptor::bytes: message raw '%s'", qPrintable(QCA::arrayToHex(buf)));
    QCA::SecureArray secretText = cipher->process(buf);
    //qDebug("MessageContainerEncyptor::bytes: message encryped '%s'", qPrintable(QCA::arrayToHex(secretText.toByteArray())));
    return secretText.toByteArray();    
}

bool MessageContainerEncypted::isValidFormat(const QByteArray& bytes) {

    if(!this->isEncryptionSupported()) {
        return MessageContainerWrapper::isValidFormat(bytes);
    }

    QByteArray myBytes = bytes;
    //if (bytes.length() && (byte)bytes[0] == this->version) {
    //    int len = bytes.length() - 1;
    //    myBytes = bytes.right(len);
    //
    //}

    QCA::Cipher* cipher = this->createCipher(QCA::Decode);
    //qDebug("SteganoCore::decryptData: message raw '%s'", qPrintable(QCA::arrayToHex(buf)));
    QCA::SecureArray clearText = cipher->process(myBytes);
    //qDebug("SteganoCore::decryptData: message decryped '%s'", qPrintable(QCA::arrayToHex(clearText.toByteArray())));
    return MessageContainerWrapper::isValidFormat(clearText.toByteArray());
}