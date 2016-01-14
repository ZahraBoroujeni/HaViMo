/*
 * DataLink.h
 *
 *  Created on: 23.06.2011
 *      Author: mobalegh
 */

#ifndef DATALINK_H_
#define DATALINK_H_

#include "qstringlist.h"
#include <QProgressDialog>

struct HaViMoRegion{
	unsigned char Father,Color;
	unsigned short int NumPix;
	unsigned int SumX,SumY;
	unsigned char MaxX,MinX,MaxY,MinY;
};
struct HaViMoGrid{
	unsigned char color:4;
	unsigned char count:4;
};
struct GVGCell{
	unsigned char dir:3;
	unsigned char pos:3;
	unsigned char intensity:2;
};

class DataLink {
public:
	DataLink();
	virtual ~DataLink();

	virtual bool findPorts(QStringList &s) = 0;

	virtual void openPort(int port) = 0;
	virtual void closePort() = 0;

	virtual bool getDataFromCamera(const char* cmd, int length, unsigned char* readBuf, int numBytesToRead, int timeOut, QProgressDialog* progress) = 0;

	virtual bool checkCameraLink(char* s) = 0;

	virtual bool downloadImage(unsigned char* image, QProgressDialog* progress) = 0;

	virtual bool readCamRegs(int addr, int count ,unsigned char* values) = 0;

	virtual bool writeCamReg(int addr, unsigned char data) = 0;

	virtual bool writeLUT(unsigned char lut[16][16][16],QProgressDialog* progress) = 0;

	virtual bool readLUT(unsigned char lut[16][16][16],QProgressDialog* progress) = 0;

	virtual bool captureRegions(HaViMoRegion* regions, char* s) = 0;

	virtual bool captureGrid(HaViMoGrid grid[][32],char* err) = 0;

	virtual bool captureGVG(GVGCell edges[][20],char* err) = 0;

	bool opened;

	int camType;

};

#endif /* DATALINK_H_ */
