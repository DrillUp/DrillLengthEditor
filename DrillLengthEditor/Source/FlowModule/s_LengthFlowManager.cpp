#include "stdafx.h"
#include "S_LengthFlowManager.h"

#include "Source/PluginModule/lengthEditor/s_LEAnnotationReader.h"
#include "Source/PluginModule/lengthEditor/s_LEConfigWriter.h"
#include "W_TipWaitBox.h"

#include "Source/Utils/widget/fastForm/c_FastClass.h"
#include "Source/Utils/widget/fastWindow/w_FastWindow.h"


/*
-----==========================================================-----
		类：		最大值编辑 流程管理.h
		所属模块：	流程模块
		功能：		属于 核心 与 ui 的中介。
					存储内部的配置数据，ui根据流程中的数据可以随时刷新。

		目标：		单文件控制流程
						> 打开文件
						> 编辑文件
						> 执行文件覆盖

					批量文件控制流程
						> 打开文件夹
						> 编辑文件
						> 导入导出配置
						> 执行批量文件覆盖
		
		说明：		由于两个流程写在同一个类中，看起来会有些眼花。
					流程性的东西都非常绕，需要反复看。
					这里主要留意一下数据类的传递。打debug断点查看。

-----==========================================================-----
*/

S_LengthFlowManager::S_LengthFlowManager(){
	init();
}
S_LengthFlowManager::~S_LengthFlowManager() {
}
/*  - - 单例 - - */
S_LengthFlowManager* S_LengthFlowManager::cur_manager = NULL;
S_LengthFlowManager* S_LengthFlowManager::getInstance() {
	if (cur_manager == NULL) {
		cur_manager = new S_LengthFlowManager();
	}
	return cur_manager;
}
/*  - - 初始化 - - */
void S_LengthFlowManager::init() {

}


