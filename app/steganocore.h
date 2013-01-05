#ifndef STEGANO_H
#define STEGANO_H

#include <QProgressDialog>
#include <QByteArray>
#include <QtDebug>
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
    QByteArray  key;
    QString     keyString;
    bool        useCrypt;
    QString     sourceMediaFile;
    const char* hashAlgorithm;

    void        newPassword(const QString&      passw);
};

}

#endif
