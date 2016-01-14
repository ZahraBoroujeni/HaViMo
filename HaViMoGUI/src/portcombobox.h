#ifndef PORTCOMBOBOX_H
#define PORTCOMBOBOX_H

#include <QComboBox>

class PortComboBox : public QComboBox
{
    Q_OBJECT

public:
    PortComboBox(QWidget *parent = 0);
    ~PortComboBox();

private:

    virtual void showPopup();

signals:
	void gotFocus (void);


};

#endif // PORTCOMBOBOX_H
