/*
 * BitIterator.cpp
 *
 *  Created on: 17.11.2009
 *      Author: sassman
 */

#include "bititerator.h"

BitIterator::BitIterator(const QByteArray& buf) {
	this->buf = buf;
}

BitIterator::~BitIterator() {
}

QByteArray BitIterator::data(){
	this->buf.resize(qstrlen(this->buf));
	return this->buf;
}

uint BitIterator::bit(int i){
    uchar curr = buf.size() > i / 8 ? buf[(i / 8)] : (char)0x00;
    return (uint)(curr >> (i % 8) & 0x01);
}

void BitIterator::setBit(int i, uint b){
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

	/* ~ -> NOT Operator to clear the i. postition in byte an than overwrite it
		* by the bit out from the source
		*/
	uchar curr = buf[i / 8];
	buf[i / 8] = (uchar)((curr & ~((b & 0x01) << i % 8)) | ((b & 0x01) << i % 8));
}


uint BitIterator::operator[](int i){
	return this->bit(i);
}
