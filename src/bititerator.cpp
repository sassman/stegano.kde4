/*
 * BitIterator.cpp
 *
 *  Created on: 17.11.2009
 *      Author: sassman
 */

#include "bititerator.h"

BitIterator::BitIterator(const QByteArray& buf) {
	this->data = buf;
}

BitIterator::~BitIterator() {
}

QByteArray BitIterator::getData(){
	return this->data;
}

bool BitIterator::getBit(int i){
    char curr = data.size() > i / 8 ? data[(i / 8)] : (char)0x00;
    return (bool)(curr >> (i % 8) & 0x01);
}

bool BitIterator::operator[](int i){
	return this->getBit(i);
}
