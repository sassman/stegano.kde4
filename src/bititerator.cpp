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
	return this->buf;
}

bool BitIterator::bit(int i){
    char curr = buf.size() > i / 8 ? buf[(i / 8)] : (char)0x00;
    return (bool)(curr >> (i % 8) & 0x01);
}

bool BitIterator::operator[](int i){
	return this->bit(i);
}
