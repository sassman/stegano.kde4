#ifndef STEGANO_H
#define STEGANO_H
 
#include <QtCrypto/QtCrypto>
#include <KUrl>

#include "bititerator.h"
 
class Stegano : public QObject
{
    Q_OBJECT
    public:
        Stegano();
        ~Stegano();
	
  signals:
    void keyChanged(QString);
    void useCryptChanged(bool);
    void sourceMediaChanged(KUrl);
    void hideProgressChanged(int);
	
    public slots:
      void setPassword(const QString&);
      void setUseCrypt(bool);
      void setSourceMedia(QString);
      
      void hideData(const QByteArray&);
 
    private:
      QString 	keyString;
      bool	useCrypt;
      QString	sourceMedia;

      QByteArray 	encryptData(const QByteArray& 	buf);
      void 		newPassword(const QString& 	passw);
};
 
#endif
