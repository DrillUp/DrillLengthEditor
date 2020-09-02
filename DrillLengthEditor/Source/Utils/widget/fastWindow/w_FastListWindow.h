#ifndef W_FastListWindow_H
#define W_FastListWindow_H

#include "ui_W_FastListWindow.h"
#include "../fastForm/p_FastForm.h"

/*
-----==========================================================-----
		类：		快速列表 窗口.h
		版本：		v1.01
		作者：		drill_up
		所属模块：	工具模块
		功能：		数据编辑的快速列表控制。
					（详细见cpp）
-----==========================================================-----
*/

class W_FastListWindow : public QDialog
{
	Q_OBJECT

	public:
		W_FastListWindow(QWidget *parent = 0);
		~W_FastListWindow();
		
	//-----------------------------------
	//----组件
	public slots:
		void addOneRow();			//添加
		void modifyOneRow();		//编辑
		void deleteOneRow();		//删除

		void refreshTable();		//刷新表格

	//-----------------------------------
	//----窗口
	public:
		bool isAddMode;
		C_FastClass local_class;
		QList<QJsonObject> local_dataList;
	public:
										//窗口 - 设置数据
		void setData(C_FastClass c_f, QList<QJsonObject> p);
										//窗口 - 取出数据
		QList<QJsonObject> getData();
										//窗口 - 本地数据 -> ui数据
		void putDataToUi();							
										//窗口 - ui数据 -> 本地数据
		void putUiToData();			

	public slots:
										//窗口 - 提交数据（校验）
		void acceptData();	
	signals:
		void acceptDataSignal(bool isAddMode, QList<QJsonObject> data);

	private:
		Ui::W_FastListWindow ui;
	//-----------------------------------
};

#endif // W_FastListWindow_H
