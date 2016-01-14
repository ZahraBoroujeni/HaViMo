/*
 * LUTView.h
 *
 *  Created on: 08.06.2011
 *      Author: mobalegh
 */

#ifndef LUTVIEW_H_
#define LUTVIEW_H_

#include <QWidget>
#include "time.h"

class LUTView: public QWidget {
Q_OBJECT

public:
	LUTView(QWidget *parent = 0);
	int selectedColor;
	bool interpolateMarking;
	unsigned char* getImagePtr();
	unsigned char* getLutPtr();
	void retriveImage(QImage *img);
	void retriveLUTOverlayImage(QImage *img);
	void redrawColorSpace();
	void eraseColor(int);
	void eraseLut(void);
	void exchangeColor(int color1,int color2);
	QColor getObjectColor(int index);
	void markLutEntryRGB(int r_, int g_, int b_, int objCode, bool interpolation);
	void ycrcb2rgb (int y,int cr,int cb, unsigned char &r,unsigned char &g,unsigned char &b);
	void rgb2ycrcb (unsigned char r,unsigned char g,unsigned char b, int &y,int &cr,int &cb);

protected:
	void mousePressEvent(QMouseEvent *event);
	void mouseMoveEvent(QMouseEvent *event);
	void mouseReleaseEvent(QMouseEvent *event);
	void paintEvent(QPaintEvent *event);


private:
	int tempx,tempy;
	QPixmap lutViewBuff;
	int lutCounter[16][16][16];
	unsigned char lut[16][16][16]; //LUT will be set via this routine
	unsigned char image[160][120]; //YCrCb image as read from the camera
	float lPoint1, lPoint2, point1, point2;

	clock_t lastmouseevent;

	QColor colors[256];

signals:
	void lutChanged (void);

};

#endif /* LUTVIEW_H_ */
