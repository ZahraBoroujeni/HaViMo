/*
 * DataLinkRoboBuilder.h
 *
 *  Created on: 21.10.2011
 *      Author: mobalegh
 */

#ifndef DATALINKROBOBUILDER_H_
#define DATALINKROBOBUILDER_H_

#include "DataLinkUSB2Dyn.h"

class DataLinkRoboBuilder: public DataLinkUSB2Dyn {
public:
	DataLinkRoboBuilder();
	virtual ~DataLinkRoboBuilder();

	virtual bool findPorts(QStringList &s);

	virtual void openPort(int port);

	virtual bool checkCameraLink(char* s);

	virtual bool getDataFromCamera(const char* cmd, int length, unsigned char* readBuf, int numBytesToRead, int timeOut, QProgressDialog* progress);

	virtual bool downloadImage(unsigned char* image, QProgressDialog* progress);

	virtual bool captureRegions(HaViMoRegion *regions,char* err);

	virtual bool captureGrid(HaViMoGrid grid[][32],char* err);

	virtual bool captureGVG(GVGCell edges[][20],char* err);
};

#endif /* DATALINKROBOBUILDER_H_ */
