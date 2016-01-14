#include "havimogui.h"
#include <stdio.h>
#include <QtWidgets>
#include <QFileDialog>
#include <QImage>

#include "DataLinkDirectConn.h"
#include "DataLinkRoboBuilder.h"
#include "DataLinkUSB2Dyn.h"
#include "DataLinkCM5.h"

#include <iostream>
#include <fstream>
using namespace std;

HaViMoGUI::HaViMoGUI(QWidget *parent) :
	QWidget(parent) {
	ui.setupUi(this);

    ui.buttonGroup->setId(ui.radioButton0, 0);
    ui.buttonGroup->setId(ui.radioButton1, 1);
    ui.buttonGroup->setId(ui.radioButton2, 2);
    ui.buttonGroup->setId(ui.radioButton3, 3);
    ui.buttonGroup->setId(ui.radioButton4, 4);
    ui.buttonGroup->setId(ui.radioButton5, 5);
    ui.buttonGroup->setId(ui.radioButton6, 6);
    ui.buttonGroup->setId(ui.radioButton7, 7);

    dLink = NULL;
	applyCamSettings=false;
	timer = new QTimer(this);
	connect(timer, SIGNAL(timeout()), this, SLOT(timerTick()));
	timer->start(300);
}

HaViMoGUI::~HaViMoGUI() {

}

void HaViMoGUI::selectConnType(int type) {
	if (dLink)
		delete dLink;
	ui.pushButton_stream->setChecked(false);
	ui.pushButton_stream->setEnabled(false);
	switch (type) {
	case 0: // USB2Dyn
		dLink = new DataLinkUSB2Dyn();
		break;
	case 1: // RoboBuilder
		dLink = new DataLinkRoboBuilder();
		break;
	case 2: // CM5
		dLink = new DataLinkCM5();
		break;
	case 3: // CM510
		dLink = new DataLinkCM5();
		break;
	case 4: // Direct Serial Connection
		dLink = new DataLinkDirectConn();
		break;
	}
	ui.comboBox_port->setEnabled(dLink);
}

void HaViMoGUI::connectClicked(bool b) {
	if (b) {
		dLink->openPort(ui.comboBox_port->currentText().toInt());
        char s[100];
        dLink->checkCameraLink(s);
        ui.status->setText(s);
	} else {
		dLink->closePort();
		ui.status->setText("Port Closed");
	}
	ui.comboBox_port->setEnabled(!b);
	ui.comboBox_conntype->setEnabled(!b);
	ui.pushButton_sample->setEnabled(b);
	ui.pushButton_stream->setEnabled(b && ui.comboBox_conntype->currentIndex() == 0);
	ui.buttonGriding->setEnabled(b);
	ui.buttonRegion->setEnabled(b);
    ui.buttonGVG->setEnabled(b && dLink->camType == 30);
	ui.pushButton_checkcamera->setEnabled(b);
	ui.autoUpdateSettings->setEnabled(b);
	ui.groupBoxDirectRegAccess->setEnabled(b);
	ui.readLUT->setEnabled(b);
	ui.WriteLUT->setEnabled(b);

}
void HaViMoGUI::colorRadioClicked(int index) {
	ui.lutView->selectedColor = index;
	//	char s[100];
	//	sprintf(s,"selected %d",index);
	//	ui.colorBox->setTitle(s);
}

void HaViMoGUI::portsFocused() {
	QStringList s;
	if (dLink->findPorts(s)) {
		ui.comboBox_port->clear();
		ui.comboBox_port->addItems(s);
	}
	ui.pushButton_connect->setEnabled(true);
}

void HaViMoGUI::checkCameraLink() {
	char s[100];
	dLink->checkCameraLink(s);
	ui.status->setText(s);
}

