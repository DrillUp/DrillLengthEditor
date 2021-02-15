#ifndef W_TipWaitBox_H
#define W_TipWaitBox_H

#include "ui_w_TipWaitBox.h"

/*
-----==========================================================-----
		类：		无按钮等待提示窗口.h
		所属模块：	导入模块
		功能：		仅仅提示用户进度情况。
-----==========================================================-----
*/
class W_TipWaitBox : public QDialog
{
	Q_OBJECT

	public:
		W_TipWaitBox(QString context,QWidget *parent = 0);
		~W_TipWaitBox();


	public slots:
		void changeText(QString s);

	private:
		Ui::W_TipWaitBoxClass ui;

};

#endif // W_TipWaitBox_H
