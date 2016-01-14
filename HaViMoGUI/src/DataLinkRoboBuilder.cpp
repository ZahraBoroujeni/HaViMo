/*
 * DataLinkRoboBuilder.cpp
 *
 *  Created on: 21.10.2011
 *      Author: mobalegh
 */

#include "DataLinkRoboBuilder.h"
#include "DXL/dxl_hal.h"
#include <unistd.h>

DataLinkRoboBuilder::DataLinkRoboBuilder() {
	// TODO Auto-generated constructor stub

}

DataLinkRoboBuilder::~DataLinkRoboBuilder() {
	// TODO Auto-generated destructor stub
}

bool DataLinkRoboBuilder::findPorts(QStringList &s) {
	char st[100];
	s.clear();
	int portsfound = 0;
	for (int i = 0; i < 20; i++) {
		if (dxl_hal_open(i, 115200)) {
			sprintf(st, "%d", i);
			s.append(QString(st));
			portsfound++;
			dxl_hal_close();
		}
	}
	return (portsfound != 0);

}

void DataLinkRoboBuilder::openPort(int port) {

	opened = dxl_hal_open(port, 115200);

	// code to enter direct mode in rbc
	unsigned char s[] =
			"\xFF\xFF\xAA\x55\xAA\x55\x37\xBA\x10\x00\x00\x00\x00\x01\x01\x01";
	dxl_hal_tx(s, sizeof(s));
    sleep(200);
	dxl_hal_rx(s, sizeof(s));

}

bool DataLinkRoboBuilder::checkCameraLink(char* s) {

	if (!opened) {
		sprintf(s, "Check Camera: COM not open");
		return false;
	}

	unsigned char buff[100];

	if (!getDataFromCamera("x\xff\xff\x64\x04\x0c\x00\x01\x8a", 9, buff, 7,
			300, NULL)) {
		sprintf(s, "Check Camera: No PING Resp.");
		return false;
	}

    if (buff[5] == 0x42)
		camType = 20;
    else{
        if (!getDataFromCamera("x\xff\xff\x64\x04\x0c\x1d\x01\x6d", 9, buff, 7,
                300, NULL)) {
            sprintf(s, "Check Camera: No PING Resp.");
            return false;
        }
        if (buff[5] == 0xA2)
            camType = 30;
        else
            camType = 0;
    }

	switch (camType) {
	case 0:
		sprintf(s, "Check Camera: Not found");
		break;
    case 20:
        sprintf(s, "Check Camera: HaViMo2 Found");
        break;
    case 30:
        sprintf(s, "Check Camera: HaViMo3.0 Found");
        break;
    }

	return true;
}

bool DataLinkRoboBuilder::getDataFromCamera(const char* cmd, int length,
		unsigned char* readBuf, int numBytesToRead, int timeOut,
		QProgressDialog* progress) {

	if (!opened)
		return false;
	//    if ShowWindow then Form2.visible:=True;

	dxl_hal_clear();

	int offset = 5;
	if (numBytesToRead == 6) //PING
		numBytesToRead = 8;
	if (numBytesToRead == 1) {//LUT
		numBytesToRead = 7;
		offset = 0;
	}
	if (numBytesToRead == 0) {//SAMPLE
		numBytesToRead = 6;
		offset = 0;
	}

	unsigned char s[100];
	memcpy(s, cmd, 100);

	int i = 0;
	for (i = 0; (i < 100 && (s[i] != 0xFF || s[i + 1] != 0xFF)); i++)
		;
	i += 2;
	//compute the checksum

	s[i + 5] = (s[i + 4] ^ s[i + 3] ^ s[i + 2]) & 127;
	s[i + 1] = 0xFE;
	s[i] = 0xFF;

	//Write command
	dxl_hal_tx(s + i, 6);

	//    printf("sent: %d %d %d %d %d %d \r\n",s[i],s[i+1],s[i+2],s[i+3],s[i+4],s[i+5]);

	LARGE_INTEGER timer1;
	LARGE_INTEGER timer2;

	startTimer(&timer1); // overall timeout
	startTimer(&timer2); // packet timeout

	int readPtr = 0;
	do {
		readPtr += dxl_hal_rx(readBuf + readPtr + offset, numBytesToRead); //added 5 for campatibility to robotis
		if (progress)
			progress->setValue(readPtr * 100 / (numBytesToRead - 6));
	} while ((readPtr < numBytesToRead - 6) && (!isTimeout(&timer1, timeOut)));

	if (readPtr < numBytesToRead - 6)
		return false;

	//    if ShowWindow then Form2.visible:=False;

	return true;

}