/*-------------------------------------------------
		单文件 - 打开
*/
bool S_LengthFlowManager::openSingle(QString file_name){
	
	this->m_single_plugin = S_LEAnnotationReader::getInstance()->readPlugin(QFileInfo(file_name));

	// > 插件检查
	QStringList temp_list = QStringList();
	if (m_single_plugin->isForbidden() == true){
		temp_list.push_back("该插件禁止编辑最大值。");
		this->m_single_plugin->message = temp_list.join("\n");
		return false;
	}
	if (m_single_plugin->isEditable() == false){
		temp_list.push_back("未找到该插件的最大值。");
		if (S_LEAnnotationReader::getInstance()->isPluginIncludedLengthParam(m_single_plugin->context)){		//存在"xxx-10"的插件
			temp_list.push_back("插件的版本可能比较旧。");
		}
		this->m_single_plugin->message = temp_list.join("\n");
		return false;
	}
	if (m_single_plugin->isIntegrity() == false){
		temp_list.push_back("插件结构完整性存在问题，含残缺项，无法编辑。");
		this->m_single_plugin->message = temp_list.join("\n");
		return false;
	}
	
	// > 获取插件参数信息
	QString paramMsg = "";
	QList<C_LEAnnotation_Param> params = m_single_plugin->paramList;
	for (int i = 0; i < params.count(); i++){
		C_LEAnnotation_Param param = params.at(i);
		paramMsg += param.getParamShowingName() + "【" + QString::number(param.getRealLen()) + "】";
		if (i != params.count()-1 ){
			paramMsg += ",";
		}
	}
	this->m_single_plugin->message = paramMsg;
	
	return true;
}
/*-------------------------------------------------
		单文件 - 获取打开后的插件数据
*/
C_LEAnnotation* S_LengthFlowManager::getLastSinglePlugin(){
	return m_single_plugin;
}
/*-------------------------------------------------
		单文件 - 编辑插件
*/
void S_LengthFlowManager::editSingle(){
	if (m_single_plugin->isNull()){ return; }
	QJsonObject obj = this->editOneWithWindow(m_single_plugin, m_single_data);
	if (obj.empty()){ return; }

	// > 清空 单文件 的配置
	this->m_single_data = QList<C_LEConfigData>();

	// > 填入配置
	QList<C_LEAnnotation_Param> params = m_single_plugin->paramList;
	for (int i = 0; i < params.count(); i++){
		C_LEAnnotation_Param param = params.at(i);

		QString name_p = m_single_plugin->pluginName;
		QString name_c = param.getParamKey();
		int configLen = obj.value(name_p + name_c + "最大值").toString().toInt();

		C_LEConfigData data = C_LEConfigData();
		data.initParam(name_p, name_c, configLen);
		this->m_single_data.push_back(data);
	}
	
	// > 刷新消息
	QString message = "";
	for (int j = 0; j < m_single_data.count(); j++){
		C_LEConfigData data = m_single_data.at(j);
		message += data.getParamShowingName() + "【" + QString::number(data.getConfigLen()) + "】";
		if (j != m_single_data.count() - 1){
			message += ",";
		}
	}
	if (message != ""){
		//message = "<span style='color:#00aa00;'>修改为：" + message + "</span>";	//（改为用stylesheet）
		message = "修改为：" + message;
	}
	m_single_plugin->editMessage = message;

}
/*-------------------------------------------------
		单文件 - 执行修改
*/
void S_LengthFlowManager::executeSingle(){

	// > 数据修改
	for (int i = 0; i < m_single_data.count();i++){
		C_LEConfigData config = m_single_data.at(i);
		C_LEAnnotation_Param param = m_single_plugin->getParamByKey(config.getParamKey());

		QString context = S_LEConfigWriter::getInstance()->doOverwritePlugin(m_single_plugin->context, param, config);
		if (context == ""){ return; }
		m_single_plugin->context = context;
	}

	// > 写入
	QTextCodec::codecForName("utf-8");
	QFile file_to(m_single_plugin->fullPath);
	if (!file_to.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
		QMessageBox::warning(nullptr, "错误", "无法打开文件。", QMessageBox::Yes);
		return;
	}
	QTextStream streamFileOut(&file_to);
	streamFileOut.setCodec("UTF-8");
	streamFileOut.flush();
	streamFileOut << m_single_plugin->context;
	file_to.close();
	QMessageBox::information(nullptr, "提示", "修改成功。", QMessageBox::Yes);
	
	// > 重新打开插件
	this->openSingle(m_single_plugin->fullPath);
	// > 清空编辑信息
	this->m_single_data = QList<C_LEConfigData>();
}



/*-------------------------------------------------
		批量文件 - 打开
*/
void S_LengthFlowManager::openBatch(QString dir_name){
	QDir dir_from(dir_name);

	// > 文件夹遍历
	QFileInfoList d_list = dir_from.entryInfoList(QDir::Files | QDir::NoDotAndDotDot);
	W_TipWaitBox waitBox("插件解析中...");
	waitBox.show();

	for (int i = 0; i < d_list.count(); i++){
		QFileInfo info = d_list.at(i);
		if (info.suffix() != "js"){ continue; }
		
		C_LEAnnotation* plugin = this->openBatchOne(info.filePath());
		this->m_batch_pluginList.push_back(plugin);
	}
	waitBox.hide();

}
/*-------------------------------------------------
		批量文件 - 打开一个文件
*/
C_LEAnnotation* S_LengthFlowManager::openBatchOne(QString file_name){

	C_LEAnnotation* plugin = S_LEAnnotationReader::getInstance()->readPlugin(QFileInfo(file_name));

	// > 插件检查
	QStringList temp_list = QStringList();
	if (plugin->isForbidden() == true){
		temp_list.push_back("该插件禁止编辑最大值。");
		plugin->message = temp_list.join("\n");
		return plugin;
	}
	if (plugin->isEditable() == false){
		temp_list.push_back("未找到该插件的最大值。");
		if (S_LEAnnotationReader::getInstance()->isPluginIncludedLengthParam(plugin->context)){		//存在"xxx-10"的插件
			temp_list.push_back("插件的版本可能比较旧。");
		}
		plugin->message = temp_list.join("\n");
		return plugin;
	}
	if (plugin->isIntegrity() == false){
		temp_list.push_back("插件结构完整性存在问题，含残缺项，无法编辑。");
		plugin->message = temp_list.join("\n");
		return plugin;
	}

	// > 获取插件参数信息
	QString paramMsg = "";
	QList<C_LEAnnotation_Param> params = plugin->paramList;
	for (int j = 0; j < params.count(); j++){
		C_LEAnnotation_Param param = params.at(j);
		paramMsg += param.getParamShowingName() + "【" + QString::number(param.getRealLen()) + "】";
		if (j != params.count() - 1){
			paramMsg += ",";
		}
	}
	plugin->message = paramMsg;

	return plugin;
}

