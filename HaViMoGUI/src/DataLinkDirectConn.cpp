/*
 * DataLinkDirectConn.cpp
 *
 *  Created on: 27.10.2011
 *      Author: mobalegh
 */

#include "DataLinkDirectConn.h"
#include "DXL/dxl_hal.h"


DataLinkDirectConn::DataLinkDirectConn() {
	// TODO Auto-generated constructor stub

}

DataLinkDirectConn::~DataLinkDirectConn() {
	// TODO Auto-generated destructor stub
}

void DataLinkDirectConn::openPort(int port){

	opened = dxl_hal_open(port, 115200);
}
