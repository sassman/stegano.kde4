#include "steganocore.h"

#include <stdio.h>
#include <stdlib.h>

#include <QtGui/QBitmap>
#include <QtGui/QRgb>
#include <QtCore/QBitArray>

SteganoCore::SteganoCore() : 
    media(0),
    keyString(""), 
    useCrypt(false), 
    sourceMediaFile(""),
    encryptionAlgorithm("aes128"),
    encryptionAlgorithmType("aes128-cbc-pkcs7"),
    hashAlgorithm("md5")
{ }

SteganoCore::~SteganoCore() { 
    if ( this->media ) {
        delete this->media;
    }
}

void SteganoCore::setPassword(const QString& passw) {
    this->newPassword(passw);
}

void SteganoCore::newPassword(const QString& passw) {
    QCA::Initializer init;

    if(!QCA::isSupported(this->hashAlgorithm)) {
        qDebug("SteganoCore::newPassword: hash algorithm %s not supported!", 
                this->hashAlgorithm
        );
        return;
    }
    QString hash = "";
    if(passw.length() > 0) {
        hash = QCA::Hash(this->hashAlgorithm).hashToString(passw.toUtf8());
        this->setUseCrypt(true);
    }
    else {
        setUseCrypt(false);
    }

    this->keyString = hash;
    emit keyChanged( this->keyString );
}

void SteganoCore::setUseCrypt(bool use) {
    this->useCrypt = use;
    emit useCryptChanged(this->useCrypt);
}

void SteganoCore::setSourceMedia(QString source) {
    this->sourceMediaFile = source;
    emit sourceMediaChanged(KUrl(this->sourceMediaFile));
}

void SteganoCore::hideData(const QByteArray& source, QProgressDialog* monitor) {
    QByteArray chiffre = QByteArray(source);
    chiffre.prepend((char) 0x02);
    chiffre.append((char) 0xff);

    if (this->useCrypt) {
        chiffre = encryptData(chiffre);
    }

    this->media = new QImage(this->sourceMediaFile);
    BitIterator bi(chiffre);

    int lastProg = 1, currProg = -1, i = 0, colorChannels = 3;
    long progGes = media->width() * media->height() * colorChannels;

    monitor->setValue(lastProg);
    for (int x = 0; x < media->width(); x++) {
        for (int y = 0; y < media->height(); y++) {
            currProg = (int)((x * y * colorChannels * 100) / progGes);
            if(currProg > lastProg) {
                lastProg = currProg;
                monitor->setValue(currProg);
                if (monitor->wasCanceled()) {
                    return; 
                }
            }
            QColor pixelColor = QColor(media->pixel(x, y));
            QColor nColor = QColor::fromRgb(
                                (int)((pixelColor.red()     & 0xFE) | bi.bit(i)),
                                (int)((pixelColor.green()   & 0xFE) | bi.bit(i+1)),
                                (int)((pixelColor.blue()    & 0xFE) | bi.bit(i+2))
                            );
            i += 3;
            media->setPixel(x, y, nColor.rgb());
        }
    }
}

QByteArray SteganoCore::unhideData(QProgressDialog* monitor) {
    this->media = new QImage(this->sourceMediaFile);
    BitIterator bi;

    int lastProg = 1, currProg = -1, i = 0, colorChannels = 3;
    long progGes = media->width() * media->height() * colorChannels;

    monitor->setValue(lastProg);
    for (int x = 0; x < media->width(); x++) {
        for (int y = 0; y < media->height(); y++) {
            currProg = (int)((x * y * colorChannels * 100) / progGes);
            if(currProg > lastProg) {
                lastProg = currProg;
                monitor->setValue( currProg );
                if (monitor->wasCanceled())
                    return QByteArray();
            }
            QColor pixelColor = QColor(media->pixel(x, y));
            bi.setBit(i,    pixelColor.red());
            bi.setBit(i+1,  pixelColor.green());
            bi.setBit(i+2,  pixelColor.blue());
            i += 3;
        }
    }

    QByteArray message = bi.data();
    if (this->useCrypt) {
        message = this->decryptData(message);
    }

    if(     message[0] == (char)0x01
        &&  message[message.length() -1] == (char)0xff
    ) { // legacy message container
        message.resize(message.length()-1);
    }else { // current message container
        message = message.mid(1, message.length()-2);
    }
    return message;
}

QByteArray SteganoCore::encryptData(const QByteArray& buf) {
    QCA::Initializer init;

    if(!this->isEncryptionSupported()) {
        return buf;
    }

    QCA::SymmetricKey key(this->keyString.toUtf8());
    QCA::InitializationVector iv(this->keyString.toUtf8());
    QCA::Cipher cipher(
        this->encryptionAlgorithm,
        QCA::Cipher::CBC,
        QCA::Cipher::PKCS7,
        QCA::Encode,
        key, iv
    );

    qDebug("SteganoCore::encryptData: message raw '%s'", qPrintable(QCA::arrayToHex(buf)));
    QCA::SecureArray secretText = cipher.process(buf);
    qDebug("SteganoCore::encryptData: message encryped '%s'", qPrintable(QCA::arrayToHex(secretText.toByteArray())));
    return secretText.toByteArray();
}

QByteArray SteganoCore::decryptData(const QByteArray& buf) {
    QCA::Initializer init;

    if(!this->isEncryptionSupported()) {
        return buf;
    }

    QCA::SymmetricKey key(this->keyString.toUtf8());
    QCA::InitializationVector iv(this->keyString.toUtf8());
    QCA::Cipher cipher(
        this->encryptionAlgorithm,
        QCA::Cipher::CBC,
        QCA::Cipher::PKCS7,
        QCA::Decode,
        key, iv
    );

    qDebug("SteganoCore::decryptData: message raw '%s'", qPrintable(QCA::arrayToHex(buf)));
    QCA::SecureArray clearText = cipher.process(buf);
    qDebug("SteganoCore::decryptData: message decryped '%s'", qPrintable(QCA::arrayToHex(clearText.toByteArray())));
    return clearText.toByteArray();
}

bool SteganoCore::isEncryptionSupported() {
    QCA::Initializer init;

    if(!QCA::isSupported(this->encryptionAlgorithmType)) {
        qDebug("SteganoCore::isEncryptionSupported: encryption algorithm %s with %s not supported!", 
               this->encryptionAlgorithm, 
               this->encryptionAlgorithmType
        );
        return false;
    }
    return true;
}

bool SteganoCore::isSourceMediaValid() {
    if(this->sourceMediaFile.isEmpty()) {
        return false;
    }
    QImage img = QImage(this->sourceMediaFile);
    if(img.isNull()) {
        return false;
    }
    return true;
}

/**
 * if a media is set here the maximium size that is available for data is returned
 * 
 * @return max available size in bytes
 */
long int SteganoCore::getMaximumMessageSize() {
    if(this->sourceMediaFile.isEmpty()) {
        return 0;
    }
    
    QImage media = QImage(this->sourceMediaFile);
    short colorChannels = 3;
    return (media.width() * media.height() * colorChannels) / 8;
    
}

