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
		����
*/
DrillLengthEditor* DrillLengthEditor::cur_instance = NULL;
DrillLengthEditor* DrillLengthEditor::getInstance() {
	if (cur_instance == NULL) {
		cur_instance = new DrillLengthEditor(NULL);
	}
	return cur_instance;
}
/*-------------------------------------------------
		��ʼ��
*/
void DrillLengthEditor::_init() {

	this->m_open_successed = false;
	this->m_single_filePath = "";

	this->m_isHideUneditableFile = false;
	this->m_batch_dirPath = "";

	//-----------------------------------
	//----�¼���
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

	// > �п�����
	ui.tableWidget_b->setColumnWidth(0, 180);
	ui.tableWidget_b->setColumnWidth(1, 240);
	ui.tableWidget_b->setColumnWidth(2, 90);
	ui.tableWidget_b->setColumnWidth(3, 320);
}

/* --------------------------------------------------------------
		����
*/
void DrillLengthEditor::toStepBatch(){
	ui.tabWidget->setCurrentIndex(1);
}
void DrillLengthEditor::toStepSingle(){
	ui.tabWidget->setCurrentIndex(2);
}
void DrillLengthEditor::openWord(){
	QFileInfo info("���ڲ�����ֵ�༭��.docx");
	if (!info.exists()){
		QMessageBox::warning(this, "����", "�ĵ������ˡ�", QMessageBox::Yes);
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
		���ļ� - ��ȡ������ļ�
*/
void DrillLengthEditor::openSingleFile() {
	QFileDialog fd;
	fd.setWindowTitle("������");
	fd.setAcceptMode(QFileDialog::AcceptOpen);		//�Ի������ͣ���/���棩����������ļ�������ʾ��
	fd.setNameFilters(QStringList()	<< "js�ļ�(*.js)");
	fd.setViewMode(QFileDialog::Detail);
	if (fd.exec() == QDialog::Accepted) {
		if (fd.selectedFiles().empty()) {
			//��û��ѡ���ļ�����������
			return;
		}
		m_single_filePath = fd.selectedFiles().at(0);
	}
	else {
		//������رջ���ȡ�������������
		return;
	}

	// > ���������Ϣ
	ui.lineEdit_s_fileurl->setText(m_single_filePath);

	// > �򿪲��
	this->m_open_successed = S_LengthFlowManager::getInstance()->openSingle(m_single_filePath);

	// > ���ش��� - ��ȡʧ��
	C_LEAnnotation* p = S_LengthFlowManager::getInstance()->getLastSinglePlugin();
	if (p->isNull()){
		QMessageBox::warning(this, "����", "�����ȡʧ�ܡ�", QMessageBox::Yes);
	}

	// > ��ˢui
	this->refreshSingleUi();
}
/* --------------------------------------------------------------
		���ļ� - �༭���
*/
void DrillLengthEditor::openSingleParamEdit(){
	S_LengthFlowManager::getInstance()->editSingle();
	this->refreshSingleUi();
}
/* --------------------------------------------------------------
		���ļ� - ��ˢui
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


	// > ��ȡ�ɹ�
	if (this->m_open_successed){
		ui.toolButton_s_editParam->setEnabled(true);
		ui.toolButton_s_submit->setEnabled(true);

	// > һ����� - �޷��޸�
	}else{
		ui.toolButton_s_editParam->setEnabled(false);
		ui.toolButton_s_submit->setEnabled(false);
	}
}

/* --------------------------------------------------------------
		���ļ� - ��һ��
*/
void DrillLengthEditor::toNextSingle(){
	if (ui.lineEdit_s_fileurl->text() == ""){
		QMessageBox::warning(this, "��ʾ", "����ѡ��һ��js�ļ���", QMessageBox::Yes);
		return;
	}
	ui.stackedWidget_2->setCurrentIndex(1);
}
/* --------------------------------------------------------------
		���ļ� - ��һ��
*/
void DrillLengthEditor::toLastSingle(){
	ui.stackedWidget_2->setCurrentIndex(0);
}
/* --------------------------------------------------------------
		���ļ� - �ύ
*/
void DrillLengthEditor::submitSingle(){
	S_LengthFlowManager::getInstance()->executeSingle();
	this->refreshSingleUi();
}


/* --------------------------------------------------------------
		�����޸� - ��ȡ����ļ���
*/
void DrillLengthEditor::openBatchDir(){
	
	// > �û�ѡ���ļ���
	QFileDialog fd;	
	this->m_batch_dirPath = fd.getExistingDirectory(this, "��ѡ�񺬶��js������ļ���", "./");
	if (this->m_batch_dirPath == ""){
		return;
	}

	// > �򿪲��	
	S_LengthFlowManager::getInstance()->openBatch(m_batch_dirPath);

	// > ͳ���ļ���Ϣ
	QDir dir_from(m_batch_dirPath);
	QFileInfoList d_list = dir_from.entryInfoList(QDir::Files | QDir::NoDotAndDotDot);
	int count = 0;
	for (int i = 0; i < d_list.count(); i++){
		QFileInfo info = d_list.at(i);
		if (info.suffix() == "js"){
			count++;
		}
	}
	ui.label_b_fileMessage->setText("js�ļ�����:" + QString::number(count));

	
	// > ��ˢui
	this->refreshBatchUi();
}
/* --------------------------------------------------------------
		�����޸� - ѡ��һ���༭
*/
void DrillLengthEditor::openBatchParamOneEdit(){
	QList<QTableWidgetSelectionRange> range = ui.tableWidget_b->selectedRanges();
	if (range.size() == 0) {
		QMessageBox::warning(this, ("��ʾ"), ("����ѡ����Ҫ�༭��һ�С�"));
		return;
	}
	int pos = range.at(0).topRow();
	int index = ui.tableWidget_b->item(pos, 0)->toolTip().toInt();

	// > �򿪲��
	QList<C_LEAnnotation*> p_list = S_LengthFlowManager::getInstance()->getLastBatchPlugin();
	S_LengthFlowManager::getInstance()->editBatchOne(p_list.at(index));

	this->refreshBatchUi();
}
/* --------------------------------------------------------------
		�����޸� - ��ˢui
*/
void DrillLengthEditor::refreshBatchUi(){

	// > �ļ���·����Ϣ
	ui.lineEdit_b_dir->setText(m_batch_dirPath);

	QList<C_LEAnnotation*> p_list = S_LengthFlowManager::getInstance()->getLastBatchPlugin();

	// > �����Ϣ
	int row_count = 0;
	ui.tableWidget_b->setRowCount(p_list.size());
	for (int i = 0; i < p_list.size(); i++) {
		C_LEAnnotation* p = p_list.at(i);
		if (this->m_isHideUneditableFile == true){
			if (p->message.contains("��") == false){ continue; }
		}
		QTableWidgetItem *item = new QTableWidgetItem(p->pluginName);
		item->setToolTip(QString::number(i));		//����������
		ui.tableWidget_b->setItem(row_count, 0, item);
		ui.tableWidget_b->setItem(row_count, 1, new QTableWidgetItem(p->pluginDesc));
		ui.tableWidget_b->setItem(row_count, 2, new QTableWidgetItem(p->pluginAuthor));

		// > �����Ϣ
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
		�����޸� - ��һ��
*/
void DrillLengthEditor::toNextBatch(){
	if (ui.lineEdit_b_dir->text() == ""){
		QMessageBox::warning(this, "��ʾ", "����ѡ��һ���ļ��С�", QMessageBox::Yes);
		return;
	}
	ui.stackedWidget->setCurrentIndex(1);
}
/* --------------------------------------------------------------
		�����޸� - ��һ��
*/
void DrillLengthEditor::toLastBatch(){
	ui.stackedWidget->setCurrentIndex(0);
}
/* --------------------------------------------------------------
		�����޸� - �ύ
*/
void DrillLengthEditor::submitBatch(){
	S_LengthFlowManager::getInstance()->executeBatch();
	this->refreshBatchUi();
}
/* --------------------------------------------------------------
		�����޸� - ���ز����޸��ļ�
*/
void DrillLengthEditor::checkBoxChanged(bool b){
	this->m_isHideUneditableFile = ui.checkBox->isChecked();
	this->refreshBatchUi();
}
/* --------------------------------------------------------------
		�����޸� - ��������
*/
void DrillLengthEditor::importBatch(){
	S_LengthFlowManager::getInstance()->importConfig();
	this->refreshBatchUi();
}
/* --------------------------------------------------------------
		�����޸� - ��������
*/
void DrillLengthEditor::exportBatch(){
	S_LengthFlowManager::getInstance()->exportConfig();
	this->refreshBatchUi();
}