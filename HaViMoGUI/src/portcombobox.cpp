#include "portcombobox.h"

PortComboBox::PortComboBox(QWidget *parent)
//    : QWidget(parent)
{
}

PortComboBox::~PortComboBox()
{

}

void PortComboBox::showPopup(){
	emit gotFocus();
	QComboBox::showPopup();
}
