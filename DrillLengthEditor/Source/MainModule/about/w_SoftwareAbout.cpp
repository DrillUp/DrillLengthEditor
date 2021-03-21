#include "stdafx.h"
#include "W_SoftwareAbout.h"

#include "Source/Utils/common/TTool.h"


/*
-----==========================================================-----
		类：		关于窗口.cpp
		所属模块：	主窗体模块
		功能：		关于窗口。

		使用方法：
				>打开窗口
					W_SoftwareAbout d;
					d.exec();

-----==========================================================-----
*/
W_SoftwareAbout::W_SoftwareAbout(QWidget *parent)
	: QDialog(parent)
{
	ui.setupUi(this);

	//-----------------------------------
	//----事件绑定
	connect(ui.toolButton, &QToolButton::clicked, this, &W_SoftwareAbout::openUserManual);

	//-----------------------------------
	//----ui初始化
	TTool::_chinese_(ui.buttonBox);

}

W_SoftwareAbout::~W_SoftwareAbout(){
}


/* --------------------------------------------------------------
		控件 - 用户手册
*/
void W_SoftwareAbout::openUserManual() {
	QString sPath = qApp->applicationDirPath();

	QFileInfo info("关于插件最大值编辑器.docx");
	if (!info.exists()){
		QMessageBox::warning(this, "错误", "文档不见了。", QMessageBox::Yes);
		return;
	}
	QDesktopServices::openUrl(QUrl("file:///" + info.absoluteFilePath()));

}