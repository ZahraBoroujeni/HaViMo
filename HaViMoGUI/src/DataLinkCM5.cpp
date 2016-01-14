/*
 * DataLinkCM5.cpp
 *
 *  Created on: 16.09.2011
 *      Author: mobalegh
 */

#include "DataLinkCM5.h"
#include "DXL/dxl_hal.h"

DataLinkCM5::DataLinkCM5() {
	// TODO Auto-generated constructor stub

}

DataLinkCM5::~DataLinkCM5() {
	// TODO Auto-generated destructor stub
}

bool DataLinkCM5::findPorts(QStringList &s){
	char st[100];
	s.clear();
	int portsfound=0;
	for (int i=0; i<20; i++){
			if (dxl_hal_open(i, 57600)){
				sprintf(st,"%d",i);
				s.append(QString(st));
				portsfound++;
				dxl_hal_close();
			}
	}
	return (portsfound!=0);

}

void DataLinkCM5::openPort(int port){

	opened = dxl_hal_open(port, 57600);

	unsigned char s[]="\x0d\x0a";
	dxl_hal_tx(s,sizeof(s));
    sleep(1000);
	unsigned char ss[]="toss\x0d\x0a";
	dxl_hal_tx(ss,sizeof(ss));





}

bool DataLinkCM5::downloadImage(unsigned char* image, QProgressDialog* progress){

	unsigned char buff[32768];

    if (camType!=30) writeCamReg(16,3); //increase the HBlank to avoid buffer overflow of the CM5


	if (!getDataFromCamera("x\xff\xff\x64\x02\x0f\x8A",7,buff,19200,10000,progress)){
        if (camType!=30) writeCamReg(16,0); // reset to the default value
		return false;
	}

	for (int k = 0; k < 40; k++){
		for (int j = 0; j < 120; j++){
			for (int i = 0; i < 4; i++){
				image[(i + k * 4) * 120 + j] = buff[i + j * 4 + k * 480];
			}
		}
	}

    sleep(100);

    if (camType == 30 || writeCamReg(16,0)) return true; // reset to the default value
	return false;
}

