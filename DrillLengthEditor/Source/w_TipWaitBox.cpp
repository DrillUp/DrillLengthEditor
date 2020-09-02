#include "stdafx.h"

#include "w_TipWaitBox.h"
#include <QtWidgets>

/*
-----==========================================================-----
		类：		无按钮等待提示窗口.cpp
		所属模块：	导入模块
		功能：		仅仅提示用户进度情况。
-----==========================================================-----
*/
W_TipWaitBox::W_TipWaitBox(QString context,QWidget *parent)
	: QDialog(parent)
{
	ui.setupUi(this);

	this->setWindowTitle(context);
	ui.label->setText(context);
	ui.label->repaint();
}

W_TipWaitBox::~W_TipWaitBox()
{

}


void W_TipWaitBox::changeText(QString s) {
	ui.label->setText(s);
	ui.label->repaint();
}


