#include "stdafx.h"
#include "W_UpdateLog.h"

#include "Source/Utils/common/TTool.h"


/*
-----==========================================================-----
		�ࣺ		���ڴ���.cpp
		����ģ�飺	������ģ��
		���ܣ�		���ڴ��ڡ�

		ʹ�÷�����
				>�򿪴���
					W_UpdateLog d;
					d.exec();

-----==========================================================-----
*/
W_UpdateLog::W_UpdateLog(QWidget *parent)
	: QDialog(parent)
{
	ui.setupUi(this);

	//-----------------------------------
	//----�¼���

	//-----------------------------------
	//----ui��ʼ��
	TTool::_chinese_(ui.buttonBox);

}

W_UpdateLog::~W_UpdateLog(){
}
