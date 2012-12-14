/*
 * BitIterator.cpp
 *
 *  Created on: 17.11.2009
 *      Author: sassman
 */

#include "bititerator.h"
#include <QtCrypto/QtCrypto>

BitIterator::BitIterator(const QByteArray& buf) {
    this->buf = buf;
}

BitIterator::~BitIterator() {
}

QByteArray BitIterator::data() {
    // this algorithm is more safe then before, it starts from behind and looks for 0x00
    int resize = 0;
    uchar current = 0, next = -1, empty = 0x00;
    for(int i = buf.size()-1; i > 0; i--) {
        current = buf[i];
        if ( i - 1 > 0 ) {
            next = buf[i-1];
        }else {
            next = -1;
        }
        if(current == empty) {
            resize = i;
        //}else if(current == (uchar)0x60 && next == (uchar)0x00) {
        }else if(current != empty && next == empty) {
            resize = i;
        }else {
            break;
        }
    }
    if(resize > 0) {
        this->buf.resize(resize);
    }
    QCA::Initializer init;
    qDebug("SBitIterator::data: buffer '%s'", qPrintable(QCA::arrayToHex(this->buf)));        
    return this->buf;
}

uint BitIterator::bit(int i) {
    uchar curr = buf.size() > i / 8 ? buf[(i / 8)] : (char)0x00;
    return (uint)(curr >> (i % 8) & 0x01);
}

void BitIterator::setBit(int i, uint b) {
    if (i / 8 > buf.length())
        buf.resize(buf.length() + 1);

    //int round = i / 8;
    //byte current;

    //current = data[round];

    //int shifter = (i - (round * 8));
    //byte nb = (byte)(b & 0x01);
    //byte lastBit = (byte)(b & 0x01);
    //current = (byte)((current << 1) | (b & 0x01));
    //if (round < data.Length)
    //    data[round] = current;

    /** ~ -> NOT Operator to clear the i-th postition in byte and than overwrite it
     * by the bit out from the source
     */
    uchar curr = buf[i / 8];
    buf[i / 8] = (uchar)((curr & ~((b & 0x01) << i % 8)) | ((b & 0x01) << i % 8));
}


uint BitIterator::operator[](int i) {
    return this->bit(i);
}
