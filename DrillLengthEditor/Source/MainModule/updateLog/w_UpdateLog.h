#ifndef W_UpdateLog_H
#define W_UpdateLog_H

#include <QtWidgets/QDialog>
#include "ui_W_UpdateLog.h"

/*
-----==========================================================-----
		�ࣺ		���ڴ���.h
		����ģ�飺	������ģ��
		���ܣ�		���ڴ��ڡ�
					����ϸ��cpp��
-----==========================================================-----
*/

class W_UpdateLog : public QDialog
{
	Q_OBJECT

	public:
		W_UpdateLog(QWidget *parent = 0);		//���캯��
		~W_UpdateLog();							//��������
		
	//-----------------------------------
	//----����
									//���� - ��������
									//���� - ��ȡ����
									//���� - �������� -> ui����
									//���� - ui���� -> ��������
									//���� - �ύ���ݣ�У�飩
	private:
		Ui::W_UpdateLog ui;
		
};

#endif // W_QExpressionTest_H
