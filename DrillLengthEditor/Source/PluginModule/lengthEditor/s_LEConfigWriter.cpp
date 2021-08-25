#include "stdafx.h"
#include "s_LEConfigWriter.h"


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

S_LEConfigWriter::S_LEConfigWriter() : QObject(){
	init();
}
S_LEConfigWriter::~S_LEConfigWriter() {
}
/*  - - 单例 - - */
S_LEConfigWriter* S_LEConfigWriter::cur_manager = NULL;
S_LEConfigWriter* S_LEConfigWriter::getInstance() {
	if (cur_manager == NULL) {
		cur_manager = new S_LEConfigWriter();
	}
	return cur_manager;
}
/*  - - 初始化 - - */
void S_LEConfigWriter::init() {

}


/*-------------------------------------------------
		覆写 - 最大值修改
*/
QString S_LEConfigWriter::doOverwritePlugin(QString context, C_LEAnnotation_Param param, C_LEConfigData config){
	
	// > 最大值检查
	if (config.getConfigLen() <= -1 ){
		QMessageBox::warning(nullptr, "错误", "你需要先配置" + config.getPluginName() + "的最大值。", QMessageBox::Yes);
		return "";
	}
	if (config.getConfigLen() <= 4){
		QMessageBox::warning(nullptr, "错误", "插件" + config.getPluginName() + "参数的最大值不能小于4。", QMessageBox::Yes);
		return "";
	}
	if (param.getRealLen() < 4){		// （该错误只在单独调用该函数时会出现）
		QMessageBox::warning(nullptr, "错误", "使用修改器函数时，必须要知道修改的目标插件实际长度。\n（通过读取器中读插件获取）", QMessageBox::Yes);
		return "";
	}
	// > 相同最大值时，不修改
	if (config.getConfigLen() == param.getRealLen()){ return context; }
	QStringList context_list = context.split(QRegExp("(\n\r)|(\n)|(\r\n)"));

	//-----------------------------
	// >> 不含分组的生成
	if (!param.isParentGrouping()){

		// > 找到变量起始行
		int i_param = -1;
		for (int i = 0; i < context_list.count(); i++){
			QString text = context_list.at(i);
			if (text.contains(QRegExp(param.getParamCommentRe(1)))){	//必须全词匹配（"@param SV敌人-1" 不能匹配 "敌人-%d" ）
				i_param = i;
				break;
			}
		}

		// > 严重错误检查
		if (i_param == -1){
			QMessageBox::warning(nullptr, "错误", "插件" + config.getPluginName() + "转换时发生了严重错误，该错误绕开了编辑器的检查程序。\n请将该错误的具体情况告知我drill_up。", QMessageBox::Yes);
			return "";
		}

		// > 找到第一个变量结构
		QRegExp param_re = param.getParamCommentRe(1);
		QStringList param_str_list = this->getParamGroup(context_list, param_re);

		// > 获取该变量下的"@default"数据列表
		int start_row = this->indexOfRow(context_list, param_re);
		int real_len = param.getRealLen();
		QStringList defaultData_list = this->getParamList_default(context_list, start_row, real_len);

		// > 迭代遍历 删除 行
		int name_index = param.getRealLen();
		bool deleteOn = false;
		for (int i = context_list.count() - 1; i >= i_param; i--){
			QString text = context_list.at(i).trimmed();
			if (text.contains(QRegExp(param.getParamCommentRe(name_index)))){
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
			QString param_str = param_str_list.join("\n");
			QString default_str = "@default ";
			if (i < defaultData_list.count()){
				default_str.append(defaultData_list.at(i));
			}
			param_str = param_str.replace(param.getParamName(1), param.getParamName(i + 1));
			param_str = param_str.replace(QRegExp("@default[^\n]*"), default_str);		//（@default值替换）
			param_str.append("\n * ");
			context_list.insert(i_param + i, param_str);
		}


	//-----------------------------
	// >> 含分组的生成
	}else{

		// > 找到变量起始行
		int i_param = 0;
		for (int i = 0; i < context_list.count(); i++){
			QString text = context_list.at(i);
			if (text.contains(QRegExp(param.getParamCommentRe(1)))){	//必须全词匹配（"@param SV敌人-1" 不能匹配 "敌人-%d" ）
				i_param = i;
				break;
			}
		}
		int i_parent = 0;
		for (int i = 0; i < context_list.count(); i++){
			QString text = context_list.at(i);
			if (text.contains(QRegExp(param.getParentCommentRe(1)))){
				i_parent = i;
				break;
			}
		}

		// > 严重错误检查
		if (i_param == -1 || i_parent == -1){
			QMessageBox::warning(nullptr, "错误", "插件" + config.getPluginName() + "转换时发生了严重错误，该错误绕开了编辑器的检查程序。\n请将该错误的具体情况告知我drill_up。", QMessageBox::Yes);
			return "";
		}
		
		// > 找到第一个变量结构
		QRegExp param_re = param.getParamCommentRe(1);
		QRegExp parent_re = param.getParentCommentRe(1);
		QStringList param_str_list = this->getParamGroup(context_list, param_re);
		QStringList parent_str_list = this->getParamGroup(context_list, parent_re);

		// > 获取该变量下的"@default"和"@parent"数据列表（包括 分组 的数据）
		int start_row = this->indexOfRow(context_list, parent_re);
		int real_len = param.getRealLen() + qFloor(param.getRealLen()/20);	//（包括 行和分组）
		QStringList defaultData_list = this->getParamList_default(context_list, start_row, real_len);
		QStringList parentData_list = this->getParamList_parent(context_list, start_row, real_len);

		// > 迭代遍历删除 行
		int name_index = param.getRealLen();
		bool deleteOn = false;
		for (int i = context_list.count() - 1; i >= i_param; i--){
			QString text = context_list.at(i).trimmed();
			if (text.contains(QRegExp(param.getParamCommentRe(name_index)))){
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
		// > 迭代遍历删除 分组
		name_index = param.getRealLen();
		for (int i = context_list.count() - 1; i >= i_parent; i--){
			QString text = context_list.at(i).trimmed();
			if (text.contains(QRegExp(param.getParentCommentRe(name_index)))){
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
		int default_index = 0;
		for (int i = 0; i < config.getConfigLen(); i++){
			QString parent_str = "";
			QString param_str = param_str_list.join("\n");

			// > 分组
			if (param.isParentGrouping() && i % 20 == 0){
				QString default_str = "@default ";
				if (default_index < defaultData_list.count()){
					default_str.append(defaultData_list.at(default_index));
				}
				parent_str = parent_str_list.join("\n");
				parent_str = parent_str.replace(param.getParentName(1), param.getParentName(i + 1));
				parent_str = parent_str.replace(QRegExp("@default[^\n]*"), default_str);		//（@default值替换）
				if (default_index < parentData_list.count()){									//（@parent值替换）
					QString parentData_str = "@parent ";
					parentData_str.append(parentData_list.at(default_index));
					parent_str = parent_str.replace(QRegExp("@parent[^\n]*"), parentData_str);	
				}
				parent_str.append("\n * ");
				parent_str.append("\n");
				default_index++;
			}

			// > 变量
			QString default_str2 = "@default ";
			if (default_index < defaultData_list.count()){
				default_str2.append(defaultData_list.at(default_index));
			}
			param_str = param_str.replace(param.getParamName(1), param.getParamName(i + 1));
			param_str = param_str.replace(QRegExp("@default[^\n]*"), default_str2);		//（@default值替换）
			if (param.isParentGrouping()){												//（@parent值强制赋值）
				param_str = param_str.replace(param.getParentName(1), param.getParentName(i + 1));
			}
			param_str.append("\n * ");
			default_index++;

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
		获取 - 符合条件的指定行
*/
int S_LEConfigWriter::indexOfRow(QStringList contextList, QRegExp re){
	for (int i = 0; i < contextList.count(); i++){
		if (contextList.at(i).contains(re)){
			return i;
		}
	}
	return -1;
}
/*-------------------------------------------------
		获取 - 根据参数名 "阶段-1" 获取到一连串@参数列表
*/
QStringList S_LEConfigWriter::getParamGroup(QStringList contextList,QRegExp re){
	QStringList result_list = QStringList();
	bool finded = false;
	for (int i = 0; i < contextList.count(); i++){
		QString text =  contextList.at(i);
		if (finded == false){
			if (text.contains(re)){
				finded = true;
				result_list.append(text);
			}
		}
		else{	// finded == true
			if (text.contains("@") && !text.contains("@param")){
				result_list.append(text);
			}else{
				// > 结构寻找结束
				break;
			}
		}
	}
	return result_list;
}
/*-------------------------------------------------
		获取 - 获取指定位置的连续default数据
*/
QStringList S_LEConfigWriter::getParamList_default(QStringList contextList, int start_row, int len){
	return this->getParamList(contextList, "@default", start_row, len);
}
/*-------------------------------------------------
		获取 - 获取指定位置的连续parent数据
*/
QStringList S_LEConfigWriter::getParamList_parent(QStringList contextList, int start_row, int len){
	return this->getParamList(contextList, "@parent", start_row, len);
}
/*-------------------------------------------------
		获取 - 获取指定位置的连续数据
*/
QStringList S_LEConfigWriter::getParamList(QStringList contextList, QString param_name, int start_row, int len){
	if (start_row < 0){ start_row = 0; }
	QStringList result_list = QStringList();

	bool is_inParam = false;
	for (int i = start_row; i < contextList.count(); i++){
		QString text = contextList.at(i);
		if (text.contains("@param")){
			if (is_inParam == false ){	//（开始找 @default）
				is_inParam = true;
				continue;
			}else{						//（跳转到了新的数据，说明上一个数据没有 @default 注释）
				is_inParam = true;
				result_list.append("");
				continue;
			}
		}
		if (text.contains(param_name) && is_inParam == true ){	//（找到一行 @default）
			is_inParam = false;
			text = text.replace(param_name, "").trimmed();		//（可以为空字符串）
			if (text.isEmpty() == false && text.at(0) == '*' ){
				text = text.mid(1).trimmed();					//（去掉" * "内容）
			}
			result_list.append(text);

			// > 找到所有数据后，返回
			if (len >= 0 && result_list.count() >= len){
				return result_list;
			}
		}
	}
	return result_list;
}