#include "stdafx.h"
#include "s_LEConfigWriter.h"


/*
-----==========================================================-----
		�ࣺ		�ı�ɨ����.h
		����ģ�飺	����ģ��
		���ܣ�		�ṩɨ�衢��дֱ�ӹ��ܡ�

		Ŀ�꣺		ʶ��������ı��������޸ģ�
						* @Drill_LE_param "����-%d"
						* @Drill_LE_parentKey "---������%d��%d---"
						* @Drill_LE_var "DrillUp.g_SSpA_context_list_length"

		˵����		����ֻ�ṩ��Ҫ�������ܡ�
					�����������ԵĽṹ��
					���������ݴ洢��ֻ��һ��ִ�й��ߡ�

-----==========================================================-----
*/

S_LEConfigWriter::S_LEConfigWriter() : QObject(){
	init();
}
S_LEConfigWriter::~S_LEConfigWriter() {
}
/*  - - ���� - - */
S_LEConfigWriter* S_LEConfigWriter::cur_manager = NULL;
S_LEConfigWriter* S_LEConfigWriter::getInstance() {
	if (cur_manager == NULL) {
		cur_manager = new S_LEConfigWriter();
	}
	return cur_manager;
}
/*  - - ��ʼ�� - - */
void S_LEConfigWriter::init() {

}


