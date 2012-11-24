#ifndef STEGANO_H
#define STEGANO_H

#include <QtCrypto/QtCrypto>
#include <QProgressDialog>
#include <QByteArray>
#include <KUrl>

#include "bititerator.h"

class SteganoCore : public QObject {
    Q_OBJECT
public:
    SteganoCore();
    ~SteganoCore();

signals:
    void keyChanged(QString);
    void useCryptChanged(bool);
    void sourceMediaChanged(KUrl);
    void hideProgressChanged(int);

public slots:
    void setPassword(const QString&);
    void setUseCrypt(bool);
    void setSourceMedia(QString);

    void hideData(const QByteArray& source, QProgressDialog* monitor);

public:
    QImage*     sourceMedia() {
        return media;
    }

    QByteArray  unhideData(QProgressDialog* monitor);
    bool        isEncryptionSupported();

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

#endif
