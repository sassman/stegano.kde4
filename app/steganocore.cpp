#include "steganocore.h"
#include "messagecontainer.h"
#include "MessageContainerFactory.h"

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
    hashAlgorithm("md5"),
    currentContainer(NULL)
{ }

SteganoCore::~SteganoCore() { 
    if ( this->media ) {
        delete this->media;
    }
    if(this->currentContainer) delete this->currentContainer;
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
    
    if (!this->currentContainer) {
        this->currentContainer = new MessageContainerV1();
    }
    
    IMessageContainer* messageContainer = dynamic_cast<IMessageContainer*>(this->currentContainer);
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
    ISteganoContainer*  container  = NULL;
    IMessageContainer*  icmes      = NULL;
    IDocumentContainer* icdoc      = NULL;
    
    if(this->useCrypt) {
        container = MessageContainerFactory::createMessageContainerFromRaw(message, this->key);
    }else {
        container = MessageContainerFactory::createMessageContainerFromRaw(message);
    }
    icmes = dynamic_cast< IMessageContainer* >(container);
    icdoc = dynamic_cast< IDocumentContainer* >(container);
    if(icmes) {
        result = icmes->text();
    }
    if(icdoc) {
        QString all = icdoc->files().join(",");
        qDebug() << all;
    }
    
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

bool SteganoCore::addFile(QFile& file) {
    if(!this->currentContainer) {
        IDocumentContainer* i = new MessageContainerV2();
        this->currentContainer = i;
    }
    return (dynamic_cast<IDocumentContainer*>(this->currentContainer))->addFile(file);
}

int SteganoCore::count()
{
    if(this->currentContainer) {
        return (dynamic_cast<IDocumentContainer*>(this->currentContainer))->count();
    }
    return 0;
}

bool SteganoCore::extractFile(const QString& filename, QFile& target)
{
    return false;
}

QStringList SteganoCore::files()
{
    if(this->currentContainer) {
        return (dynamic_cast<IDocumentContainer*>(this->currentContainer))->files();
    }
    return QStringList();
}

bool SteganoCore::removeFile(const QString& filename)
{
    return false;
}

}
