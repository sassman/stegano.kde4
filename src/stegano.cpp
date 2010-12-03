#include "stegano.h"
#include <stdio.h> 
#include <stdlib.h>

#include <QtGui/QBitmap>
#include <QtCore/QBitArray>

Stegano::Stegano(){
}

Stegano::~Stegano(){
}

void Stegano::setPassword(const QString& passw){
  this->newPassword(passw);
}

void Stegano::newPassword(const QString& passw){
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

void Stegano::setUseCrypt(bool use){
  this->useCrypt = use;
  emit useCryptChanged(this->useCrypt);
}

void Stegano::setSourceMedia(QString source){
  this->sourceMedia = source;
  emit sourceMediaChanged(KUrl(this->sourceMedia));
}

void Stegano::hideData(const QByteArray& source){
	QByteArray chiffre = QByteArray(source);
	// terminator setzen.
	chiffre.append(0xff);
	if (this->useCrypt)
	  chiffre = encryptData(chiffre);

	QImage img = QImage(this->sourceMedia);
	BitIterator bi(chiffre);

  /*
   *             float progGes = img.Width * img.Height * 3;
            int lastProg = 0;
            int currProg = -1;
            int i = 0;
            for (int x = 0; x < img.Width - 1; x++)
                for (int y = 0; y < img.Height - 1; y++)
                {
                    if (bw.CancellationPending)
                    {
                        e.Cancel = true;
                        return;
                    }
                    Color pixelColor = bm.GetPixel(x, y);
                    Color nColor = Color.FromArgb((int)((pixelColor.R & 0xFE) | bi.GetBit(i++)),
                                                    (int)((pixelColor.G & 0xFE) | bi.GetBit(i++)),
                                                    (int)((pixelColor.B & 0xFE) | bi.GetBit(i++)));
                    bm.SetPixel(x, y, nColor);
                    currProg = (int)((x * y * 3 * 100) / progGes);
                    if(lastProg != currProg)
                        bw.ReportProgress(lastProg = currProg);
                }
            hideImg = bm;
            e.Result = hideImg;
   */


	int colorChannels = 3; //img.colorCount();
	float progGes = img.width() * img.height() * colorChannels;
	int lastProg = 0;
	int currProg = -1;
	QSize size = img.size();
	
	for (int x = 0; x < size.width(); x++) {
		for (int y = 0; y < size.height(); ++y) {
		  //if(y % 100 == 0 || x % 100 == 0)
		  currProg = (int)((x * y * colorChannels * 100) / progGes);
		  if(currProg > lastProg)
		    emit( lastProg = currProg );
		  
		    //QRgb pix = img.pixel(x, y);
		    //qDebug("Pixel ", x, y);
			//if(bw.Channcel)
			//QColor pixelColor =
			
		}
	}
}

QByteArray Stegano::encryptData(const QByteArray& buf){
	return buf;
}
