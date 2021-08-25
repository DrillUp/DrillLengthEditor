#ifndef W_UpdateLog_H
#define W_UpdateLog_H

#include <QtWidgets/QDialog>
#include "ui_W_UpdateLog.h"

/*
-----==========================================================-----
		类：		关于窗口.h
		所属模块：	主窗体模块
		功能：		关于窗口。
					（详细见cpp）
-----==========================================================-----
*/

class W_UpdateLog : public QDialog
{
	Q_OBJECT

	public:
		W_UpdateLog(QWidget *parent = 0);		//构造函数
		~W_UpdateLog();							//析构函数
		
	//-----------------------------------
	//----窗口
									//窗口 - 设置数据
									//窗口 - 获取数据
									//窗口 - 本地数据 -> ui数据
									//窗口 - ui数据 -> 本地数据
									//窗口 - 提交数据（校验）
	private:
		Ui::W_UpdateLog ui;
		
};

#endif // W_QExpressionTest_H
