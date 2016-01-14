/*
 * DataLinkUSB2Dyn.cpp
 *
 *  Created on: 23.06.2011
 *      Author: mobalegh
 */

#include "DataLinkUSB2Dyn.h"
#include "DXL/dxl_hal.h"
#include "qpainter.h"
#include <time.h>

DataLinkUSB2Dyn::DataLinkUSB2Dyn() {
	// TODO Auto-generated constructor stub

}

bool DataLinkUSB2Dyn::findPorts(QStringList &s){
	char st[100];
	s.clear();
	int portsfound=0;
	for (int i=0; i<20; i++){
			if (dxl_hal_open(i, 1000000)){
				sprintf(st,"%d",i);
				s.append(QString(st));
				portsfound++;
				dxl_hal_close();
			}
	}
	return (portsfound!=0);

}

DataLinkUSB2Dyn::~DataLinkUSB2Dyn() {
	// TODO Auto-generated destructor stub
}

void DataLinkUSB2Dyn::openPort(int port){

	opened = dxl_hal_open(port, 1000000);

}

void DataLinkUSB2Dyn::closePort(){

	opened = false;
	dxl_hal_close();

}

bool DataLinkUSB2Dyn::checkCameraLink(char* s){

	if (!opened) {
		sprintf(s,"Check Camera: COM not open");
		return false;
	}

	unsigned char buff[100];

	if (!getDataFromCamera("x\xff\xff\x64\x04\x0c\x00\x01\x8a",9,buff,7,300,NULL)){
		sprintf(s,"Check Camera: No PING Resp.");
		return false;
	}


	if (buff[4]==0x00 && buff[5]==0x40) camType = 10;
	else if (buff[4]==20 && buff[5]==0x42) camType=20;
    else if (buff[4]==30) camType=30;
    else if (buff[5]==0x42) camType=15;
	else camType=0;

	switch (camType){
		case 0: sprintf(s,"Check Camera: Not found"); break;
		case 10: sprintf(s,"Check Camera: HaViMo Found"); break;
		case 15: sprintf(s,"Check Camera: HaViMo1.5 Found"); break;
        case 20: sprintf(s,"Check Camera: HaViMo2 Found"); break;
        case 30: sprintf(s,"Check Camera: HaViMo3.0 Found"); break;
    }
//
//	      Button14.Enabled:=(camtype=20);
//	      CheckBox2.Enabled:=(camtype=20);
//	      Button15.Enabled:=(camtype=20);
//	      CheckBox3.Enabled:=(camtype=20);
//
	return true;
}


bool DataLinkUSB2Dyn::getDataFromCamera(const char* cmd, int length, unsigned char* readBuf, int numBytesToRead, int timeOut, QProgressDialog* progress = NULL){

    if (! opened) return false;
//    if ShowWindow then Form2.visible:=True;

    dxl_hal_clear();

    unsigned char s[100];
    memcpy(s,cmd,100);
//compute the checksum
    int num_FF_s = 0;
    unsigned char checkSum = 0;
    for (int i=0; i<length-1; i++){
    	if(num_FF_s >= 2){
    		checkSum += s[i];
    	}
    	if(s[i] == 0xff) num_FF_s++;
    }
    s[length-1] = ~checkSum;

//Write command
    dxl_hal_tx(s,length);

    LARGE_INTEGER timer1;
    LARGE_INTEGER timer2;

    startTimer(&timer1);  // overall timeout
    startTimer(&timer2);  // packet timeout

    int readPtr=0;
    do{
    	readPtr += dxl_hal_rx(readBuf+readPtr,numBytesToRead);
    	if (progress) progress->setValue(readPtr*100/numBytesToRead);
    }while((readPtr < numBytesToRead) && (!isTimeout(&timer1,timeOut)));

    if (readPtr < numBytesToRead) return false;


    return true;

}

bool DataLinkUSB2Dyn::downloadImage(unsigned char* image, QProgressDialog* progress){

	unsigned char buff[32768];

	 if (!getDataFromCamera("x\xff\xff\x64\x02\x17\x82",7,buff,19200,1000,NULL)){
		 return false;
	 }

	 for (int k= 0; k < 4; k++){
	    for (int j=0; j<30; j++){
	      for (int i= 0; i<160; i++){
	        image[i*120 + j*4+k] = buff[i+j*160+k*160*30];
	      }
	    }
	 }
	 return true;
}

bool DataLinkUSB2Dyn::readCamRegs(int addr, int count ,unsigned char* values){

	unsigned char buff[32768];
	char s[] = "x\xff\xff\x64\x04\x0c\x00\x01\x8a";
	s[6]=addr;
	s[7]=count;

	if (!getDataFromCamera(s,9,buff,6+count,300,NULL)){
		return false;
	}

	memcpy(values,buff+5,count);

	return true;
}

