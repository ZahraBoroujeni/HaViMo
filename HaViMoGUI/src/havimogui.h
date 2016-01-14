#ifndef HAVIMOGUI_H
#define HAVIMOGUI_H

#include <QWidget>
#include "../ui_havimogui.h"
#include "LUTView.h"
#include "DataLink.h"

class HaViMoGUI : public QWidget
{
    Q_OBJECT

public:
    HaViMoGUI(QWidget *parent = 0);
    ~HaViMoGUI();

public slots:
	void connectClicked(bool);
	void colorRadioClicked(int);
	void portsFocused();
	void checkCameraLink();
	void sampleImageClick();
	void streamImageClick();
	void updateLutImage();
	void toggleInterpolate(bool);
	void lutErase();
	void eraseColor();
	void exchangeColor();
	void camImageMouseReleased(QMouseEvent*);
	void camImageMouseMoved(QMouseEvent*);
	void timerTick();
	void toggleAutoUpdate(bool);
	void camSettingChanged();
	void writeLutClicked();
	void readLutClicked();
	void saveLutClicked();
	void loadLutClicked();
	void regionResults();
	void gridingResults();
	void GVGResults();
	void resetFactory();
	void selectConnType(int);
	void writeReg();
	void readReg();

private:
    Ui::HaViMoGUIClass ui;
    DataLink* dLink;
    QTimer *timer;
    bool applyCamSettings;


};

#endif // HAVIMOGUI_H