void HaViMoGUI::sampleImageClick() {

	//	QProgressDialog progress("Capturing Image...", NULL, 0, 100, this);
	//	progress.setMinimumDuration(100);
	//	progress.setWindowModality(Qt::WindowModal);
	//
	//	if (!dLink->downloadImage(ui.lutView->getImagePtr(), &progress)){
	//		ui.status->setText("Error Capturing the Image!");
	//		return;
	//	}
	//	ui.lutView->redrawColorSpace();
	//
	//	QImage orig(160,120,QImage::Format_ARGB32);
	//	ui.lutView->retriveImage(&orig);
	//
	//	QPixmap original;
	//	original.convertFromImage(orig);
	//
	//	ui.originalImage->setPixmap(original);
	//
	//	updateLutImage();
	//
	//	progress.setValue(100);

}

void HaViMoGUI::streamImageClick() {

}

void HaViMoGUI::updateLutImage() {
	QImage orig(160, 120, QImage::Format_ARGB32);
	ui.lutView->retriveLUTOverlayImage(&orig);

	QPixmap original;
	original.convertFromImage(orig);

	ui.overlayImage->setPixmap(original);

}

void HaViMoGUI::toggleInterpolate(bool checked) {
	ui.lutView->interpolateMarking = checked;
}

void HaViMoGUI::lutErase() {
	ui.lutView->eraseLut();
}

void HaViMoGUI::eraseColor() {
	ui.lutView->eraseColor(ui.lutView->selectedColor);
}

void HaViMoGUI::exchangeColor() {
	if (ui.lutView->selectedColor == 0 || ui.colorCombo->currentIndex() == 0)
		return;
	ui.lutView->exchangeColor(ui.lutView->selectedColor,
			ui.colorCombo->currentIndex());
}

void HaViMoGUI::camImageMouseMoved(QMouseEvent* ev) {

	if (!ui.originalImage->pixmap())
		return;
	QColor color = ui.originalImage->pixmap()->toImage().pixel(ev->pos());
	int y, cr, cb;
	ui.lutView->rgb2ycrcb(color.red(), color.green(), color.blue(), y, cr, cb);
	char s[100];
	sprintf(s, "RBG (%d,%d,%d) -> YCrCb (%d,%d,%d)", color.red(),
			color.green(), color.blue(), y, cr, cb);
	ui.status->setText(s);
}

void HaViMoGUI::camImageMouseReleased(QMouseEvent* ev) {

	if (!ui.originalImage->pixmap())
		return;
	QColor color = ui.originalImage->pixmap()->toImage().pixel(ev->pos());
	ui.lutView->markLutEntryRGB(color.red(), color.green(), color.blue(),
			ui.lutView->selectedColor, false);
}

