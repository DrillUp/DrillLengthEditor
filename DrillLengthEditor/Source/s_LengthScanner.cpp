#include "stdafx.h"
#include "s_LengthScanner.h"

#include <QDir>
#include <QFile>
#include <QMessageBox>
#include <QTextStream>
#pragma execution_character_set("utf-8")


/*
-----==========================================================-----
		类：		文本扫描器.h
		所属模块：	核心模块
		功能：		提供扫描、覆写直接功能。

		目标：		识别下面的文本，并作修改：
						* @Drill_LE_param "内容-%d"
						* @Drill_LE_parentKey "---内容组%d至%d---"
						* @Drill_LE_var "DrillUp.g_SSpA_context_list_length"

		说明：		这里只提供必要独立功能。
					不考虑流程性的结构。
					不考虑数据存储，只是一个执行工具。

-----==========================================================-----
*/

S_LengthScanner::S_LengthScanner() : QObject(){
	init();
}
S_LengthScanner::~S_LengthScanner() {
}
/*  - - 单例 - - */
S_LengthScanner* S_LengthScanner::cur_manager = NULL;
S_LengthScanner* S_LengthScanner::getInstance() {
	if (cur_manager == NULL) {
		cur_manager = new S_LengthScanner();
	}
	return cur_manager;
}
/*  - - 初始化 - - */
void S_LengthScanner::init() {

}

/*-------------------------------------------------
		扫描 - 获取参数 + 最大值
*/
C_LEPlugin* S_LengthScanner::doScanPlugin(QString file_name){
	C_LEPlugin* result = new C_LEPlugin();
	result->pluginName = QFileInfo(file_name).fileName();	//xxxx.js
	result->fullPath = QFileInfo(file_name).absoluteFilePath();

	// > 文件来源
	QFile file_from(file_name);	
	if (!file_from.open(QFile::ReadOnly)){
		QMessageBox message(QMessageBox::Critical, ("错误"), ("无法打开文件" + file_name ));
		message.exec();
		return result;
	}
	result->context = file_from.readAll();
	file_from.close();

	// > 扫描器准备
	P_TxtFastReader reader = P_TxtFastReader(result->context);
	reader.prepare_trimAllRows();
	reader.prepare_replaceInStrings(QRegExp("( \\* )|( \\*)|(\\* )|(/\\*:)|(/\\*:ja)"), "");	//预备去掉注释

	// > 获取插件简介
	int i_desc = reader.d_indexOf("@plugindesc", 0);
	if (i_desc != -1){
		QString desc_data = reader.d_rowAt(i_desc);
		result->pluginDesc = desc_data.replace("@plugindesc", "").trimmed();
	}

	// > 获取插件作者
	int i_author = reader.d_indexOf("@author", 0);
	if (i_author != -1){
		QString author_data = reader.d_rowAt(i_author);
		result->pluginAuthor = author_data.replace("@author", "").trimmed();
	}

	// > 禁用情况检查
	if (result->context.indexOf("@Drill_LE_editForbidden") != -1){
		result->paramForbidden = true;
		return result;
	}

	// > 获取插件参数
	int length = reader.d_rowCount();
	QList<int> index_list = reader.d_getAllRowIndexsContains("@Drill_LE_param");
	for (int i = 0; i < index_list.length(); i++){
		int i_param = index_list.at(i);
		QString aaa = reader.d_rowAt(i_param + 1);
		QString bbb = reader.d_rowAt(i_param + 2);
		if (!reader.d_rowAt(i_param + 1).contains("@Drill_LE_parentKey")){ continue; }
		if (!reader.d_rowAt(i_param + 2).contains("@Drill_LE_var")){ continue; }

		// > 获取字符串（会去掉引号）
		QString s_param = reader.d_rowAt(i_param).replace("@Drill_LE_param", "");
		QString s_parentKey = reader.d_rowAt(i_param + 1).replace("@Drill_LE_parentKey", "");
		QString s_var = reader.d_rowAt(i_param + 2).replace("@Drill_LE_var", "");
		s_param = s_param.trimmed().replace("\"", "");
		s_parentKey = s_parentKey.trimmed().replace("\"", "");
		s_var = s_var.trimmed().replace("\"", "");

		// > 加入组
		C_LEPluginParam c_p = C_LEPluginParam();
		c_p.initParam(s_param, s_parentKey, s_var);
		result->paramList.push_back(c_p);
	}

	// > 获取参数最大值
	this->refreshScanPluginLenth(result);

	return result;
}
/*-------------------------------------------------
		扫描 - 刷新参数最大值
*/
void S_LengthScanner::refreshScanPluginLenth(C_LEPlugin* plugin){

	// > 扫描器准备
	QString context = plugin->context;
	P_TxtFastReader reader = P_TxtFastReader(context);
	reader.prepare_trimAllRows();
	reader.prepare_replaceInStrings(QRegExp("( \\* )|( \\*)|(\\* )|(/\\*:)|(/\\*:ja)"), "");	//此行的去掉注释

	// > 获取 - 变量最大值
	for (int i = 0; i < plugin->paramList.count(); i++){
		C_LEPluginParam pluginParam = plugin->paramList.at(i);
		QString var_str = pluginParam.getVarName();
		if (var_str == "" || var_str == "null"){ break; }

		QString re_str = var_str.replace(".", "\\.");		//防止误识别正则
		QRegExp re = QRegExp(re_str + "[ ]*=[^=]*");
		int i_var = context.indexOf(re, 0);
		int i_varEnd = context.indexOf("\n", i_var);
		if (i_var == -1){ continue; }
		if (i_varEnd == -1){ continue; }

		// > 变量值获取
		QString data = context.mid(i_var, i_varEnd - i_var);
		QStringList dataList = data.split("=");
		if (dataList.count() == 0){ continue; }
		int var_length = TTool::_to_int_(dataList.at(1));	//榨取等号后面的int值

		pluginParam.setVarLen(var_length);
		plugin->paramList.replace(i, pluginParam);
	}

	// > 搜索 - 实际最大值
	for (int i = 0; i < plugin->paramList.count(); i++){
		C_LEPluginParam pluginParam = plugin->paramList.at(i);

		int real_length = 0; 
		QStringList param_names = reader.d_getAllRowsContains("@param");
		for (int j = 0; j < param_names.count(); j++){
			QString name = param_names.at(j);
			name = name.replace("@param", "").trimmed();
			
			QString match_str = pluginParam.getParamName(real_length + 1);
			if (match_str == name){
				real_length += 1;
			}
		}

		pluginParam.setRealLen(real_length);
		plugin->paramList.replace(i, pluginParam);
	}

}