/*-------------------------------------------------
		批量文件 - 获取打开后的插件数据
*/
QList<C_LEAnnotation*> S_LengthFlowManager::getLastBatchPlugin(){
	return m_batch_pluginList;
}
/*-------------------------------------------------
		批量文件 - 编辑插件
*/
void S_LengthFlowManager::editBatchOne(C_LEAnnotation* plugin){
	if (!plugin->message.contains("【")){ return; }
	
	QJsonObject obj = this->editOneWithWindow(plugin, this->getBatchConfigDataByPlugin(plugin));
	if (obj.empty()){ return; }

	QList<C_LEAnnotation_Param> params = plugin->paramList;
	for (int i = 0; i < params.count(); i++){
		C_LEAnnotation_Param param = params.at(i);

		QString name_p = plugin->pluginName;
		QString name_c = param.getParamKey();
		int configLen = obj.value(name_p + name_c + "最大值").toString().toInt();

		C_LEConfigData data = C_LEConfigData();
		data.initParam(name_p, name_c, configLen);
		this->m_batch_dataList.removeOne(data);		//移除相同的对象
		this->m_batch_dataList.push_back(data);
		this->m_batch_storageList.removeOne(data);
		this->m_batch_storageList.push_back(data);

	}

	this->refreshBatchEditMessage();
}

/*-------------------------------------------------
		批量文件 - 执行修改
*/
void S_LengthFlowManager::executeBatch(){
	W_TipWaitBox waitBox("插件修改中...");
	waitBox.show();
	for (int i = 0; i < m_batch_pluginList.count(); i++){
		C_LEAnnotation* plugin = m_batch_pluginList.at(i);
		QList<C_LEConfigData> config_list = this->getBatchConfigDataByPlugin(plugin);
		if (config_list.count() == 0){ continue; }

		// > 数据修改
		for (int j = 0; j < config_list.count(); j++){
			C_LEConfigData config = config_list.at(j);
			C_LEAnnotation_Param param = plugin->getParamByKey(config.getParamKey());

			QString context = S_LEConfigWriter::getInstance()->doOverwritePlugin(plugin->context, param, config);
			if (context == ""){ break; }
			plugin->context = context;
		}

		// > 写入
		QTextCodec::codecForName("utf-8");
		QFile file_to(plugin->fullPath);
		if (!file_to.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
			QMessageBox::warning(nullptr, "错误", "无法打开文件。", QMessageBox::Yes);
			return;
		}
		QTextStream streamFileOut(&file_to);
		streamFileOut.setCodec("UTF-8");
		streamFileOut.flush();
		streamFileOut << plugin->context;
		file_to.close();

		// > 重新载入插件
		plugin = this->openBatchOne(plugin->fullPath);
		m_batch_pluginList.replace(i, plugin);
	}

	// > 刷新编辑数据
	this->m_batch_dataList.clear();
	this->refreshBatchEditMessage();
	waitBox.hide();
	QMessageBox::information(nullptr, "提示", "修改成功。", QMessageBox::Yes);
}