bool DataLinkRoboBuilder::downloadImage(unsigned char* image,
		QProgressDialog* progress) {

	unsigned char buff[32768];

	if (!getDataFromCamera("x\xff\xff\x64\x02\x0f\x00\x00\x00", 9, buff, 19205,
			10000, progress)) {
		return false;
	}

	for (int k = 0; k < 40; k++) {
		progress->setValue(k * 100 / 40);
		for (int j = 0; j < 120; j++) {
			for (int i = 0; i < 4; i++) {
				image[(i + k * 4) * 120 + j] = buff[i + j * 4 + k * 480 + 5];
			}
		}
	}

    sleep(100);

	return true;

}

bool DataLinkRoboBuilder::captureRegions(HaViMoRegion *regions, char* err) {

	unsigned char buff[100];

	if (!getDataFromCamera("x\xff\xff\x64\x02\x0e\x8b", 7, buff, 0, 300, NULL)){
		sprintf(err,"%s, error in capture",err);
		return false;
	}

    sleep(100);

	LARGE_INTEGER timer1;
	int timeOut = 1000;

	startTimer(&timer1); // overall timeout

	while (!getDataFromCamera("\xff\xff\x64\x02\x01\x98", 6, buff, 6, 100, NULL)
			|| (buff[5] != 0) || (buff[6] != 0)) {
        sleep(10);
//		sprintf(err,"%s  %d,%d,%d,%d,%d,%d.%d\r\n",err,buff[0],buff[1],buff[2],buff[3],buff[4],buff[5],buff[6]);
		if (isTimeout(&timer1, timeOut)){
			sprintf(err,"%s, timeout in capt.",err);
			return false;
		}
	}

	for (int j = 1; j < 16; j++) {
		char s[] = "\xff\xff\x64\x04\x02\x00\x10\x98";
		s[5] = j * 16;
		if (!getDataFromCamera(s, 8, buff, 22, 100, NULL)){
			sprintf(err,"%s, error reading Capt. data",err);
			return false;
		}
		memcpy((unsigned char*) (regions + j), buff + 5, 16);
		//		sprintf(err,"%s   f:%d,c:%d,n:%d,(%d,%d)-(%d,%d)\r\n",err,regions[j].Father,regions[j].Color,regions[j].NumPix,regions[j].MinX,regions[j].MinY,regions[j].MaxX,regions[j].MaxY);

	}
	return true;
}

bool DataLinkRoboBuilder::captureGrid(HaViMoGrid grid[][32], char* err) {

	unsigned char buff[100];

	if (!getDataFromCamera("x\xff\xff\x64\x02\x15\x84", 7, buff, 0, 300, NULL))
		return false;

    sleep(100);

	LARGE_INTEGER timer1;
	int timeOut = 1000;

	startTimer(&timer1); // overall timeout

	while (!getDataFromCamera("\xff\xff\x64\x02\x01\x98", 6, buff, 6, 100, NULL)
			|| (buff[5] != 0) || (buff[6] != 0)) {

        sleep(10);
		if (isTimeout(&timer1, timeOut))
			return false;
	}

	for (int j = 0; j < 48; j++) {
		char s[] = "\xff\xff\x64\x04\x16\x00\x10\x84";
		s[5] = j;
		if (!getDataFromCamera(s, 8, buff, 22, 200, NULL))
			return false;
		memcpy((unsigned char*) (grid) + j * 16, buff + 5, 16);
//		sprintf(err, "%s  %d", err, j);

	}
	return true;
}
bool DataLinkRoboBuilder::captureGVG(GVGCell edges[][20],char* err){
	unsigned char buff[100];

	if (!getDataFromCamera("x\xff\xff\x64\x02\x1D\x8C", 7, buff, 0, 300, NULL))
		return false;

    sleep(100);

	LARGE_INTEGER timer1;
	int timeOut = 1000;

	startTimer(&timer1); // overall timeout

	while (!getDataFromCamera("\xff\xff\x64\x02\x01\x98", 6, buff, 6, 100, NULL)
			|| (buff[5] != 0) || (buff[6] != 0)) {

        sleep(10);
		if (isTimeout(&timer1, timeOut))
			return false;
	}

	for (int j=0; j<20; j++){
		char s[] = "\xff\xff\x64\x04\x16\x00\x10\x84";
		s[5]=j;
		if(!getDataFromCamera(s,8,buff,22,200,NULL)) return false;
		memcpy((unsigned char*)(edges)+j*16,buff+5,16);
//		sprintf(err,"%s  %d",err,j);

	}
	return true;
}