bool DataLinkUSB2Dyn::writeCamReg(int addr, unsigned char data){
	unsigned char buff[32768];
	char s[] = "x\xff\xff\x64\x04\x0d\x00\x01\x8a";
	s[6]=addr;
	s[7]=data;

	if (!getDataFromCamera(s,9,buff,6,300,NULL)){
		return false;
	}
	return true;

}

bool DataLinkUSB2Dyn::writeLUT(unsigned char lut[16][16][16],QProgressDialog* progress){

	unsigned char buff[1000];
	if ((!getDataFromCamera("x\xff\xff\x64\x02\x10\x89",7,buff,1,300,NULL)) || buff[0]!='*') {
		return false;
	}

	dxl_hal_clear();

	unsigned char cmd='e';
	dxl_hal_tx(&cmd,1);

    LARGE_INTEGER timer1;
    int timeOut = 1000;

    startTimer(&timer1);  // overall timeout
    while (dxl_hal_rx(buff,1)<1 && (!isTimeout(&timer1,timeOut)));
    if (isTimeout(&timer1,timeOut)) return false;

    cmd='A';
	dxl_hal_tx(&cmd,1);
    cmd=128;
	dxl_hal_tx(&cmd,1);
    cmd=0;
	dxl_hal_tx(&cmd,1);

    startTimer(&timer1);  // overall timeout
    while (dxl_hal_rx(buff,1)<1 && (!isTimeout(&timer1,timeOut)));
    if (isTimeout(&timer1,timeOut)) return false;


    unsigned int k;

    int ptr=0;
    for (int j=0; j<64; j++){
    	progress->setValue(j*100/63);
    	if (progress->wasCanceled()) break;
	    for (int i=0; i<32; i++){
	      k = lut[ptr >> 8][(ptr & 240) >> 4][ptr & 15];
	      ptr++;
	      k += (lut[ptr >> 8][(ptr & 240) >> 4][ptr & 15 ]) << 8;
	      ptr++;
	      cmd='w';
	      dxl_hal_tx(&cmd,1);
	      cmd = k;
	      dxl_hal_tx(&cmd,1);
	      k = k >> 8;
	      cmd = k;
	      dxl_hal_tx(&cmd,1);

	      startTimer(&timer1);  // overall timeout
	      while (dxl_hal_rx(buff,1)<1 && (!isTimeout(&timer1,timeOut)));
	      if (isTimeout(&timer1,timeOut)) return false;
	    }

	    cmd = 'A';
	    dxl_hal_tx(&cmd,1);
	    k = j * 32 + 128;
	    cmd = k;
		dxl_hal_tx(&cmd,1);
		k = k >> 8;
		cmd = k;
		dxl_hal_tx(&cmd,1);

		startTimer(&timer1);  // overall timeout
		while (dxl_hal_rx(buff,1)<1 && (!isTimeout(&timer1,timeOut)));
		if (isTimeout(&timer1,timeOut)) return false;

	    cmd = 'm';
	    dxl_hal_tx(&cmd,1);
		startTimer(&timer1);  // overall timeout
		while (dxl_hal_rx(buff,1)<1 && (!isTimeout(&timer1,timeOut)));
		if (isTimeout(&timer1,timeOut)) return false;
    }
    cmd = 'x';
    dxl_hal_tx(&cmd,1);
    return true;
}


bool DataLinkUSB2Dyn::readLUT(unsigned char lut[16][16][16],QProgressDialog* progress){

	unsigned char buff[1000];
    LARGE_INTEGER timer1;
    int timeOut = 100;

    if ((!getDataFromCamera("x\xff\xff\x64\x02\x10\x89",7,buff,1,300,NULL)) || buff[0]!='*') {
		return false;
	}

	unsigned char cmd='A';
	dxl_hal_tx(&cmd,1);
	cmd=128;
	dxl_hal_tx(&cmd,1);
	cmd=0;
	dxl_hal_tx(&cmd,1);

	startTimer(&timer1);  // overall timeout
	while (dxl_hal_rx(buff,1)<1 && (!isTimeout(&timer1,timeOut)));
	if (isTimeout(&timer1,timeOut)) return false;


    int ptr=0;
    for (int j=0; j<64; j++){
    	progress->setValue(j*100/63);
    	if (progress->wasCanceled()) break;
	    for (int i=0; i<32; i++){

	    	cmd='R';
			dxl_hal_tx(&cmd,1);
			startTimer(&timer1);
		    int readPtr=0;
		    int numBytesToRead = 2;
		    do
		    	readPtr += dxl_hal_rx(buff+readPtr,numBytesToRead);
		    while((readPtr < numBytesToRead) && (!isTimeout(&timer1,timeOut)));

		    if (readPtr < numBytesToRead) return false;


	      lut[ptr >> 8][(ptr & 240) >> 4][ ptr & 15] = buff[1];
	      ptr++;
	      lut[ptr >> 8][(ptr & 240) >> 4][ ptr & 15] = buff[0];
	      ptr++;
	    }
    }

    cmd = 'x';
    dxl_hal_tx(&cmd,1);
    cmd = 'x';
    dxl_hal_tx(&cmd,1);
    cmd = 'x';
    dxl_hal_tx(&cmd,1);
    return true;
}