/*-------------------------------------------------
		批量文件 - 导入配置
*/
void S_LengthFlowManager::importConfig(){

	// > 选择文件
	QFileDialog fd;
	QString importFile;
	fd.setWindowTitle(("导入最大值配置"));
	fd.setAcceptMode(QFileDialog::AcceptOpen);
	fd.setNameFilters(QStringList() << ("最大值配置(*.drillle)"));
	fd.setViewMode(QFileDialog::Detail);
	if (fd.exec() == QDialog::Accepted) {
		if (fd.selectedFiles().empty()) {
			return;
		}
		importFile = fd.selectedFiles().at(0);
	}
	else{
		return;
	}

	// > 打开文件
	QFile input_file(importFile);
	if (!input_file.open(QIODevice::ReadWrite | QIODevice::Text)) {
		QMessageBox::warning(nullptr, "错误", "无法打开文件。", QMessageBox::Yes);
	}
	QTextStream in(&input_file);
	QString context = in.readAll();				//读取到input文件的所有内容

	// > 解析json
	QJsonDocument jsonDocument = QJsonDocument::fromJson(context.toUtf8());
	if (jsonDocument.isNull()){
		QMessageBox::warning(nullptr, "错误", "读取文件失败。", QMessageBox::Yes);
		return;
	}

	// > 数据转换
	QJsonObject obj_all = jsonDocument.object();
	QString obj_type = obj_all.value("dataType").toString();
	if (obj_type != "drill_LE_config"){
		QMessageBox::warning(nullptr, "错误", "文件格式不正确，请选择其他有效文件。", QMessageBox::Yes);
		return;
	}
	this->m_batch_dataList.clear();
	QJsonArray obj_arr = obj_all.value("dataList").toArray();
	for (int i = 0; i < obj_arr.count(); i++){
		QJsonObject data_obj = obj_arr.at(i).toObject();

		C_LEConfigData data = C_LEConfigData();
		data.setJsonObject(data_obj);
		this->m_batch_dataList.removeOne(data);		//移除相同的对象
		this->m_batch_dataList.push_back(data);
		this->m_batch_storageList.removeOne(data);
		this->m_batch_storageList.push_back(data);
	}

	// > 刷新编辑数据
	this->refreshBatchEditMessage();
}
/*-------------------------------------------------
		批量文件 - 导出配置
*/
void S_LengthFlowManager::exportConfig(){

	// > 选择文件
	QFileDialog fd;
	QString exportFile;
	fd.setWindowTitle(("导出最大值配置"));
	fd.setAcceptMode(QFileDialog::AcceptSave);
	fd.setNameFilters(QStringList() << ("最大值配置(*.drillle)"));
	fd.selectFile("rmmv最大值配置.drillle");
	fd.setViewMode(QFileDialog::Detail);
	if (fd.exec() == QDialog::Accepted) {
		if (fd.selectedFiles().empty()) {
			return;
		}
		exportFile = fd.selectedFiles().at(0);
	}
	else{
		return;
	}

	// > 打开写入的文件
	QFile file_to(exportFile);
	if (!file_to.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
		QMessageBox::warning(nullptr, "错误", "无法打开文件。", QMessageBox::Yes);
	}

	// > 数据转换（storageList不受大部分clear影响）
	QJsonArray obj_arr = QJsonArray();
	for (int i = 0; i < m_batch_storageList.count(); i++){
		C_LEConfigData data = m_batch_storageList.at(i);
		obj_arr.append(data.getJsonObject());
	}
	QJsonObject obj_all = QJsonObject();
	obj_all.insert("dataType", "drill_LE_config");
	obj_all.insert("dataList", obj_arr);

	// > 写入
	QString context_all = QJsonDocument(obj_all).toJson();
	file_to.write(context_all.toLocal8Bit());
	file_to.close();
}



