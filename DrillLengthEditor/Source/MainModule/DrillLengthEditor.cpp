#include "stdafx.h"
#include "DrillLengthEditor.h"

#include "updateLog/w_UpdateLog.h"
#include "about/w_SoftwareAbout.h"
#include "Source/FlowModule/s_LengthFlowManager.h"

DrillLengthEditor::DrillLengthEditor(QWidget *parent)
	: QMainWindow(parent)
{
	DrillLengthEditor::cur_instance = this;
	ui.setupUi(this);
	this->_init();
}

DrillLengthEditor::~DrillLengthEditor()
{

}
/*-------------------------------------------------
		单例
*/
DrillLengthEditor* DrillLengthEditor::cur_instance = NULL;
DrillLengthEditor* DrillLengthEditor::getInstance() {
	if (cur_instance == NULL) {
		cur_instance = new DrillLengthEditor(NULL);
	}
	return cur_instance;
}
/*-------------------------------------------------
		初始化
*/
void DrillLengthEditor::_init() {

	this->m_open_successed = false;
	this->m_single_filePath = "";

	this->m_isHideUneditableFile = false;
	this->m_batch_dirPath = "";

	//-----------------------------------
	//----事件绑定
	connect(ui.toolButton_batchEdit, &QToolButton::clicked, this, &DrillLengthEditor::toStepBatch);
	connect(ui.toolButton_singleEdit, &QToolButton::clicked, this, &DrillLengthEditor::toStepSingle);
	connect(ui.toolButton_openWord, &QToolButton::clicked, this, &DrillLengthEditor::openWord);
	connect(ui.toolButton_updateLog, &QToolButton::clicked, this, &DrillLengthEditor::openUpdateLog);
	connect(ui.toolButton_author, &QToolButton::clicked, this, &DrillLengthEditor::openAbout);

	connect(ui.toolButton_s_openFile, &QToolButton::clicked, this, &DrillLengthEditor::openSingleFile);
	connect(ui.toolButton_s_editParam, &QToolButton::clicked, this, &DrillLengthEditor::openSingleParamEdit);
	connect(ui.pushButton_s_next, &QPushButton::clicked, this, &DrillLengthEditor::toNextSingle);
	connect(ui.pushButton_s_last, &QPushButton::clicked, this, &DrillLengthEditor::toLastSingle);
	connect(ui.toolButton_s_submit, &QToolButton::clicked, this, &DrillLengthEditor::submitSingle);

	connect(ui.toolButton_b_open, &QToolButton::clicked, this, &DrillLengthEditor::openBatchDir);
	connect(ui.tableWidget_b, &QTableWidget::doubleClicked, this, &DrillLengthEditor::openBatchParamOneEdit);
	connect(ui.pushButton_b_next, &QPushButton::clicked, this, &DrillLengthEditor::toNextBatch);
	connect(ui.pushButton_b_last, &QPushButton::clicked, this, &DrillLengthEditor::toLastBatch);
	connect(ui.toolButton_b_submit, &QToolButton::clicked, this, &DrillLengthEditor::submitBatch);
	connect(ui.checkBox, &QCheckBox::toggled, this, &DrillLengthEditor::checkBoxChanged);

	connect(ui.toolButton_b_import, &QToolButton::clicked, this, &DrillLengthEditor::importBatch);
	connect(ui.toolButton_b_export, &QToolButton::clicked, this, &DrillLengthEditor::exportBatch);

	// > 列宽设置
	ui.tableWidget_b->setColumnWidth(0, 180);
	ui.tableWidget_b->setColumnWidth(1, 240);
	ui.tableWidget_b->setColumnWidth(2, 90);
	ui.tableWidget_b->setColumnWidth(3, 320);
}

/* --------------------------------------------------------------
		步骤
*/
void DrillLengthEditor::toStepBatch(){
	ui.tabWidget->setCurrentIndex(1);
}
void DrillLengthEditor::toStepSingle(){
	ui.tabWidget->setCurrentIndex(2);
}
void DrillLengthEditor::openWord(){
	QFileInfo info("关于插件最大值编辑器.docx");
	if (!info.exists()){
		QMessageBox::warning(this, "错误", "文档不见了。", QMessageBox::Yes);
		return;
	}
	QDesktopServices::openUrl(QUrl("file:///" + info.absoluteFilePath()));
}
void DrillLengthEditor::openUpdateLog(){
	W_UpdateLog d(this);
	d.exec();
}
void DrillLengthEditor::openAbout(){
	W_SoftwareAbout d(this);
	d.exec();
}

