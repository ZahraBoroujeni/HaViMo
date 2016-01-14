/*
 * CamImageView.cpp
 *
 *  Created on: 10.11.2011
 *      Author: mobalegh
 */

#include "CamImageView.h"
#include <QtGui>

	void CamImageView::mouseMoveEvent( QMouseEvent* ev )
	{
	    emit mouseMoved( ev );
	}

	void CamImageView::mouseReleaseEvent( QMouseEvent* ev )
	{
	    emit mouseReleased( ev );
	}

	CamImageView::CamImageView( QWidget * parent, Qt::WindowFlags f )
	    : QLabel( parent, f ) {
		this->setMouseTracking(true);

	}

	CamImageView::CamImageView( const QString& text, QWidget* parent, Qt::WindowFlags f )
	    : QLabel( text, parent, f ) {
		this->setMouseTracking(true);
	}

