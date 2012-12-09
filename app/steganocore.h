#ifndef STEGANO_H
#define STEGANO_H

#include <QtCrypto/QtCrypto>
#include <QProgressDialog>
#include <QByteArray>
#include <QString>
#include <KUrl>

#include "bititerator.h"

namespace Stegano {

class SteganoCore : public QObject {
    Q_OBJECT
public:
    SteganoCore();
    virtual ~SteganoCore();

    QImage*     sourceMedia() { return media; }

    QString     unhideData(QProgressDialog* monitor);
    bool        isEncryptionSupported();
    bool        isSourceMediaValid();
    long        getMaximumMessageSize();

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
    QString     keyString;
    bool        useCrypt;
    QString     sourceMediaFile;
    const char* encryptionAlgorithm;
    const char* encryptionAlgorithmType;
    const char* hashAlgorithm;

    QByteArray  encryptData(const QByteArray&   buf);
    QByteArray  decryptData(const QByteArray&   buf);
    void        newPassword(const QString&      passw);
};

}

#endif
