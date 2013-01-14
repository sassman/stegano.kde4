#ifndef STEGANO_H
#define STEGANO_H

#include <QProgressDialog>
#include <QByteArray>
#include <QtDebug>
#include <QString>
#include <QStringList>
#include <QFile>
#include <KUrl>

#include "bititerator.h"
#include "messagecontainer.h"

namespace Stegano {

class SteganoCore : public QObject, public IDocumentContainer{
    Q_OBJECT
    Q_INTERFACES(IDocumentContainer ISteganoContainer)
public:
    SteganoCore();
    virtual ~SteganoCore();

    QImage*     sourceMedia() { return media; }

    QString     unhideData(QProgressDialog* monitor);
    bool        isSourceMediaValid();
    long        getMaximumMessageSize();
    
    virtual bool        isValidFormat(const QByteArray& bytes) { return true; };
    virtual bool        addFile(QFile& file);
    virtual bool        removeFile(const QString& filename);
    virtual bool        extractFile(const QString& filename, QFile& target);
    virtual QStringList files();
    virtual int         count();

signals:
    void keyChanged(QString);
    void useCryptChanged(bool);
    void sourceMediaChanged(KUrl);
    void hideProgressChanged(int);

public slots:
    void setPassword(const QString&);
    void setUseCrypt(bool);
    void setSourceMedia(QString);

    void hideData(QString message, QProgressDialog* monitor);

private:
    QImage*     media;
    QByteArray  key;
    QString     keyString;
    bool        useCrypt;
    QString     sourceMediaFile;
    const char* hashAlgorithm;
    
    ISteganoContainer*  currentContainer;

    void        newPassword(const QString&      passw);
};

}

#endif
