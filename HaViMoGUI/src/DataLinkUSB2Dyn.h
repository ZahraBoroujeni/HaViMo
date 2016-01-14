/*
 * DataLinkUSB2Dyn.h
 *
 *  Created on: 23.06.2011
 *      Author: mobalegh
 */

#ifndef DATALINKUSB2DYN_H_
#define DATALINKUSB2DYN_H_

#include "DataLink.h"

#include <stdio.h>
//#include <windows.h>
#include <unistd.h>
#define CAMID 100
#ifndef DWORD
#define WINAPI
typedef unsigned long DWORD;
typedef short WCHAR;
typedef void * HANDLE;
#define MAX_PATH    PATH_MAX
typedef unsigned char BYTE;
typedef unsigned short WORD;
typedef unsigned int BOOL;
#endif
typedef long LONG;
#if !defined(_M_IX86)
 typedef double LONGLONG;
#else
 typedef double LONGLONG;
#endif
typedef union _LARGE_INTEGER {
  struct {
    DWORD LowPart;
    LONG  HighPart;
  };
  struct {
    DWORD LowPart;
    LONG  HighPart;
  } u;
  LONGLONG QuadPart;
} LARGE_INTEGER, *PLARGE_INTEGER;



class DataLinkUSB2Dyn: public DataLink{
public:
	DataLinkUSB2Dyn();
	virtual ~DataLinkUSB2Dyn();

	virtual bool findPorts(QStringList &s);

	virtual void openPort(int port);
	virtual void closePort();

	virtual bool getDataFromCamera(const char* cmd, int length, unsigned char* readBuf, int numBytesToRead, int timeOut, QProgressDialog* progress);

	virtual bool checkCameraLink(char* s);

	virtual bool downloadImage(unsigned char* image, QProgressDialog* progress);

	virtual bool readCamRegs(int addr, int count ,unsigned char* values);

	virtual bool writeCamReg(int addr, unsigned char data);

	virtual bool writeLUT(unsigned char lut[16][16][16],QProgressDialog* progress);

	virtual bool readLUT(unsigned char lut[16][16][16],QProgressDialog* progress);

	virtual bool captureRegions(HaViMoRegion* regions, char* s);

	virtual bool captureGrid(HaViMoGrid grid[][32],char* err);

	virtual bool captureGVG(GVGCell edges[][20],char* err);



protected:
	void startTimer(LARGE_INTEGER *timeStamp);
	bool isTimeout(LARGE_INTEGER *timeStamp,int mSecTimeout);
	double getElapsed(LARGE_INTEGER *timeStamp);

};



#endif /* DATALINKUSB2DYN_H_ */