/* --------------------------------------------------------------
		单文件 - 读取插件单文件
*/
void DrillLengthEditor::openSingleFile() {
	QFileDialog fd;
	fd.setWindowTitle("导入插件");
	fd.setAcceptMode(QFileDialog::AcceptOpen);		//对话框类型（打开/保存）（保存会有文件覆盖提示）
	fd.setNameFilters(QStringList()	<< "js文件(*.js)");
	fd.setViewMode(QFileDialog::Detail);
	if (fd.exec() == QDialog::Accepted) {
		if (fd.selectedFiles().empty()) {
			//（没有选择文件进入的情况）
			return;
		}
		m_single_filePath = fd.selectedFiles().at(0);
	}
	else {
		//（点击关闭或者取消操作的情况）
		return;
	}

	// > 插件基本信息
	ui.lineEdit_s_fileurl->setText(m_single_filePath);

	// > 打开插件
	this->m_open_successed = S_LengthFlowManager::getInstance()->openSingle(m_single_filePath);

	// > 严重错误 - 读取失败
	C_LEAnnotation* p = S_LengthFlowManager::getInstance()->getLastSinglePlugin();
	if (p->isNull()){
		QMessageBox::warning(this, "错误", "插件读取失败。", QMessageBox::Yes);
	}

	// > 重刷ui
	this->refreshSingleUi();
}
/* --------------------------------------------------------------
		单文件 - 编辑插件
*/
void DrillLengthEditor::openSingleParamEdit(){
	S_LengthFlowManager::getInstance()->editSingle();
	this->refreshSingleUi();
}
/* --------------------------------------------------------------
		单文件 - 重刷ui
*/
void DrillLengthEditor::refreshSingleUi(){
	C_LEAnnotation* p = S_LengthFlowManager::getInstance()->getLastSinglePlugin();

	ui.lineEdit_s_pluginName->setText(p->pluginName);
	ui.lineEdit_s_pluginDesc->setText(p->pluginDesc);
	ui.lineEdit_s_pluginAuthor->setText(p->pluginAuthor);
	QString message = p->message;
	if (p->editMessage != ""){
		message += "\n" + p->editMessage;
		ui.label_s_pluginMsg->setStyleSheet("color: rgb(29, 0, 255);");
		ui.label_s_pluginMsg->setText(message);
	}else{
		ui.label_s_pluginMsg->setStyleSheet("");
		ui.label_s_pluginMsg->setText(message);
	}


	// > 读取成功
	if (this->m_open_successed){
		ui.toolButton_s_editParam->setEnabled(true);
		ui.toolButton_s_submit->setEnabled(true);

	// > 一般错误 - 无法修改
	}else{
		ui.toolButton_s_editParam->setEnabled(false);
		ui.toolButton_s_submit->setEnabled(false);
	}
}

/* --------------------------------------------------------------
		单文件 - 下一步
*/
void DrillLengthEditor::toNextSingle(){
	if (ui.lineEdit_s_fileurl->text() == ""){
		QMessageBox::warning(this, "提示", "请先选择一个js文件。", QMessageBox::Yes);
		return;
	}
	ui.stackedWidget_2->setCurrentIndex(1);
}
/* --------------------------------------------------------------
		单文件 - 上一步
*/
void DrillLengthEditor::toLastSingle(){
	ui.stackedWidget_2->setCurrentIndex(0);
}
/* --------------------------------------------------------------
		单文件 - 提交
*/
void DrillLengthEditor::submitSingle(){
	S_LengthFlowManager::getInstance()->executeSingle();
	this->refreshSingleUi();
}


