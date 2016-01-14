/*
 * CamImageView.h
 *
 *  Created on: 10.11.2011
 *      Author: mobalegh
 */

#ifndef CAMIMAGEVIEW_H_
#define CAMIMAGEVIEW_H_

#include <QLabel>

class CamImageView: public QLabel {
Q_OBJECT
public:
signals:
	void mouseMoved(QMouseEvent* ev);
	void mouseReleased(QMouseEvent* ev);
public:
	CamImageView(QWidget* parent = 0, Qt::WindowFlags f = 0);
	CamImageView(const QString& text, QWidget* parent = 0, Qt::WindowFlags f =
			0);

	void mouseMoveEvent(QMouseEvent* ev);
	void mouseReleaseEvent( QMouseEvent* ev );
};

#endif /* CAMIMAGEVIEW_H_ */
