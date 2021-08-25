#include "stdafx.h"
#include "W_UpdateLog.h"

#include "Source/Utils/common/TTool.h"


/*
-----==========================================================-----
		类：		关于窗口.cpp
		所属模块：	主窗体模块
		功能：		关于窗口。

		使用方法：
				>打开窗口
					W_UpdateLog d;
					d.exec();

-----==========================================================-----
*/
W_UpdateLog::W_UpdateLog(QWidget *parent)
	: QDialog(parent)
{
	ui.setupUi(this);

	//-----------------------------------
	//----事件绑定

	//-----------------------------------
	//----ui初始化
	TTool::_chinese_(ui.buttonBox);

}

W_UpdateLog::~W_UpdateLog(){
}
