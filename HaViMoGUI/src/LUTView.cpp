/*
 * LUTView.cpp
 *
 *  Created on: 08.06.2011
 *      Author: mobalegh
 */

#include "LUTView.h"
#include <QStylePainter>
#include <QPainter>
#include <QtGui>
#include <string.h>

#define PI 3.1415926536

LUTView::LUTView(QWidget *parent) :
	QWidget(parent) {

	tempx = 0;
	tempy = 0;
	lutViewBuff = QPixmap(500, 300);
	lutViewBuff.fill(qRgb(128, 128, 128));

	selectedColor = 0;
	interpolateMarking = false;
	colors[0] = Qt::black;
	colors[1] =	Qt::cyan;
	colors[2] = Qt::red;
	colors[3] = Qt::darkYellow;
	colors[4] = Qt::darkMagenta;
	colors[5] = Qt::white;
	colors[6] = Qt::blue;
	colors[7] = Qt::green;

}

void LUTView::rgb2ycrcb(unsigned char r, unsigned char g, unsigned char b,
		int &y, int &cr, int &cb) {
	y = ((77 * r + 150 * g + 29 * b) / 256 + 8) & 240;
	cb = ((-44 * r - 87 * g + 131 * b) / 256 + 128 + 8) & 240;
	cr = ((131 * r - 110 * g - 21 * b) / 256 + 128 + 8) & 240;
}

void LUTView::ycrcb2rgb(int y, int cr, int cb, unsigned char &r,
		unsigned char &g, unsigned char &b) {
	float calcR = y * 16 + 1.371 * (cr * 16 - 128);
	float calcG = y * 16 - 0.698 * (cr * 16 - 128) - 0.336 * (cb * 16 - 128);
	float calcB = y * 16 + 1.732 * (cb * 16 - 128);

	if (calcR < 0)
		calcR = 0;
	if (calcG < 0)
		calcG = 0;
	if (calcB < 0)
		calcB = 0;
	if (calcR > 255)
		calcR = 255;
	if (calcG > 255)
		calcG = 255;
	if (calcB > 255)
		calcB = 255;
	r = calcR;
	g = calcG;
	b = calcB;

}

void LUTView::paintEvent(QPaintEvent *event) {

	QStylePainter painter(this);
	painter.drawPixmap(0, 0, lutViewBuff);

}

void LUTView::mousePressEvent(QMouseEvent *event) {
	tempx = event->x();
	tempy = event->y();
}

void LUTView::markLutEntryRGB(int r_, int g_, int b_, int objCode, bool interpolation){
	if (r_ == 0 && g_ == 0 && b_ == 0)
		return;

	int y, cr, cb;
	unsigned char r, g, b;
	bool colNotFound = true;
	for (y = 0; y < 16 && colNotFound; y++)
		for (cr = 0; cr < 16 && colNotFound; cr++)
			for (cb = 0; cb < 16 && colNotFound; cb++) {
				ycrcb2rgb(y, cr, cb, r, g, b);
				if (r == r_ && g == g_ && b
						== b_){
					if (interpolation && objCode != 0) {
						for (int y_ = 0; y_ < 16; y_++)
							for (int cr_ = 0; cr_ < 16; cr_++)
								for (int cb_ = 0; cb_ < 16; cb_++)
									if (lut[y_][cr_][cb_] == objCode)
										for (int o = 0; o < 16; o++)
											lut[( y_ * o + y  * (16 - o) + 8) / 16]
											   [(cr_ * o + cr * (16 - o) + 8) / 16]
											   [(cb_ * o + cb * (16 - o) + 8) / 16]
											    = objCode | 128;
						for (int y_ = 0; y_ < 16; y_++)
							for (int cr_ = 0; cr_ < 16; cr_++)
								for (int cb_ = 0; cb_ < 16; cb_++)
									lut[y_][cr_][cb_] &= 127;

					}
					lut[y][cr][cb] = objCode;
					colNotFound = false;
				}
			}
	emit
	lutChanged();

	redrawColorSpace();

}