void HaViMoGUI::timerTick() {
	if (!dLink){
		return;
	}
	if (dLink->opened && applyCamSettings && ui.autoUpdateSettings->isChecked()
			&& ui.tabWidget->currentIndex() == 1) {
        if (dLink->camType==30){
            dLink->writeCamReg(0xFF, 0x01);

            dLink->writeCamReg(0x13, (ui.autoExposure->isChecked() ? 5 : 0) + 0xC2);

            dLink->writeCamReg(0x45, (ui.exposure->value() >> 18) );
            dLink->writeCamReg(0x10, (ui.exposure->value() >> 10) & 0xFF);
            dLink->writeCamReg(0x04, ((ui.exposure->value() >> 8) & 0x03) |
                               (ui.flipHoriz->isChecked() ? 128 : 0) |
                               (ui.flipVert->isChecked() ? 64 : 0));


            dLink->writeCamReg(0x00, ui.gain->value());

            dLink->writeCamReg(0xFF, 0x00);

            dLink->writeCamReg(0xC7, (ui.autoWhiteBal->isChecked() ? 0 : 0x40));

            dLink->writeCamReg(0xCC, ui.redGain->value());
            dLink->writeCamReg(0xCD, ui.greenGain->value());
            dLink->writeCamReg(0xCE, ui.blueGain->value());

            dLink->writeCamReg(0xFF, 0x01);
        }
        else
        {
            dLink->writeCamReg(0x73, ui.exposure->value() >> 16);
            dLink->writeCamReg(0x74, ui.exposure->value() >> 8);
            dLink->writeCamReg(0x75, ui.exposure->value());

            dLink->writeCamReg(0x14, ui.redGain->value());
            dLink->writeCamReg(0x15, ui.greenGain->value());
            dLink->writeCamReg(0x16, ui.blueGain->value());

            dLink->writeCamReg(0x17, ui.gain->value());

            dLink->writeCamReg(0x5a, ui.contrast->value());
            dLink->writeCamReg(0x5b, ui.brightness->value());
            dLink->writeCamReg(0x5c, ui.saturation->value());

            dLink->writeCamReg(0x01, (ui.flipHoriz->isChecked() ? 32 : 0)
                    + (ui.flipVert->isChecked() ? 16 : 0) + 1); //see datasheet
            dLink->writeCamReg(0x70, (ui.autoExposure->isChecked() ? 1 : 0) + 0x28);
            dLink->writeCamReg(0x80, (ui.autoWhiteBal->isChecked() ? 16 : 0) + 0x08);
        }
	} else if (dLink->opened && ui.tabWidget->currentIndex() == 1
			&& ui.autoUpdateSettings->isChecked()) {

		unsigned char regs[160];

        if (dLink->camType==30){
//            dLink->writeCamReg(0x13, (ui.autoExposure->isChecked() ? 5 : 0) + 0xC2);

//            dLink->writeCamReg(0x45, (ui.exposure->value() >> 18) | 0x20);
//            dLink->writeCamReg(0x10, (ui.exposure->value() >> 10) & 0xFF);
//            dLink->writeCamReg(0x04, (ui.exposure->value()>>8) & 0x03);

        }
        else
        {

            dLink->writeCamReg(0x02, 0x00); //enabling the camera allows AWB and AE to update values

            if (dLink->readCamRegs(0, 160, regs)) {
                ui.exposure->setValue((regs[0x73] << 16) + (regs[0x74] << 8)
                        + regs[0x75]);
                ui.gain->setValue(regs[0x17]);

                ui.contrast->setValue(regs[0x5a]);
                ui.brightness->setValue(regs[0x5b]);
                ui.saturation->setValue(regs[0x5c]);

                ui.flipHoriz->setChecked(regs[0x01] & 32);
                ui.flipVert->setChecked(regs[0x01] & 16);
                ui.autoExposure->setChecked(regs[0x70] & 3);
                ui.autoWhiteBal->setChecked(regs[0x80] & 16);

                ui.redGain->setValue(regs[0x14]);
                ui.greenGain->setValue(regs[0x15]);
                ui.blueGain->setValue(regs[0x16]);
            }
		}
	}
	if ((ui.pushButton_stream->isChecked() && dLink->camType == 20)) {

		dLink->downloadImage(ui.lutView->getImagePtr(), NULL);
		ui.lutView->redrawColorSpace();

		QImage orig(160, 120, QImage::Format_ARGB32);
		ui.lutView->retriveImage(&orig);

		QPixmap original;
		original.convertFromImage(orig);

		ui.originalImage->setPixmap(original);

		updateLutImage();
	}

	if (ui.pushButton_sample->isChecked()) {
		QProgressDialog progress("Capturing Image...", NULL, 0, 100, this);
		progress.setMinimumDuration(100);
		progress.setWindowModality(Qt::WindowModal);

		if (!dLink->downloadImage(ui.lutView->getImagePtr(), &progress)) {
			ui.status->setText("Error Capturing the Image!");
			ui.pushButton_sample->setChecked(false);
			return;
		}
		ui.lutView->redrawColorSpace();

		QImage orig(160, 120, QImage::Format_ARGB32);
		ui.lutView->retriveImage(&orig);

		QPixmap original;
		original.convertFromImage(orig);

		ui.originalImage->setPixmap(original);

		updateLutImage();

		progress.setValue(100);

		ui.pushButton_sample->setChecked(false);

	}

	applyCamSettings = false;
}

