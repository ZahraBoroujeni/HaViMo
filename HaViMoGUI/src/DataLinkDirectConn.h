/*
 * DataLinkDirectConn.h
 *
 *  Created on: 27.10.2011
 *      Author: mobalegh
 */

#ifndef DATALINKDIRECTCONN_H_
#define DATALINKDIRECTCONN_H_

#include "DataLinkRoboBuilder.h"

class DataLinkDirectConn: public DataLinkRoboBuilder {
public:
	DataLinkDirectConn();
	virtual ~DataLinkDirectConn();
	virtual void openPort(int port);
};

#endif /* DATALINKDIRECTCONN_H_ */
