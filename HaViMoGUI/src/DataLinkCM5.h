/*
 * DataLinkCM5.h
 *
 *  Created on: 16.09.2011
 *      Author: mobalegh
 */

#ifndef DATALINKCM5_H_
#define DATALINKCM5_H_

#include "DataLinkUSB2Dyn.h"

class DataLinkCM5: public DataLinkUSB2Dyn {
public:
	DataLinkCM5();
	virtual ~DataLinkCM5();

	virtual bool findPorts(QStringList &s);

	virtual void openPort(int port);

	virtual bool downloadImage(unsigned char* image, QProgressDialog* progress);

};

#endif /* DATALINKCM5_H_ */