void HaViMoGUI::toggleAutoUpdate(bool b) {
	if (!dLink)
		return;
    if (dLink->camType!=30) dLink->writeCamReg(0x02, b ? 0x00 : 0x10);

	ui.groupBoxImageSettings->setEnabled(b);
	ui.groupBoxColorBalance->setEnabled(b);
    ui.groupBoxPostProcessing->setEnabled(b && (dLink->camType!=30));
}

void HaViMoGUI::camSettingChanged() {
	applyCamSettings = true;
	ui.status->setText("changed");
}
void HaViMoGUI::writeLutClicked() {
	QProgressDialog progress("Writing LUT...", "Abort Write", 0, 100, this);
	progress.setMinimumDuration(100);
	progress.setWindowModality(Qt::WindowModal);

	if (dLink->writeLUT((unsigned char(*)[16][16]) ui.lutView->getLutPtr(),
			&progress))
		ui.status->setText("LUT Written Successfully");
	else
		ui.status->setText("Error Writing LUT");

	progress.setValue(100);
}

void HaViMoGUI::readLutClicked() {

	QProgressDialog progress("Reading LUT...", "Abort Read", 0, 100, this);
	progress.setMinimumDuration(100);
	progress.setWindowModality(Qt::WindowModal);

	if (dLink->readLUT((unsigned char(*)[16][16]) ui.lutView->getLutPtr(),
			&progress))
		ui.status->setText("LUT Read Successfully");
	else
		ui.status->setText("Error Reading LUT");
	ui.lutView->redrawColorSpace();
	updateLutImage();

	progress.setValue(100);
}

void HaViMoGUI::saveLutClicked() {
	QString fileName = QFileDialog::getSaveFileName(this, tr("Save LUT"), ".",
			tr("LUT files (*.lut)"));

	QByteArray ba = fileName.toLocal8Bit();
	ofstream file(ba.data(), ios::out | ios::binary);
	if (file.is_open()) {
		file.write((char *) ui.lutView->getLutPtr(), 4096);
		file.close();
		ui.status->setText("LUT Saved Successfully");
	} else
		ui.status->setText("Error Saving LUT");

}

void HaViMoGUI::loadLutClicked() {
	QString fileName = QFileDialog::getOpenFileName(this, tr("Load LUT"), ".",
			tr("LUT files (*.lut)"));

	QByteArray ba = fileName.toLocal8Bit();
	ifstream file(ba.data(), ios::out | ios::binary);
	if (file.is_open()) {
		file.read((char *) ui.lutView->getLutPtr(), 4096);
		file.close();
		ui.status->setText("LUT Loaded Successfully");
	} else
		ui.status->setText("Error Loading LUT");

	ui.lutView->redrawColorSpace();
	updateLutImage();

}

void HaViMoGUI::regionResults() {

	char s[1000];
	sprintf(s, "msg:");

	QImage orig(160, 120, QImage::Format_ARGB32);
	ui.lutView->retriveImage(&orig);

	HaViMoRegion regions[16];

	if (!dLink->captureRegions(regions, s)) {
		ui.status->setText(s);//"Error Reading Regions");
		return;
	}

	QPixmap original;
	original.convertFromImage(orig);

	QPainter painter(&original);
	QPen pen(Qt::black, 1, Qt::SolidLine);

	for (int j = 1; j < 16; j++) {
		if (regions[j].Father != 0) {
			pen.setColor(ui.lutView->getObjectColor(regions[j].Color));
			painter.setPen(pen);
			painter.drawRect(regions[j].MinX, regions[j].MinY, regions[j].MaxX
					- regions[j].MinX, regions[j].MaxY - regions[j].MinY);
		}
	}

	ui.resultImage->setPixmap(original);

	ui.status->setText(s);
}

