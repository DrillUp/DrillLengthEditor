#ifndef W_FastWindow_H
#define W_FastWindow_H

#include "ui_W_FastWindow.h"
#include "../fastForm/p_FastForm.h"

/*
-----==========================================================-----
		类：		快速窗口 窗口.h
		版本：		v1.01
		作者：		drill_up
		所属模块：	工具模块
		功能：		快速填写表单参数的窗口。
					（详细见cpp）
-----==========================================================-----
*/

class W_FastWindow : public QDialog
{
	Q_OBJECT

	public:
		W_FastWindow(QWidget *parent = 0);
		~W_FastWindow();
		
	public:
										//回车过滤
		void keyPressEvent(QKeyEvent *event);
		

	public:
		P_FastForm* m_fastForm;

	//-----------------------------------
	//----窗口
	public:
		bool isAddMode;
		C_FastClass local_class;
		QJsonObject local_data;
	public:
										//窗口 - 设置数据（添加）
		void setDataInAddMode(C_FastClass c_f);
										//窗口 - 设置数据（修改）
		void setDataInModifyMode(C_FastClass c_f, QJsonObject p);
										//窗口 - 取出数据
		QJsonObject getData();
										//窗口 - 本地数据 -> ui数据
		void putDataToUi();							
										//窗口 - ui数据 -> 本地数据
		void putUiToData();

	public slots:
										//窗口 - 提交数据（校验）
		void acceptData();	
	signals:
		void acceptDataSignal(bool isAddMode, QJsonObject data);

	private:
		Ui::W_FastWindow ui;
	//-----------------------------------
};

#endif // W_FastWindow_H