/* --------------------------------------------------------------
		批量修改 - 读取插件文件夹
*/
void DrillLengthEditor::openBatchDir(){
	
	// > 用户选择文件夹
	QFileDialog fd;	
	this->m_batch_dirPath = fd.getExistingDirectory(this, "请选择含多个js插件的文件夹", "./");
	if (this->m_batch_dirPath == ""){
		return;
	}

	// > 打开插件	
	S_LengthFlowManager::getInstance()->openBatch(m_batch_dirPath);

	// > 统计文件信息
	QDir dir_from(m_batch_dirPath);
	QFileInfoList d_list = dir_from.entryInfoList(QDir::Files | QDir::NoDotAndDotDot);
	int count = 0;
	for (int i = 0; i < d_list.count(); i++){
		QFileInfo info = d_list.at(i);
		if (info.suffix() == "js"){
			count++;
		}
	}
	ui.label_b_fileMessage->setText("js文件数量:" + QString::number(count));

	
	// > 重刷ui
	this->refreshBatchUi();
}
/* --------------------------------------------------------------
		批量修改 - 选择一个编辑
*/
void DrillLengthEditor::openBatchParamOneEdit(){
	QList<QTableWidgetSelectionRange> range = ui.tableWidget_b->selectedRanges();
	if (range.size() == 0) {
		QMessageBox::warning(this, ("提示"), ("请先选择需要编辑的一行。"));
		return;
	}
	int pos = range.at(0).topRow();
	int index = ui.tableWidget_b->item(pos, 0)->toolTip().toInt();

	// > 打开插件
	QList<C_LEAnnotation*> p_list = S_LengthFlowManager::getInstance()->getLastBatchPlugin();
	S_LengthFlowManager::getInstance()->editBatchOne(p_list.at(index));

	this->refreshBatchUi();
}
/* --------------------------------------------------------------
		批量修改 - 重刷ui
*/
void DrillLengthEditor::refreshBatchUi(){

	// > 文件夹路径信息
	ui.lineEdit_b_dir->setText(m_batch_dirPath);

	QList<C_LEAnnotation*> p_list = S_LengthFlowManager::getInstance()->getLastBatchPlugin();

	// > 表格信息
	int row_count = 0;
	ui.tableWidget_b->setRowCount(p_list.size());
	for (int i = 0; i < p_list.size(); i++) {
		C_LEAnnotation* p = p_list.at(i);
		if (this->m_isHideUneditableFile == true){
			if (p->message.contains("【") == false){ continue; }
		}
		QTableWidgetItem *item = new QTableWidgetItem(p->pluginName);
		item->setToolTip(QString::number(i));		//在这里藏序号
		ui.tableWidget_b->setItem(row_count, 0, item);
		ui.tableWidget_b->setItem(row_count, 1, new QTableWidgetItem(p->pluginDesc));
		ui.tableWidget_b->setItem(row_count, 2, new QTableWidgetItem(p->pluginAuthor));

		// > 插件信息
		QWidget* qw = new QWidget();
		QHBoxLayout* qh = new QHBoxLayout(qw);
		QLabel* label = new QLabel();
		QString message = p->message;
		if (p->editMessage != ""){ 
			message += "\n" + p->editMessage;
			label->setStyleSheet("color: rgb(29, 0, 255);");
			label->setText(message);
		}else{
			label->setStyleSheet("");
			label->setText(message);
		}
		qh->addWidget(label);
		qh->setMargin(6);
		qh->setSpacing(0);
		ui.tableWidget_b->setCellWidget(row_count, 3, qw);

		row_count++;
	}
	ui.tableWidget_b->setRowCount(row_count);
	ui.tableWidget_b->resizeRowsToContents();
}

/* --------------------------------------------------------------
		批量修改 - 下一步
*/
void DrillLengthEditor::toNextBatch(){
	if (ui.lineEdit_b_dir->text() == ""){
		QMessageBox::warning(this, "提示", "请先选择一个文件夹。", QMessageBox::Yes);
		return;
	}
	ui.stackedWidget->setCurrentIndex(1);
}
/* --------------------------------------------------------------
		批量修改 - 上一步
*/
void DrillLengthEditor::toLastBatch(){
	ui.stackedWidget->setCurrentIndex(0);
}
/* --------------------------------------------------------------
		批量修改 - 提交
*/
void DrillLengthEditor::submitBatch(){
	S_LengthFlowManager::getInstance()->executeBatch();
	this->refreshBatchUi();
}
/* --------------------------------------------------------------
		批量修改 - 隐藏不可修改文件
*/
void DrillLengthEditor::checkBoxChanged(bool b){
	this->m_isHideUneditableFile = ui.checkBox->isChecked();
	this->refreshBatchUi();
}
/* --------------------------------------------------------------
		批量修改 - 导入配置
*/
void DrillLengthEditor::importBatch(){
	S_LengthFlowManager::getInstance()->importConfig();
	this->refreshBatchUi();
}
/* --------------------------------------------------------------
		批量修改 - 导出配置
*/
void DrillLengthEditor::exportBatch(){
	S_LengthFlowManager::getInstance()->exportConfig();
	this->refreshBatchUi();
}