/*-------------------------------------------------
		��д - ���ֵ�޸�
*/
QString S_LEConfigWriter::doOverwritePlugin(QString context, C_LEAnnotation_Param param, C_LEConfigData config){
	
	// > ���ֵ���
	if (config.getConfigLen() <= -1 ){
		QMessageBox::warning(nullptr, "����", "����Ҫ������" + config.getPluginName() + "�����ֵ��", QMessageBox::Yes);
		return "";
	}
	if (config.getConfigLen() <= 4){
		QMessageBox::warning(nullptr, "����", "���" + config.getPluginName() + "���������ֵ����С��4��", QMessageBox::Yes);
		return "";
	}
	if (param.getRealLen() < 4){		// ���ô���ֻ�ڵ������øú���ʱ����֣�
		QMessageBox::warning(nullptr, "����", "ʹ���޸�������ʱ������Ҫ֪���޸ĵ�Ŀ����ʵ�ʳ��ȡ�\n��ͨ����ȡ���ж������ȡ��", QMessageBox::Yes);
		return "";
	}
	// > ��ͬ���ֵʱ�����޸�
	if (config.getConfigLen() == param.getRealLen()){ return context; }
	QStringList context_list = context.split(QRegExp("(\n\r)|(\n)|(\r\n)"));

	//-----------------------------
	// >> �������������
	if (!param.isParentGrouping()){

		// > �ҵ�������ʼ��
		int i_param = -1;
		for (int i = 0; i < context_list.count(); i++){
			QString text = context_list.at(i);
			if (text.contains(QRegExp(param.getParamCommentRe(1)))){	//����ȫ��ƥ�䣨"@param SV����-1" ����ƥ�� "����-%d" ��
				i_param = i;
				break;
			}
		}

		// > ���ش�����
		if (i_param == -1){
			QMessageBox::warning(nullptr, "����", "���" + config.getPluginName() + "ת��ʱ���������ش��󣬸ô����ƿ��˱༭���ļ�����\n�뽫�ô���ľ��������֪��drill_up��", QMessageBox::Yes);
			return "";
		}

		// > �ҵ���һ�������ṹ
		QRegExp param_re = param.getParamCommentRe(1);
		QStringList param_str_list = this->getParamGroup(context_list, param_re);

		// > ��ȡ�ñ����µ�"@default"�����б�
		int start_row = this->indexOfRow(context_list, param_re);
		int real_len = param.getRealLen();
		QStringList defaultData_list = this->getParamList_default(context_list, start_row, real_len);

		// > �������� ɾ�� ��
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

		// > ��������
		for (int i = 0; i < config.getConfigLen(); i++){
			QString param_str = param_str_list.join("\n");
			QString default_str = "@default ";
			if (i < defaultData_list.count()){
				default_str.append(defaultData_list.at(i));
			}
			param_str = param_str.replace(param.getParamName(1), param.getParamName(i + 1));
			param_str = param_str.replace(QRegExp("@default[^\n]*"), default_str);		//��@defaultֵ�滻��
			param_str.append("\n * ");
			context_list.insert(i_param + i, param_str);
		}


	//-----------------------------
	// >> �����������
	}else{

		// > �ҵ�������ʼ��
		int i_param = 0;
		for (int i = 0; i < context_list.count(); i++){
			QString text = context_list.at(i);
			if (text.contains(QRegExp(param.getParamCommentRe(1)))){	//����ȫ��ƥ�䣨"@param SV����-1" ����ƥ�� "����-%d" ��
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

		// > ���ش�����
		if (i_param == -1 || i_parent == -1){
			QMessageBox::warning(nullptr, "����", "���" + config.getPluginName() + "ת��ʱ���������ش��󣬸ô����ƿ��˱༭���ļ�����\n�뽫�ô���ľ��������֪��drill_up��", QMessageBox::Yes);
			return "";
		}
		
		// > �ҵ���һ�������ṹ
		QRegExp param_re = param.getParamCommentRe(1);
		QRegExp parent_re = param.getParentCommentRe(1);
		QStringList param_str_list = this->getParamGroup(context_list, param_re);
		QStringList parent_str_list = this->getParamGroup(context_list, parent_re);

		// > ��ȡ�ñ����µ�"@default"��"@parent"�����б����� ���� �����ݣ�
		int start_row = this->indexOfRow(context_list, parent_re);
		int real_len = param.getRealLen() + qFloor(param.getRealLen()/20);	//������ �кͷ��飩
		QStringList defaultData_list = this->getParamList_default(context_list, start_row, real_len);
		QStringList parentData_list = this->getParamList_parent(context_list, start_row, real_len);

		// > ��������ɾ�� ��
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
		// > ��������ɾ�� ����
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


		// > ��������
		int default_index = 0;
		for (int i = 0; i < config.getConfigLen(); i++){
			QString parent_str = "";
			QString param_str = param_str_list.join("\n");

			// > ����
			if (param.isParentGrouping() && i % 20 == 0){
				QString default_str = "@default ";
				if (default_index < defaultData_list.count()){
					default_str.append(defaultData_list.at(default_index));
				}
				parent_str = parent_str_list.join("\n");
				parent_str = parent_str.replace(param.getParentName(1), param.getParentName(i + 1));
				parent_str = parent_str.replace(QRegExp("@default[^\n]*"), default_str);		//��@defaultֵ�滻��
				if (default_index < parentData_list.count()){									//��@parentֵ�滻��
					QString parentData_str = "@parent ";
					parentData_str.append(parentData_list.at(default_index));
					parent_str = parent_str.replace(QRegExp("@parent[^\n]*"), parentData_str);	
				}
				parent_str.append("\n * ");
				parent_str.append("\n");
				default_index++;
			}

			// > ����
			QString default_str2 = "@default ";
			if (default_index < defaultData_list.count()){
				default_str2.append(defaultData_list.at(default_index));
			}
			param_str = param_str.replace(param.getParamName(1), param.getParamName(i + 1));
			param_str = param_str.replace(QRegExp("@default[^\n]*"), default_str2);		//��@defaultֵ�滻��
			if (param.isParentGrouping()){												//��@parentֵǿ�Ƹ�ֵ��
				param_str = param_str.replace(param.getParentName(1), param.getParentName(i + 1));
			}
			param_str.append("\n * ");
			default_index++;

			context_list.insert(i_param + i, parent_str + param_str);
		}
	}

	// > ���̶����Ǻ�
	QString result = context_list.join("\n");
	result = result.replace(QRegExp("([ \t\n\r]+\\*){5,}"),"\n * \n *");
	
	// > �滻
	QString var_str = param.getVarName();
	if (var_str != "" && var_str != "null"){
		QString re_str = var_str.replace(".", "\\.");		//��ֹ��ʶ������
		QRegExp re = QRegExp(re_str + "[ ]*=[^=]*\n");
		QString var_defineString = param.getVarName() + " = " + QString::number(config.getConfigLen()) + ";\n";
		result = result.replace(re, var_defineString);
	}

	return result;
}

/*-------------------------------------------------
		��ȡ - ����������ָ����
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
		��ȡ - ���ݲ����� "�׶�-1" ��ȡ��һ����@�����б�
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
				// > �ṹѰ�ҽ���
				break;
			}
		}
	}
	return result_list;
}
/*-------------------------------------------------
		��ȡ - ��ȡָ��λ�õ�����default����
*/
QStringList S_LEConfigWriter::getParamList_default(QStringList contextList, int start_row, int len){
	return this->getParamList(contextList, "@default", start_row, len);
}
/*-------------------------------------------------
		��ȡ - ��ȡָ��λ�õ�����parent����
*/
QStringList S_LEConfigWriter::getParamList_parent(QStringList contextList, int start_row, int len){
	return this->getParamList(contextList, "@parent", start_row, len);
}
/*-------------------------------------------------
		��ȡ - ��ȡָ��λ�õ���������
*/
QStringList S_LEConfigWriter::getParamList(QStringList contextList, QString param_name, int start_row, int len){
	if (start_row < 0){ start_row = 0; }
	QStringList result_list = QStringList();

	bool is_inParam = false;
	for (int i = start_row; i < contextList.count(); i++){
		QString text = contextList.at(i);
		if (text.contains("@param")){
			if (is_inParam == false ){	//����ʼ�� @default��
				is_inParam = true;
				continue;
			}else{						//����ת�����µ����ݣ�˵����һ������û�� @default ע�ͣ�
				is_inParam = true;
				result_list.append("");
				continue;
			}
		}
		if (text.contains(param_name) && is_inParam == true ){	//���ҵ�һ�� @default��
			is_inParam = false;
			text = text.replace(param_name, "").trimmed();		//������Ϊ���ַ�����
			if (text.isEmpty() == false && text.at(0) == '*' ){
				text = text.mid(1).trimmed();					//��ȥ��" * "���ݣ�
			}
			result_list.append(text);

			// > �ҵ��������ݺ󣬷���
			if (len >= 0 && result_list.count() >= len){
				return result_list;
			}
		}
	}
	return result_list;
}