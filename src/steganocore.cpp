#include "steganocore.h"

#include <stdio.h> 
#include <stdlib.h>

#include <QtGui/QBitmap>
#include <QtGui/QRgb>
#include <QtCore/QBitArray>

SteganoCore::SteganoCore(){
}

SteganoCore::~SteganoCore(){
}

void SteganoCore::setPassword(const QString& passw){
	this->newPassword(passw);
}

void SteganoCore::newPassword(const QString& passw){
	QCA::Initializer init;
	char algoStr[] = "md5";

	if(!QCA::isSupported(algoStr)){
		qDebug("Algorithmn not supported!");
		return;
	}
	QString hash = "";
	if(passw.length() > 0){
		hash = QCA::Hash(algoStr).hashToString(passw.toUtf8());
		this->setUseCrypt(true);
	}
	else{
		setUseCrypt(false);
	}

	this->keyString = hash;
	emit keyChanged( this->keyString );
}

void SteganoCore::setUseCrypt(bool use){
	this->useCrypt = use;
	emit useCryptChanged(this->useCrypt);
}

void SteganoCore::setSourceMedia(QString source){
	this->sourceMediaFile = source;
	emit sourceMediaChanged(KUrl(this->sourceMediaFile));
}

void SteganoCore::hideData(const QByteArray& source, QProgressDialog* monitor){
	QByteArray chiffre = QByteArray(source);
	chiffre.prepend((char) 0x02);
	chiffre.append((char) 0xff);

	if (this->useCrypt) {
		chiffre = encryptData(chiffre);
	}

	this->media = new QImage(this->sourceMediaFile);
	BitIterator bi(chiffre);

	int lastProg = 0, currProg = -1, i = 0, colorChannels = 3; //media->colorCount();
	float progGes = media->width() * media->height() * colorChannels;

	monitor->setValue(lastProg);
	for (int x = 0; x < media->width(); x++) {
		for (int y = 0; y < media->height(); ++y) {
			currProg = (int)((x * y * colorChannels * 100) / progGes);
			if(currProg > lastProg) {
				monitor->setValue(lastProg = currProg );

				if (monitor->wasCanceled())
					return;
			}
			QColor pixelColor = QColor(media->pixel(x, y));
			QColor nColor = QColor::fromRgb(
				(int)((pixelColor.red() 	& 0xFE) | bi.bit(i++)),
				(int)((pixelColor.green() 	& 0xFE) | bi.bit(i++)),
				(int)((pixelColor.blue()	& 0xFE) | bi.bit(i++))
			);
			media->setPixel(x, y, nColor.rgb());
		}
	}
}

QByteArray SteganoCore::unhideData(QProgressDialog* monitor) {
	this->media = new QImage(this->sourceMediaFile);
	BitIterator bi;
	
	int lastProg = 0, currProg = -1, i = 0, colorChannels = 3; //media->colorCount();
	float progGes = media->width() * media->height() * colorChannels;
	
	monitor->setValue(lastProg);
	for (int x = 0; x < media->width(); x++) {
		for (int y = 0; y < media->height(); ++y) {
			currProg = (int)((x * y * colorChannels * 100) / progGes);
			if(currProg > lastProg) {
				monitor->setValue(lastProg = currProg );
				
				if (monitor->wasCanceled())
					return QByteArray();
			}
			QColor pixelColor = QColor(media->pixel(x, y));
			bi.setBit(i++, pixelColor.red());
			bi.setBit(i++, pixelColor.green());
			bi.setBit(i++, pixelColor.blue());
		}
	}

	QByteArray message = bi.data();
	if (this->useCrypt) {
		message = this->decryptData(message);
	}
	if( message[0] == (char)0x01
	 && message[message.length() -1] == (char)0xff
	) {
		// the version 1 encoded image
		message.resize(message.length()-1);
	}
	if(message.length() > 1)
		message = message.mid(1, message.length()-2);
	return message;
}

QByteArray SteganoCore::encryptData(const QByteArray& buf){
	QCA::Initializer init;
	QString algo("aes128");
	
	if(!QCA::isSupported("aes128-cbc-pkcs7")){
		qDebug("SteganoCore::encryptData: aes128-cbc-pkcs7 not supported!");
		return buf;
	}

	QCA::SymmetricKey key(this->keyString.toUtf8());
	QCA::InitializationVector iv(16);
	iv.fill('e');
	QCA::Cipher cipher(
		algo,
		QCA::Cipher::CBC,
		QCA::Cipher::DefaultPadding,
		QCA::Encode,
		key, iv
	);

	QCA::SecureArray secretText = cipher.process(buf);
	return secretText.toByteArray();
}

QByteArray SteganoCore::decryptData(const QByteArray& buf){
	QCA::Initializer init;
	QString algo("aes128");
	
	if(!QCA::isSupported("aes128-cbc-pkcs7")){
		qDebug("SteganoCore::decryptData: aes128-cbc-pkcs7 not supported!");
		return buf;
	}
	
	QCA::SymmetricKey key(this->keyString.toUtf8());
	QCA::InitializationVector iv(16);
	iv.fill('e');
	QCA::Cipher cipher(
		algo,
		QCA::Cipher::CBC,
		QCA::Cipher::DefaultPadding,
		QCA::Decode,
		key, iv
	);
	
	QCA::SecureArray clearText = cipher.process(buf);
	return clearText.toByteArray();
}

