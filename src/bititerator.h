/*
 * BitIterator.h
 *
 *  Created on: 17.11.2009
 *      Author: sassman
 */

#ifndef BITITERATOR_H_
#define BITITERATOR_H_

#include <QObject>
#include <QByteArray>

class BitIterator : public QObject
{
    Q_OBJECT
    public:
	inline BitIterator(){}
	BitIterator(const QByteArray& );
	virtual ~BitIterator();

	QByteArray 	data();
	uint 		bit(int 	i);
	uint 		operator[](int i);
	void 		setBit(int i, uint b);

private:
	QByteArray buf;
};

#endif /* BITITERATOR_H_ */