bool DataLinkUSB2Dyn::captureRegions(HaViMoRegion *regions,char* err){

	unsigned char buff[100];

	if (!getDataFromCamera("x\xff\xff\x64\x02\x0e\x8b",7,buff,0,300,NULL)) {
		sprintf(err,"invoke err");
		return false;
	}

    sleep(100);

    LARGE_INTEGER timer1;
    int timeOut = 1000;

	startTimer(&timer1);  // overall timeout

	while (!getDataFromCamera("\xff\xff\x64\x02\x01\x98",6,buff,6,100,NULL)
	|| (buff[0]!=255)
	|| (buff[1]!=255)
	|| (buff[2]!=100)
	|| (buff[3]!=2)
	|| (buff[4]!=0)
	|| (buff[5]!= 153)){
        sleep(10);
		sprintf(err,"%s  %d,%d,%d,%d,%d,%d",err,buff[0],buff[1],buff[2],buff[3],buff[4],buff[5]);
		if (isTimeout(&timer1,timeOut)){
			return false;
		}
	}


	for (int j=1; j<16; j++){
		char s[] = "\xff\xff\x64\x04\x02\x00\x10\x98";
		s[5]=j*16;
		if(!getDataFromCamera(s,8,buff,22,200,NULL)) return false;
		memcpy((unsigned char*)(regions+j),buff+5,16);
		sprintf(err,"%s   f:%d,c:%d,n:%d,(%d,%d)-(%d,%d)\r\n",err,regions[j].Father,regions[j].Color,regions[j].NumPix,regions[j].MinX,regions[j].MinY,regions[j].MaxX,regions[j].MaxY);

	}
	return true;
}


bool DataLinkUSB2Dyn::captureGrid(HaViMoGrid grid[][32],char* err){

	unsigned char buff[100];

	if (!getDataFromCamera("x\xff\xff\x64\x02\x15\x84",7,buff,0,300,NULL)) return false;

    sleep(100);

    LARGE_INTEGER timer1;
    int timeOut = 1000;

	startTimer(&timer1);  // overall timeout

	while (!getDataFromCamera("\xff\xff\x64\x02\x01\x98",6,buff,6,100,NULL)
	|| (buff[0]!=255)
	|| (buff[1]!=255)
	|| (buff[2]!=100)
	|| (buff[3]!=2)
	|| (buff[4]!=0)
	|| (buff[5]!= 153)){
        sleep(10);
		if (isTimeout(&timer1,timeOut)) return false;
	}

	for (int j=0; j<48; j++){
		char s[] = "\xff\xff\x64\x04\x16\x00\x10\x84";
		s[5]=j;
		if(!getDataFromCamera(s,8,buff,22,200,NULL)) return false;
		memcpy((unsigned char*)(grid)+j*16,buff+5,16);
//		sprintf(err,"%s  %d",err,j);

	}
	return true;
}

bool DataLinkUSB2Dyn::captureGVG(GVGCell edges[][20],char* err){
	unsigned char buff[100];

	if (!getDataFromCamera("x\xff\xff\x64\x02\x1D\x00",7,buff,0,300,NULL)) return false; //todo add gvg invokation

    sleep(100);

    LARGE_INTEGER timer1;
    int timeOut = 1000;

	startTimer(&timer1);  // overall timeout

	while (!getDataFromCamera("\xff\xff\x64\x02\x01\x98",6,buff,6,100,NULL)
	|| (buff[0]!=255)
	|| (buff[1]!=255)
	|| (buff[2]!=100)
	|| (buff[3]!=2)
	|| (buff[4]!=0)
	|| (buff[5]!= 153)){
        sleep(10);
		if (isTimeout(&timer1,timeOut)) return false;
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




void DataLinkUSB2Dyn::startTimer(LARGE_INTEGER *timeStamp){
    struct timespec now;
    clock_gettime(CLOCK_MONOTONIC, &now);
//      return now.tv_sec + now.tv_nsec / 1000000000.0;
//    QueryPerformanceCounter( timeStamp);
}
bool DataLinkUSB2Dyn::isTimeout(LARGE_INTEGER *timeStamp,int mSecTimeout){

	return (getElapsed(timeStamp) > mSecTimeout);

}
double DataLinkUSB2Dyn::getElapsed(LARGE_INTEGER *timeStamp){

//	LARGE_INTEGER now,freq;

//	QueryPerformanceCounter( &now);
//	QueryPerformanceFrequency( &freq );

//	return ((double)(now.QuadPart - timeStamp->QuadPart) / (double)freq.QuadPart * 1000.0);


    struct timespec now;
    clock_gettime(CLOCK_MONOTONIC, &now);
    return (now.tv_sec + now.tv_nsec)- timeStamp->QuadPart / 1000000000000.0;

}
