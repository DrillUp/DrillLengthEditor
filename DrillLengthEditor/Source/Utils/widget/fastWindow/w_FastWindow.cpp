#include "stdafx.h"
#include "W_FastWindow.h"

#include "Source/Utils/common/TTool.h"

/*
-----==========================================================-----
		类：		快速窗口 窗口.cpp
		版本：		v1.01
		作者：		drill_up
		所属模块：	工具模块
		功能：		快速填写表单参数的窗口。

		使用方法：
				>准备class
					this->local_class = C_FastClass("目标模型");		
					this->local_class.addDouble("位置", "0.0", true);
					this->local_class.addDouble("方向", "0.0", true);
					this->local_class.addDouble("经度", "0.0");
					this->local_class.addDouble("纬度", "0.0");
				>创建填写窗口
					W_FastWindow d(this);
					d.setDataInAddMode(this->local_class);
					if (d.exec() == QDialog::Accepted ) {
						QJsonObject p = d.getData();
						this->local_dataList.append(p);
					}
-----==========================================================-----
*/

W_FastWindow::W_FastWindow(QWidget *parent)
	: QDialog(parent)
{
	ui.setupUi(this);

	//-----------------------------------
	//----初始化参数
	this->m_fastForm = new P_FastForm(ui.widget_params);
	this->local_class = C_FastClass("");
	this->local_data = QJsonObject();

	//-----------------------------------
	//----事件绑定
	connect(ui.buttonBox, &QDialogButtonBox::accepted, this, &W_FastWindow::acceptData);

	//-----------------------------------
	//----初始化ui
	TTool::_chinese_(ui.buttonBox);
}

W_FastWindow::~W_FastWindow(){
}


/*-------------------------------------------------
		回车事件过滤
*/
void W_FastWindow::keyPressEvent(QKeyEvent *event) {
	if (event->key() == Qt::Key_Enter || event->key() == Qt::Key_Return) {
		this->focusNextChild();
	}else{
		QDialog::keyPressEvent(event);
	}
}



/*-------------------------------------------------
		窗口 - 设置数据（添加）
*/
void W_FastWindow::setDataInAddMode(C_FastClass c_f){
	this->setWindowTitle("添加" + c_f.getClassNameShowing());
	this->m_fastForm->prepareFastClass(c_f);
	this->m_fastForm->prepareQSpacerItem();
	this->m_fastForm->rebuildUI();

	this->local_data = QJsonObject();
	this->local_class = c_f;
	this->isAddMode = true;
	this->putDataToUi();
}
/*-------------------------------------------------
		窗口 - 设置数据（修改）
*/
void W_FastWindow::setDataInModifyMode(C_FastClass c_f, QJsonObject p) {
	this->setWindowTitle("编辑" + c_f.getClassNameShowing());
	this->m_fastForm->prepareFastClass(c_f);
	this->m_fastForm->prepareQSpacerItem();
	this->m_fastForm->rebuildUI();

	this->local_data = p;
	this->local_class = c_f;
	this->isAddMode = false;
	this->putDataToUi();
}

/*-------------------------------------------------
		窗口 - 取出数据
*/
QJsonObject W_FastWindow::getData(){
	return this->local_data;
};
/*-------------------------------------------------
		窗口 - 本地数据 -> ui数据
*/
void W_FastWindow::putDataToUi() {
	this->m_fastForm->setJsonObjectAutoDefault(this->local_data);
};
/*-------------------------------------------------
		窗口 - ui数据 -> 本地数据
*/
void W_FastWindow::putUiToData() {
	this->local_data = this->m_fastForm->getJsonObject();
};
/*-------------------------------------------------
		窗口 - 提交数据（校验）
*/
void W_FastWindow::acceptData(){
	this->putUiToData();

	// > 空校验
	if (this->m_fastForm->isAllNotNullConform() == false ) {
		QMessageBox::warning(this, "提示", "必填项不能为空。");
		return;
	}
	// > 重复校验
	//if (isAddMode && S_SourceDataContainer::getInstance()->hasTagName(local_SourceVoltage.name)) {
	//	QMessageBox::warning(this, "提示", "标签已存在。");
	//	return;
	//}

	emit acceptDataSignal(this->isAddMode, this->local_data);
	this->accept();
};