void LUTView::mouseReleaseEvent(QMouseEvent *event) {


	if (lPoint1 == point1 && lPoint2 == point2) {

		QColor color = lutViewBuff.toImage().pixel(event->pos());
		markLutEntryRGB(color.red(),color.green(),color.blue(),selectedColor,interpolateMarking);


		//		QPainter painter(&lutViewBuff);
		//		QPen pen(Qt::black, 1, Qt::SolidLine);
		//		pen.setColor(qRgb(255, 0, 0));
		//		painter.setPen(pen);
		//	    QFont f( "times", 18, QFont::Bold );
		//	    painter.setFont( f );
		//
		//	    painter.drawLine(0,0,20,20);
		//        QString n;
		//        n.sprintf( "y(%d) cr(%d) cb(%d)", y,cr,cb );
		//        painter.drawText( 30, 30, n );           // draw the hue number


	}

	lPoint1 = point1;
	lPoint2 = point2;
}
void LUTView::mouseMoveEvent(QMouseEvent *event) {

	clock_t duration;

	duration = clock() - lastmouseevent;

	if ((double) duration * 1000 / ((double) CLOCKS_PER_SEC) < 30)
		return;

	lastmouseevent = clock();

	if (event->x() > 499 || event->y() > 299 || event->x() < 0 || event->y()
			< 0)
		return;

	if ((event->buttons() & Qt::LeftButton) == 0)
		return;

	point1 = lPoint1 + (event->x() - tempx) * PI / 180;
	point2 = lPoint2 + (event->y() - tempy) * PI / 180;

	redrawColorSpace();

}

void LUTView::redrawColorSpace() {
	unsigned char r, g, b;
	int y = 0, cr = 128, cb = 128;
	float S1, S2, C1, C2;
	int XX, YY, XXCr, YYCr, XXCb, YYCb;

	memset(lutCounter, 0, sizeof(lutCounter));

	for (int j = 0; j < 120; j++) {
		for (int i = 0; i < 160; i++) {
			y = (image[i][j] & 240) >> 4;
			if (i & 1) {
				cb = (image[i][j] & 15);
			} else {
				cr = (image[i][j] & 15);
			}
			lutCounter[y][cr][cb]++;
		}
	}

	lutViewBuff.fill(qRgb(0, 0, 0));
	QPainter painter(&lutViewBuff);
	QPen pen(Qt::black, 1, Qt::SolidLine);
	pen.setColor(qRgb(255, 0, 0));
	painter.setPen(pen);

	S1 = sin(point1);
	S2 = sin(point2);
	C1 = cos(point1);
	C2 = cos(point2);

	for (y = 0; y < 16; y++)
		for (cr = 0; cr < 16; cr++)
			for (cb = 0; cb < 16; cb++) {
				int k = lutCounter[y][cr][cb];

				//				if (!k) continue;
				ycrcb2rgb(y, cr, cb, r, g, b);

				XX = ((C2 * (cr * 16 - 128) - S2 * (y * 16 - 128)) * C1 + S1
						* (cb * 16 - 128)) + 250;
				YY = (S2 * (cr * 16 - 128) + C2 * (y * 16 - 128)) + 150;

				if (XX > 0 && YY > 0 && XX < 500 && YY < 300) {

					pen.setColor(qRgb(r, g, b));
					if (k > 20)
						k = 20;
					if (k > 0) {
						painter.fillRect(XX - k / 4, YY - k / 4, k / 2, k / 2,
								pen.brush());
					}
					if (lut[y][cr][cb] != 0) {
						pen.setColor(colors[lut[y][cr][cb]]);
						painter.setPen(pen);
						painter.drawRect(XX - 5, YY - 5, 10, 10);

						pen.setColor(qRgb(r, g, b));
						painter.setPen(pen);
						painter.drawRect(XX - 2, YY - 2, 4, 4);
					}
				}
			}

	XXCr = 250 + (C2 * 128 + 128 * S2) * C1 - 0 * S1;
	YYCr = 150 + S2 * 128 - 128 * C2;
	XX = 250 + (C2 * -128 + 128 * S2) * C1 - 0 * S1;
	YY = 150 + S2 * -128 - 128 * C2;
	pen.setColor(qRgb(128, 0, 0));
	painter.setPen(pen);
	painter.drawLine(XX, YY, XXCr, YYCr);

	XX = (-0 * C2 - S2 * 128) * C1 - 0 * S1;
	YY = -0 * S2 + C2 * 128;
	pen.setColor(qRgb(0, 128, 0));
	painter.setPen(pen);
	painter.drawLine(250 + XX, 150 + YY, 250 - XX, 150 - YY);

	XXCb = 250 + (-0 * C2 + 128 * S2) * C1 + S1 * 128;
	YYCb = 150 + -0 * S2 - 128 * C2;
	XX = 250 + (-0 * C2 + 128 * S2) * C1 + S1 * -128;
	YY = 150 + -0 * S2 - 128 * C2;
	pen.setColor(qRgb(0, 0, 128));
	painter.setPen(pen);
	painter.drawLine(XX, YY, XXCb, YYCb);

	update();

}

