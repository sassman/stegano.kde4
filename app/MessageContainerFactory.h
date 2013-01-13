#ifndef MESSAGECONTAINERFACTORY_H
#define MESSAGECONTAINERFACTORY_H

#include <QObject>
#include <QByteArray>
#include <QString>

#include "messagecontainer.h"

class MessageContainerFactory : public QObject{
    Q_OBJECT
public:
    static ISteganoContainer*   createMessageContainerFromRaw(const QByteArray& bytes, const QByteArray& key);
    static ISteganoContainer*   createMessageContainerFromRaw(const QByteArray& bytes);
    
    //static IDocumentContainer*  createDocuemntContainerFromRaw(const QByteArray& bytes);
    //static IDocumentContainer*  createDocuemntContainerFromRaw(const QByteArray& bytes, const QString& key);
    
protected:
    static QVector<QObject*> instanceMap;
    static ISteganoContainer*   createMessageContainerFromRaw(const QByteArray& bytes, const QByteArray& key, bool useCrypt);
    //static IDocumentContainer*  createDocuemntContainerFromRaw(const QByteArray& bytes, const QString& key, bool useCrypt);
};

#endif  // MESSAGECONTAINERFACTORY_H