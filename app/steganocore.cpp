#include "steganocore.h"
#include "messagecontainer.h"

#include <stdio.h>
#include <stdlib.h>

#include <QtGui/QBitmap>
#include <QtGui/QRgb>
#include <QtCore/QBitArray>

namespace Stegano {
    
SteganoCore::SteganoCore() : 
    media(0),
    keyString(""), 
    useCrypt(false), 
    sourceMediaFile(""),
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
        QCA::Hash hashAlg(this->hashAlgorithm);
        QCA::MemoryRegion hashRaw = hashAlg.hash(passw.toUtf8());
        this->key = hashRaw.toByteArray();
        this->setUseCrypt(true);
        this->keyString = hashAlg.hashToString(hashRaw);
        emit keyChanged( this->keyString );
    }
    else {
        setUseCrypt(false);
    }
}

void SteganoCore::setUseCrypt(bool use) {
    this->useCrypt = use;
    qDebug("SteganoCore::setUseCrypt: keystring '%s'", qPrintable(this->keyString));        
    emit useCryptChanged(this->useCrypt);
}

void SteganoCore::setSourceMedia(QString source) {
    this->sourceMediaFile = source;
    emit sourceMediaChanged(KUrl(this->sourceMediaFile));
}

void SteganoCore::hideData(QString message, QProgressDialog* monitor) {
    
    IMessageContainer* messageContainer = new MessageContainerV1();
    messageContainer->setText(message);
    
    if (this->useCrypt) {
        messageContainer = new MessageContainerEncypted(messageContainer, this->key, "");
    }
    QByteArray chiffre = messageContainer->bytes();

    if( this->media ) {
        delete this->media;
        this->media = 0L;
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
    
    if( messageContainer ) {
        delete messageContainer;
    }
}

QString SteganoCore::unhideData(QProgressDialog* monitor) {
    if( this->media ) {
        delete this->media;
        this->media = 0L;
    }
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
    QString result;
    bool found = false;

    MessageContainerV0          *cont0      = NULL;
    MessageContainerV1          *cont1      = NULL;
    MessageContainerV2          *cont2      = NULL;
    MessageContainerEncypted    *cont_enc   = NULL;
    IMessageContainer           *cont_i     = NULL;
    IDocumentContainer          *cdoc_i     = NULL;
    
    // check format 1
    cont1 = new MessageContainerV1();
    cont_i = qobject_cast< IMessageContainer* >(cont1);
    if (this->useCrypt) {
        cont_enc = new MessageContainerEncypted(cont1, this->key, QString(""));
        cont_i = qobject_cast< IMessageContainer* >(cont_enc);
    }
    if(cont_i && cont_i->isValidFormat(message)) {
        result = cont_i->text();
        found = true;
    }
    
    if(!found) {
        // check format 2
        cont2 = new MessageContainerV2();
        cont_i = qobject_cast< IMessageContainer* >(cont2);
        cdoc_i = qobject_cast< IDocumentContainer* >(cont2);
        if (this->useCrypt) {
            if(cont_enc) delete cont_enc;
            cont_enc = new MessageContainerEncypted(cont2, this->key, QString(""));
            cont_i = qobject_cast< IMessageContainer* >(cont_enc);
            cdoc_i = qobject_cast< IDocumentContainer* >(cont_enc);
        }
        if(cont_i && cont_i->isValidFormat(message)) {
            result = cont_i->text();
            
            if (cdoc_i) {
                QString all = cdoc_i->files().join(",");
                qDebug() << all;
            }
            found = true;
        }
    }
    
    if(!found) {
        // check format 0
        cont0 = new MessageContainerV0();
        cont_i = qobject_cast< IMessageContainer* >(cont0);
        if (this->useCrypt) {
            if(cont_enc) {
                delete cont_enc;
            }
            cont_enc = new MessageContainerEncypted(cont0, this->key, QString(""));
            cont_i = qobject_cast< IMessageContainer* >(cont_enc);
        }
        if(cont_i && cont_i->isValidFormat(message)) {
            result = cont_i->text();
            found = true;
        }
    }

    // cleanup
    if(cont_enc) delete cont_enc;
    if(cont0)    delete cont0;
    if(cont1)    delete cont1;
    if(cont2)    delete cont2;
    
    return result;
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

}