void HaViMoGUI::gridingResults() {
	char s[1000];
	sprintf(s, " ");

	QImage orig(160, 120, QImage::Format_ARGB32);
	ui.lutView->retriveImage(&orig);

	HaViMoGrid grid[24][32];

	if (!dLink->captureGrid(grid, s)) {
		ui.status->setText("Error Reading Grid");
		return;
	}

	QPixmap original;
	original.convertFromImage(orig);

	QPainter painter(&original);
	QPen pen(Qt::black, 1, Qt::SolidLine);

	for (int j = 1; j < 24; j++) {
		for (int i = 1; i < 32; i++) {
			if (grid[j][i].count > 0) {
				pen.setColor(ui.lutView->getObjectColor(grid[j][i].color));
				painter.setPen(pen);
				painter.drawRect(i * 5 + 3 - grid[j][i].count / 6, j * 5 + 3
						- grid[j][i].count / 6, grid[j][i].count / 3,
						grid[j][i].count / 3);
			}
		}
	}

	ui.resultImage->setPixmap(original);

	ui.status->setText(s);
}


void HaViMoGUI::GVGResults() {
	char s[1000];
	sprintf(s, " ");

	QImage orig(160, 120, QImage::Format_ARGB32);
	ui.lutView->retriveImage(&orig);

	GVGCell edges[15][20];

	if (!dLink->captureGVG(edges, s)) {
		ui.status->setText("Error Reading GVG");
		return;
	}

	QPixmap original;
	original.convertFromImage(orig);

	QPainter painter(&original);
	QPen pen(Qt::black, 1, Qt::SolidLine);

	for (int j = 0; j < 15; j++) {
		for (int i = 0; i < 20; i++) {
			if (edges[j][i].intensity != 0 && edges[j][i].pos != 0) {
				int x,y,dx,dy;
				int pos = (int)edges[j][i].pos - 4;
				switch (edges[j][i].dir)
				{
				case 0:
					y = 0;
					x = pos;
					dx = 0;
					dy = 1;
					break;
				case 4:
					y = 0;
					x = -pos;
					dx = 0;
					dy = -1;
					break;
				case 2:
					x = 0;
					y = pos;
					dx = -1;
					dy = 0;
					break;
				case 6:
					x = 0;
					y = -pos;
					dx = 1;
					dy = 0;
					break;
				case 1:
					x = pos;
					y = pos;
					dx = -1;
					dy = 1;
					break;
				case 5:
					x = -pos;
					y = -pos;
					dx = 1;
					dy = -1;
					break;
				case 7:
					x = pos;
					y = -pos;
					dx = 1;
					dy = 1;
					break;
				case 3:
					x = -pos;
					y = pos;
					dx = -1;
					dy = -1;
					break;
				}
				pen.setColor(Qt::red);
				painter.setPen(pen);
				painter.drawEllipse(i * 8 + x + 4 - 1, j * 8 + y + 4 - 1, 2, 2);
				pen.setColor(Qt::green);
				painter.setPen(pen);
				painter.drawLine(i * 8 + x + 4 , j * 8 + y + 4 , i * 8 + x + 4 + dx * 4 , j * 8 + y + 4 + dy * 4);
			}
		}
	}

	ui.resultImage->setPixmap(original);

	ui.status->setText(s);
}


void HaViMoGUI::resetFactory() {
	FILE *fp;
	int addr, data;
	fp = fopen("camerareg15.txt", "r");
	if (fp == NULL) {
		fclose(fp);
		ui.status->setText("Error Opening camerareg15.txt");
		return;
	}

	QProgressDialog progress("Writing Camera Settings...", "Abort Write", 0,
			100, this);
	progress.setMinimumDuration(100);
	progress.setWindowModality(Qt::WindowModal);

	while (fscanf(fp, "%X %X", &addr, &data) != EOF) {
		dLink->writeCamReg(addr, data);
		progress.setValue(addr * 100 / 160);
	}
	progress.setValue(100);
}

void HaViMoGUI::writeReg() {
    dLink->writeCamReg(ui.regAddr->value(), ui.regData->value());
}

void HaViMoGUI::readReg() {
	unsigned char i;
    dLink->readCamRegs(ui.regAddr->value(), 1, &i);
	ui.regData->setValue(i);
}
