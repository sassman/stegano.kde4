#include "MessageContainerFactory.h"
#include <qpluginloader.h>

QVector<QObject*> MessageContainerFactory::instanceMap = QVector<QObject*>();

ISteganoContainer* MessageContainerFactory::createMessageContainerFromRaw(const QByteArray& bytes)
{
    QByteArray key;
    return MessageContainerFactory::createMessageContainerFromRaw(
        bytes,
        key,
        false
    );
}

ISteganoContainer* MessageContainerFactory::createMessageContainerFromRaw(const QByteArray& bytes, const QByteArray& key)
{
    return MessageContainerFactory::createMessageContainerFromRaw(
        bytes,
        key,
        true
    );
}

ISteganoContainer* MessageContainerFactory::createMessageContainerFromRaw(
    const QByteArray& message, 
    const QByteArray& key, 
    bool useCrypt
) 
{
    bool found = false;
    IMessageContainer*  icmes      = NULL;
    IDocumentContainer* icdoc      = NULL;
    QObject*            container  = NULL;
    QString algorithm;
    
    // check format 1
    container = new MessageContainerV1();
    icmes = qobject_cast< IMessageContainer* >(container);
    if (useCrypt) {
        container = new MessageContainerEncypted(icmes, key, algorithm);
        icmes = qobject_cast< IMessageContainer* >(container);
    }
    if(icmes->isValidFormat(message)) {
        found = true;
    }else {
        delete container;
        container = NULL;
    }
    
    if(!found) {
        // check format 2
        container = new MessageContainerV2();
        icdoc = qobject_cast< IDocumentContainer* >(container);
        icmes = qobject_cast< IMessageContainer* >(container);
        if (useCrypt) {
            container = new MessageContainerEncypted(icmes, key, algorithm);
            icmes = qobject_cast< IMessageContainer* >(container);
            icdoc = qobject_cast< IDocumentContainer* >(container);
        }
        if(icmes->isValidFormat(message)) {
            found = true;
        }else {
            delete container;
            container = NULL;
        }
    }
    
    if(!found) {
        // check format 0
        container = new MessageContainerV0();
        icmes = qobject_cast< IMessageContainer* >(container);
        if (useCrypt) {
            container = new MessageContainerEncypted(icmes, key, algorithm);
            icmes = qobject_cast< IMessageContainer* >(container);
        }
        if(icmes->isValidFormat(message)) {
            found = true;
        }else {
            delete container;
            container = NULL;
        }
    }
    
    MessageContainerFactory::instanceMap.append(container);

    return icmes;
}