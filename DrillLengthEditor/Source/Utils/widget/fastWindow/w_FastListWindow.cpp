#include "stdafx.h"
#include "W_FastListWindow.h"

#include "Source/Utils/common/TTool.h"
#include "w_FastWindow.h"

/*
-----==========================================================-----
		类：		快速列表 窗口.cpp
		版本：		v1.01
		作者：		drill_up
		所属模块：	工具模块
		功能：		数据编辑的快速列表控制。
		
		目标：		1.粘贴某项覆盖到另一项。
					  复制某项到另一项。
					2.导入、导出。
		
		说明：		1.快速列表包含了 快速窗口，只配置快速列表，即可生成全部填写表单的数据。

		使用方法：
				>准备class
					this->local_class = C_FastClass("目标模型");		
					this->local_class.addDouble("位置", "0.0", true);
					this->local_class.addDouble("方向", "0.0", true);
					this->local_class.addDouble("经度", "0.0");
					this->local_class.addDouble("纬度", "0.0");
				>创建填写列表
					W_FastListWindow d(this);
					d.setData(this->local_class, this->local_data);
					if (d.exec() == QDialog::Accepted) {
						this->local_data = d.getData();
					}
-----==========================================================-----
*/

W_FastListWindow::W_FastListWindow(QWidget *parent)
	: QDialog(parent)
{
	ui.setupUi(this);

	//-----------------------------------
	//----初始化参数
	this->local_dataList = QList<QJsonObject>();
	this->local_class = C_FastClass("");

	//-----------------------------------
	//----事件绑定
	connect(ui.pushButton_add, &QPushButton::clicked, this, &W_FastListWindow::addOneRow);
	connect(ui.pushButton_modify, &QPushButton::clicked, this, &W_FastListWindow::modifyOneRow);
	connect(ui.pushButton_delete, &QPushButton::clicked, this, &W_FastListWindow::deleteOneRow);
	connect(ui.tableWidget, &QTableWidget::itemSelectionChanged, this, &W_FastListWindow::modifyOneRow);		//双击编辑行
	connect(ui.buttonBox, &QDialogButtonBox::accepted, this, &W_FastListWindow::acceptData);

	//-----------------------------------
	//----初始化ui
	//ui.tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);		//自适应大小
	ui.tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
	ui.tableWidget->setSelectionMode(QAbstractItemView::SingleSelection);				//单行模式
	TTool::_chinese_(ui.buttonBox);
}

W_FastListWindow::~W_FastListWindow(){
}


/*-----------------------------------
		添加
*/
void W_FastListWindow::addOneRow() {
	W_FastWindow d(this);
	d.setDataInAddMode(this->local_class);
	if (d.exec() == QDialog::Accepted ) {
		QJsonObject p = d.getData();
		local_dataList.append(p);
		this->refreshTable();
	}

}
/*-----------------------------------
		编辑
*/
void W_FastListWindow::modifyOneRow() {
	QList<QTableWidgetSelectionRange> range = ui.tableWidget->selectedRanges();
	if (range.size() == 0) {
		QMessageBox::warning(this, ("提示"), ("请先选择需要编辑的一行。"));
		return;
	}
	int pos = range.at(0).topRow();

	QJsonObject obj = local_dataList.at(pos);
	W_FastWindow d(this);
	d.setDataInModifyMode(this->local_class, obj);
	if (d.exec() == QDialog::Accepted) {
		QJsonObject p = d.getData();
		local_dataList.replace(pos, p);
		this->refreshTable();
	}
}
/*-----------------------------------
		删除
*/
void W_FastListWindow::deleteOneRow() {
	QList<QTableWidgetSelectionRange> range = ui.tableWidget->selectedRanges();
	if (range.size() == 0) {
		QMessageBox::warning(this, ("提示"), ("请先选择需要删除的一行。"));
		return;
	}
	int pos = range.at(0).topRow();

	local_dataList.removeAt(pos);

	this->refreshTable();
}
/*-----------------------------------
		刷新表格
*/
void W_FastListWindow::refreshTable() {

	this->local_class.fillDataInTableWidget(ui.tableWidget,this->local_dataList);

}


/*-------------------------------------------------
		窗口 - 设置数据（修改）
*/
void W_FastListWindow::setData(C_FastClass c_f, QList<QJsonObject> p) {

	// > 窗口名称
	this->setWindowTitle(c_f.getClassNameShowing() + "列表");
	ui.groupBox->setTitle(c_f.getClassNameShowing());

	// > 窗口大小
	int c_count = c_f.getConvertedParamList().count();
	int r_count = p.count();
	int ww = 350 + c_count * 35;
	if (ww > 1000){ ww = 1000; }
	int hh = 300 + r_count * 15;
	if (hh > 640){ ww = 640; }
	this->resize(ww, hh);

	// > 窗口数据
	this->local_dataList = p;
	this->local_class = c_f;
	this->isAddMode = false;
	this->putDataToUi();
}

/*-------------------------------------------------
		窗口 - 取出数据
*/
QList<QJsonObject> W_FastListWindow::getData(){
	return this->local_dataList;
};
/*-------------------------------------------------
		窗口 - 本地数据 -> ui数据
*/
void W_FastListWindow::putDataToUi() {
	//直接控制datalist
	this->refreshTable();
};
/*-------------------------------------------------
		窗口 - ui数据 -> 本地数据
*/
void W_FastListWindow::putUiToData() {
	//直接控制datalist
};
/*-------------------------------------------------
		窗口 - 提交数据（校验）
*/
void W_FastListWindow::acceptData(){
	this->putUiToData();

	emit acceptDataSignal(this->isAddMode, this->local_dataList);
	this->accept();
};