/*-------------------------------------------------
		扫描 - 插件是否存在"xxx-10"序列格式
*/
bool S_LengthScanner::isPluginIncludedLengthParam(QString context){

	// > 扫描器准备
	P_TxtFastReader reader = P_TxtFastReader(context);
	
	// > 实际变量依次搜索
	QStringList param_names = reader.d_getAllRowsContains(QRegExp("@param[ ]+.*-[0123456789]+"));
	return param_names.count() >= 4;
}


/*-------------------------------------------------
		覆写 - 最大值修改
*/
QString S_LengthScanner::doOverwritePlugin(QString context, C_LEPluginParam param, C_LEConfigData config){
	
	// > 最大值检查
	if (config.getConfigLen() <= -1 ){
		QMessageBox::warning(nullptr, "错误", "你需要先配置" + config.getPluginName() + "的最大值。", QMessageBox::Yes);
		return "";
	}
	if (config.getConfigLen() <= 4){
		QMessageBox::warning(nullptr, "错误", "插件" + config.getPluginName() + "参数的最大值不能小于4。", QMessageBox::Yes);
		return "";
	}
	// > 相同最大值不修改
	if (config.getConfigLen() == param.getRealLen()){ return context; }
	

	QStringList context_list = context.split(QRegExp("(\n\r)|(\n)|(\r\n)"));
	// -----------------------------
	// > 不含分组的生成
	if (!param.isParentGrouping()){

		// > 找到变量起始行
		int i_param = 0;
		for (int i = 0; i < context_list.count(); i++){
			QString text = context_list.at(i);
			if (text.contains("@param") && text.contains(param.getParamName(1))){
				i_param = i;
				break;
			}
		}

		// > 找到第一个变量结构
		QStringList param_str_list = this->getParamStringList(context_list, param.getParamName(1));

		// > 迭代遍历删除行
		int name_index = param.getRealLen();
		bool deleteOn = false;
		for (int i = context_list.count() - 1; i >= i_param; i--){
			QString text = context_list.at(i);
			if (text.contains("@param") && text.contains(param.getParamName(name_index))){
				deleteOn = true;
				name_index--;
				context_list.removeAt(i);
				i++;
				continue;
			}
			if (deleteOn == true){
				if (text.contains("@") && !text.contains("@param")){
					context_list.removeAt(i);
					i++;
				}
				else{
					deleteOn = false;
				}
			}
		}

		// > 重新生成
		for (int i = 0; i < config.getConfigLen(); i++){
			QString data_str = param_str_list.join("\n");
			data_str = data_str.replace(param.getParamName(1), param.getParamName(i + 1));
			data_str = data_str.replace(QRegExp("@default[^\n]*"), "@default ");		//清空默认值
			data_str += "\n * ";
			context_list.insert(i_param + i, data_str);
		}


	// -----------------------------
	// > 含分组的生成
	}else{

		// > 找到变量起始行
		int i_param = 0;
		for (int i = 0; i < context_list.count(); i++){
			QString text = context_list.at(i);
			if (text.contains("@param") && text.contains(param.getParentName(1))){
				i_param = i;
				break;
			}
		}
		
		// > 找到第一个变量结构
		QStringList param_str_list = this->getParamStringList(context_list, param.getParamName(1));
		QStringList parent_str_list = this->getParamStringList(context_list, param.getParentName(1));

		// > 迭代遍历删除行
		int name_index = param.getRealLen();
		bool deleteOn = false;
		for (int i = context_list.count() - 1; i >= i_param; i--){
			QString text = context_list.at(i);
			if (text.contains("@param") && text.contains(param.getParamName(name_index))){
				deleteOn = true;
				name_index--;
				context_list.removeAt(i);
				i++;
				continue;
			}
			if (deleteOn == true){
				if (text.contains("@") && !text.contains("@param")){
					context_list.removeAt(i);
					i++;
				}
				else{
					deleteOn = false;
				}
			}
		}
		// > 迭代遍历删除分组
		name_index = param.getRealLen();
		for (int i = context_list.count() - 1; i >= i_param; i--){
			QString text = context_list.at(i);
			if (text.contains("@param") && text.contains(param.getParentName(name_index))){
				deleteOn = true;
				name_index -= 20;
				context_list.removeAt(i);
				i++;
				continue;
			}
			if (deleteOn == true){
				if (text.contains("@") && !text.contains("@param")){
					context_list.removeAt(i);
					i++;
				}
				else{
					deleteOn = false;
				}
			}
		}


		// > 重新生成
		for (int i = 0; i < config.getConfigLen(); i++){
			QString parent_str = "";
			QString param_str = "";
			// > 分组
			if (param.isParentGrouping() && i % 20 == 0){
				parent_str += parent_str_list.join("\n");
				parent_str = parent_str.replace(param.getParentName(1), param.getParentName(i + 1));
				parent_str += "\n * ";
				parent_str += "\n";
			}

			// > 变量
			param_str += param_str_list.join("\n");
			param_str = param_str.replace(param.getParamName(1), param.getParamName(i + 1));
			param_str = param_str.replace(QRegExp("@default[^\n]*"), "@default ");		//清空默认值
			if (param.isParentGrouping()){	//parent修改
				param_str = param_str.replace(param.getParentName(1), param.getParentName(i + 1));
			}
			param_str += "\n * ";
			context_list.insert(i_param + i, parent_str + param_str);
		}
	}

	// > 缩短多余星号
	QString result = context_list.join("\n");
	result = result.replace(QRegExp("([ \t\n\r]+\\*){5,}"),"\n * \n *");
	
	// > 替换
	QString var_str = param.getVarName();
	if (var_str != "" && var_str != "null"){
		QString re_str = var_str.replace(".", "\\.");		//防止误识别正则
		QRegExp re = QRegExp(re_str + "[ ]*=[^=]*\n");
		QString var_defineString = param.getVarName() + " = " + QString::number(config.getConfigLen()) + ";\n";
		result = result.replace(re, var_defineString);
	}

	return result;
}

/*-------------------------------------------------
		覆写 - 根据 "阶段-1" 获取到 "@param 阶段-1" 的全部参数字符串
*/
QStringList S_LengthScanner::getParamStringList(QStringList contextList, QString paramName){
	QStringList result_list = QStringList();
	bool finded = false;
	for (int i = 0; i < contextList.count(); i++){
		QString text = contextList.at(i);
		if (finded == false){
			if (text.contains("@param") && text.contains(paramName)){
				finded = true;
				result_list.append(text);
			}
		}
		else{	// finded == true
			if (text.contains("@")){
				result_list.append(text);
			}else{
				// > 结构寻找结束
				break;
			}
		}
	}
	return result_list;
}