/*-------------------------------------------------
		批量文件 - 打开窗口编辑插件
*/
QJsonObject S_LengthFlowManager::editOneWithWindow(C_LEAnnotation* plugin, QList<C_LEConfigData> data_list){
	if (plugin->isNull()){ return QJsonObject(); }

	// > 窗口类准备
	C_FastClass data_class = C_FastClass("插件配置");
	//C_FastClass temp_class = C_FastClass("插件信息");
	//temp_class.addQString("插件名", plugin->pluginName);
	//temp_class.setParamShowingName("插件名", "插件名");
	//temp_class.setParamEditable("插件名", false);
	//temp_class.addQString("插件简介", plugin->pluginDesc);
	//temp_class.setParamShowingName("插件简介", "中文名");
	//temp_class.setParamEditable("插件简介", false);
	//temp_class.addQString("插件作者", plugin->pluginAuthor);
	//temp_class.setParamShowingName("插件作者", "作者");
	//temp_class.setParamEditable("插件作者", false);
	//data_class.addFastClassQWidget(temp_class);
	QList<C_LEAnnotation_Param> params = plugin->paramList;
	for (int i = 0; i < params.count(); i++){
		C_LEAnnotation_Param param = params.at(i);

		// > 默认值
		QString name_p = plugin->pluginName;
		QString name_c = param.getParamKey();
		QString var_len = QString::number(param.getRealLen());

		C_FastClass temp_class = C_FastClass(name_c.split("-").at(0));
		temp_class.addInt					(name_p + name_c + "最大值", var_len);		//（超长名称，防止重复）
		temp_class.setParamShowingName		(name_p + name_c + "最大值", "最大值");
		
		// > 默认数据覆盖
		for (int i = 0; i < data_list.count(); i++){
			C_LEConfigData data = data_list.at(i);
			if (name_p == data.getPluginName() && name_c == data.getParamKey()){
				temp_class.setParamDefaultValue(name_p + name_c + "最大值", QString::number(data.getConfigLen()));
			}
		}

		data_class.addFastClassQGroupBox(temp_class);
	}


	// > 窗口类数据获取 + 解析
	W_FastWindow d;
	d.setDataInAddMode(data_class);
	d.setWindowTitle("更改最大值");
	if (d.exec() == QDialog::Accepted) {
		QJsonObject p = d.getData();
		return p;
	}

	return QJsonObject();
}
/*-------------------------------------------------
		批量文件 - 根据配置获取到插件
*/
QList<C_LEConfigData> S_LengthFlowManager::getBatchConfigDataByPlugin(C_LEAnnotation* plugin){
	QList<C_LEConfigData> result_list = QList<C_LEConfigData>();
	for (int i = 0; i < this->m_batch_dataList.count(); i++){
		C_LEConfigData data = this->m_batch_dataList.at(i);
		if (data.getPluginName() == plugin->pluginName){

			for (int j = 0; j < plugin->paramList.count(); j++){
				C_LEAnnotation_Param param = plugin->paramList.at(j);
				if (data.getParamKey() == param.getParamKey()){
					result_list.push_back(data);
					break;
				}
			}
		}
	}
	return result_list;
}
/*-------------------------------------------------
		批量文件 - 刷新编辑信息
*/
void S_LengthFlowManager::refreshBatchEditMessage(){
	for (int i = 0; i < m_batch_pluginList.count(); i++){
		C_LEAnnotation* plugin = m_batch_pluginList.at(i);
		QList<C_LEConfigData> data_list = this->getBatchConfigDataByPlugin(plugin);
		if (data_list.count() == 0){ continue; }

		// > 刷新单插件编辑信息
		QString message = "";
		for (int j = 0; j < data_list.count(); j++){
			C_LEConfigData data = data_list.at(j);
			message += data.getParamShowingName() + "【" + QString::number(data.getConfigLen()) + "】";
			if (j != data_list.count() - 1){
				message += ",";
			}
		}
		if (message != ""){
			//message = "<span style='color:#00aa00;'>修改为：" + message + "</span>";	//（改为用stylesheet）
			message = "修改为：" + message ;
		}
		plugin->editMessage = message;
	}
}