void LUTView::retriveImage(QImage *img) {
	int y = 0, cr = 128, cb = 128;
	unsigned char r, g, b;
	for (int j = 0; j < 120; j++) {
		for (int i = 0; i < 160; i++) {
			y = (image[i][j] & 240) >> 4;
			if (i & 1) {
				cb = (image[i][j] & 15);
			} else {
				cr = (image[i][j] & 15);
			}
			ycrcb2rgb(y, cr, cb, r, g, b);
			img->setPixel(i, j, qRgb(r, g, b));
		}
	}

}

void LUTView::retriveLUTOverlayImage(QImage *img) {
	int y = 0, cr = 128, cb = 128;
	unsigned char r, g, b;
	for (int j = 0; j < 120; j++) {
		for (int i = 0; i < 160; i++) {
			y = (image[i][j] & 240) >> 4;
			if (i & 1) {
				cb = (image[i][j] & 15);
			} else {
				cr = (image[i][j] & 15);
			}
			ycrcb2rgb(y, cr, cb, r, g, b);
			if (lut[y][cr][cb] != 0) {
				img->setPixel(i, j, colors[lut[y][cr][cb]].rgb());
			} else {
				img->setPixel(i, j, qRgb(r, g, b));
			}
		}
	}
}

unsigned char* LUTView::getImagePtr() {
	return (unsigned char *) image;
}

void LUTView::eraseLut(void){
	memset(lut, 0, sizeof(lut));
	redrawColorSpace();
	emit lutChanged();

}
void LUTView::eraseColor(int color){
	for (int y = 0; y < 16; y++)
		for (int cr = 0; cr < 16; cr++)
			for (int cb = 0; cb < 16; cb++) {
				if (lut[y][cr][cb] == color) lut[y][cr][cb]=0;
			}
	redrawColorSpace();
	emit lutChanged();
}

void LUTView::exchangeColor(int color1,int color2){
	for (int y = 0; y < 16; y++)
		for (int cr = 0; cr < 16; cr++)
			for (int cb = 0; cb < 16; cb++) {
				if (lut[y][cr][cb] == color1) lut[y][cr][cb]=color2+128;
				if (lut[y][cr][cb] == color2) lut[y][cr][cb]=color1+128;
			}
	for (int y = 0; y < 16; y++)
		for (int cr = 0; cr < 16; cr++)
			for (int cb = 0; cb < 16; cb++)
				lut[y][cr][cb] &= 127;
	redrawColorSpace();
	emit lutChanged();
}

unsigned char* LUTView::getLutPtr()
{
	return (unsigned char*)lut;
}

QColor LUTView::getObjectColor(int index){
	return colors[index];
}

