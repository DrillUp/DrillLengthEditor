#include "stdafx.h"

#include "c_FastClass.h"
#include "../../common/TTool.h"


/*
-----==========================================================-----
		类：		快速类定义.cpp
		版本：		v1.07
		作者：		drill_up
		所属模块：	工具模块
		功能：		定义一个表单填写用的类定义，快速表单、快速窗口通用。
					【这里类似于定义model，所有字段都是字符串】
					【类名、参数名，用中文】
					
		功能：		1.设置：	类名、Double、Int、QString、QString（下拉框）
					2.二设：	英文名、显示名、必填项、后缀
					3.三设：	SI写法、不可编辑、默认值修改
					4.高级：	所在列数、类嵌套
					特殊：		QJsonObject填充到table中、QJsonObject中英文转换

		内置约束：	1.中文名和英文名不能重复，程序会检查重复，并报错。（包括类名）
					2.嵌套类 里面至少要有一个参数，不然程序会检查报错。

		自动规则：	1.中文参数键值全部为QString。
					  英文参数会根据类型自动转double和int。
					2.后缀为"Hz"时，自动SI写法。
					3.类型为double时，显示tablewidget时自动添加后缀。
					4.标识格式："<-标识字符->参数名"。
					  中文参数名中该格式的字符串不会被显示出来。（也可以用showingName设置）
					
		使用方法：
				>创建
					c_class = C_FastClass("目标模型");						//创建相当于直接填写一堆参数集，作用于快速表单、快速窗口
					c_class.addDouble				("位置", "0.0");		//详细见 快速表单、快速窗口 的使用方法
					c_class.addDouble				("方向", "0.0"); 
					c_class.addQString				("说明", "0.0");
					c_class.addQString				("描述", "0.0");
				>参数修改
					c_class.addQString				("脉冲宽度", "0");		//添加中文参数后，可以依次添加各个属性
					c_class.setParamEnglishName		("脉冲宽度", "ModPulMaxInterfPulseWidth");
					c_class.setParamShowingName		("脉冲宽度", "最大干扰脉冲宽度");
					c_class.setParamSuffix			("脉冲宽度", "μs");
				>类嵌套
					c_class = C_FastClass("脉冲");							//嵌套后，你仍然需要确保中文参数名不重复
						C_FastClass class_6 = C_FastClass("信号范围");
						class_6.addQString						("信号下限", "0");
						class_6.setParamEnglishName				("信号下限", "ModPulSignalCarrierFreqStart");
						class_6.setParamSuffix					("信号下限", "");
						class_6.addQString						("信号上限", "0");
						class_6.setParamEnglishName				("信号上限", "ModPulSignalCarrierFreqEnd");
						class_6.setParamSuffix					("信号上限", "Hz");
					c_class.addFastClassOneLine				(class_6);
				>特殊
					this->m_target_class.addQString("<-SPAN->xxxxx", "");	//含有指定前缀的变量，将会成为无意义的空占位变量

-----==========================================================-----
*/
C_FastClass::C_FastClass(){
	this->classNameChinese = "";
	this->classNameEnglish = "";
	this->paramList = QList<C_FastClassParam>();
	this->nameChinese_list = QStringList() << this->classNameChinese;
	this->nameEnglish_list = QStringList() << this->classNameEnglish;
}
C_FastClass::C_FastClass(QString className)
{
	this->classNameChinese = className;
	this->classNameEnglish = "";
	this->paramList = QList<C_FastClassParam>();
	this->nameChinese_list = QStringList() << this->classNameChinese;
	this->nameEnglish_list = QStringList() << this->classNameEnglish;
}

C_FastClass::~C_FastClass(){
}

/*-------------------------------------------------
		私有 - 根据类名获取到类
*/
C_FastClassParam C_FastClass::getParamByName(QString nameChinese){
	for (int i = 0; i < this->paramList.count(); i++){
		C_FastClassParam c_fp = this->paramList.at(i);
		if (c_fp.nameChinese == nameChinese){
			return c_fp;
		}
	}
	return C_FastClassParam();
}
int C_FastClass::getIndexByName(QString nameChinese){
	for (int i = 0; i < this->paramList.count(); i++){
		C_FastClassParam c_fp = this->paramList.at(i);
		if (c_fp.nameChinese == nameChinese){
			return i;
		}
	}
	return -1;
}

/*-------------------------------------------------
		私有 - 判断重名
*/
void C_FastClass::checkDuplicatedChineseValue(QString nameChinese){
	if (nameChinese == ""){ return; }
	#ifdef DEBUG_OPEN_PRO
		if (nameChinese_list.indexOf(nameChinese) != -1){
			qDebug() << "[" + this->classNameChinese + "]发现重名中文参数：" + nameChinese;
			Q_ASSERT(false);
		}
	#else
	
	#endif
	nameChinese_list.push_back(nameChinese);
}
void C_FastClass::checkDuplicatedEnglishValue(QString nameEnglish){
	if (nameEnglish == ""){ return; }
	#ifdef DEBUG_OPEN_PRO
		if (nameEnglish_list.indexOf(nameEnglish) != -1){
			qDebug() << "[" + this->classNameChinese + "]发现重名英文参数：" + nameEnglish;
			Q_ASSERT(false);
		}
	#else
	
	#endif
	nameEnglish_list.push_back(nameEnglish);
}

/*-------------------------------------------------
		设置 - 类名
*/
void C_FastClass::setClassName(QString className){
	this->nameChinese_list.removeOne(this->classNameChinese);
	this->classNameChinese = className;
	this->checkDuplicatedChineseValue(className);
}
void C_FastClass::setClassNameEnglish(QString className){
	this->nameEnglish_list.removeOne(this->classNameEnglish);
	this->classNameEnglish = className;
	this->checkDuplicatedEnglishValue(className);
}
void C_FastClass::setClassNameShowing(QString className){
	this->classNameShowing = className;
}
/*-------------------------------------------------
		设置 - double
*/
void C_FastClass::addDouble(QString paramName){
	C_FastClassParam c_f = C_FastClassParam();
	c_f.nameChinese = paramName;
	c_f.type = "Double";
	c_f.widgetType = "QLineEdit";
	this->checkDuplicatedChineseValue(paramName);
	this->paramList.push_back(c_f);
}
void C_FastClass::addDouble(QString paramName, QString defaultValue){
	C_FastClassParam c_f = C_FastClassParam();
	c_f.nameChinese = paramName;
	c_f.type = "Double";
	c_f.widgetType = "QLineEdit";
	c_f.defaultValue = defaultValue;
	this->checkDuplicatedChineseValue(paramName);
	this->paramList.push_back(c_f);
}
/*-------------------------------------------------
		设置 - Int
*/
void C_FastClass::addInt(QString paramName){
	C_FastClassParam c_f = C_FastClassParam();
	c_f.nameChinese = paramName;
	c_f.type = "Int";
	c_f.widgetType = "QLineEdit";
	this->checkDuplicatedChineseValue(paramName);
	this->paramList.push_back(c_f);
}
void C_FastClass::addInt(QString paramName, QString defaultValue){
	C_FastClassParam c_f = C_FastClassParam();
	c_f.nameChinese = paramName;
	c_f.type = "Int";
	c_f.widgetType = "QLineEdit";
	c_f.defaultValue = defaultValue;
	this->checkDuplicatedChineseValue(paramName);
	this->paramList.push_back(c_f);
}
/*-------------------------------------------------
		设置 - QString
*/
void C_FastClass::addQString(QString paramName){
	C_FastClassParam c_f = C_FastClassParam();
	c_f.nameChinese = paramName;
	c_f.type = "QString";
	c_f.widgetType = "QLineEdit";
	this->checkDuplicatedChineseValue(paramName);
	this->paramList.push_back(c_f);
}
void C_FastClass::addQString(QString paramName, QString defaultValue){
	C_FastClassParam c_f = C_FastClassParam();
	c_f.nameChinese = paramName;
	c_f.type = "QString";
	c_f.widgetType = "QLineEdit";
	c_f.defaultValue = defaultValue;
	this->checkDuplicatedChineseValue(paramName);
	this->paramList.push_back(c_f);
}
/*-------------------------------------------------
		设置 - QString（含选项）
*/
void C_FastClass::addQStringWithChoose(QString paramName, QString defaultValue, QStringList chooseList){
	C_FastClassParam c_f = C_FastClassParam();
	c_f.nameChinese = paramName;
	c_f.type = "QString";
	c_f.widgetType = "QComboBox";
	c_f.chooseList = chooseList;
	c_f.defaultValue = defaultValue;
	this->checkDuplicatedChineseValue(paramName);
	this->paramList.push_back(c_f);
}

/*-------------------------------------------------
		高级 - 快速类嵌套
*/
void C_FastClass::addFastClassQWidget(C_FastClass fastClass){
	C_FastClassParam c_f = C_FastClassParam();
	c_f.nameChinese = fastClass.getClassNameChinese();
	c_f.nameEnglish = fastClass.getClassNameEnglish();
	c_f.type = "FastClass";
	c_f.widgetType = "QWidget";
	c_f.fastClass = fastClass;

	// > 检查类参数重名
	QStringList name_list = fastClass.getParamChineseNameList();
	Q_ASSERT(name_list.count() > 0);
	for (int i = 0; i < name_list.count(); i++){
		this->checkDuplicatedChineseValue(name_list.at(i));
	}
	QStringList eng_name_list = fastClass.getParamEnglishNameList();
	for (int i = 0; i < eng_name_list.count(); i++){
		this->checkDuplicatedEnglishValue(eng_name_list.at(i));
	}

	this->paramList.push_back(c_f);
}
void C_FastClass::addFastClassOneLine(C_FastClass fastClass){
	C_FastClassParam c_f = C_FastClassParam();
	c_f.nameChinese = fastClass.getClassNameChinese();
	c_f.nameEnglish = fastClass.getClassNameEnglish();
	c_f.type = "FastClass";
	c_f.widgetType = "OneLine";
	c_f.fastClass = fastClass;

	// > 检查类参数重名
	QStringList name_list = fastClass.getParamChineseNameList();
	Q_ASSERT(name_list.count() > 0);
	for (int i = 0; i < name_list.count(); i++){
		this->checkDuplicatedChineseValue(name_list.at(i));
	}
	QStringList eng_name_list = fastClass.getParamEnglishNameList();
	for (int i = 0; i < eng_name_list.count(); i++){
		this->checkDuplicatedEnglishValue(eng_name_list.at(i));
	}

	this->paramList.push_back(c_f);
}
void C_FastClass::addFastClassQGroupBox(C_FastClass fastClass){
	C_FastClassParam c_f = C_FastClassParam();
	c_f.nameChinese = fastClass.getClassNameChinese();
	c_f.nameEnglish = fastClass.getClassNameEnglish();
	c_f.type = "FastClass";
	c_f.widgetType = "QGroupBox";
	c_f.fastClass = fastClass;

	// > 检查类参数重名
	QStringList name_list = fastClass.getParamChineseNameList();
	Q_ASSERT(name_list.count() > 0);
	for (int i = 0; i < name_list.count(); i++){
		this->checkDuplicatedChineseValue(name_list.at(i));
	}
	QStringList eng_name_list = fastClass.getParamEnglishNameList();
	for (int i = 0; i < eng_name_list.count(); i++){
		this->checkDuplicatedEnglishValue(eng_name_list.at(i));
	}

	this->paramList.push_back(c_f);
}

/*-------------------------------------------------
		二设 - 英文名
*/
void C_FastClass::setParamEnglishName(QString paramNameChinese, QString paramNameEnglish){
	C_FastClassParam c_fp = this->getParamByName(paramNameChinese);
	c_fp.nameEnglish = paramNameEnglish;
	this->checkDuplicatedEnglishValue(paramNameEnglish);		//检查英文名重复
	this->paramList.replace(this->getIndexByName(paramNameChinese), c_fp);
}
/*-------------------------------------------------
		二设 - 显示名
*/
void C_FastClass::setParamShowingName(QString paramNameChinese, QString showingName){
	C_FastClassParam c_fp = this->getParamByName(paramNameChinese);
	c_fp.nameShowing = showingName;
	this->paramList.replace(this->getIndexByName(paramNameChinese), c_fp);
}
/*-------------------------------------------------
		二设 - 必填项
*/
void C_FastClass::setParamNotNull(QString paramNameChinese, bool notNull){
	C_FastClassParam c_fp = this->getParamByName(paramNameChinese);
	c_fp.notNull = notNull;
	this->paramList.replace(this->getIndexByName(paramNameChinese), c_fp);
}
/*-------------------------------------------------
		二设 - 后缀
*/
void C_FastClass::setParamSuffix(QString paramNameChinese, QString suffix){
	C_FastClassParam c_fp = this->getParamByName(paramNameChinese);
	c_fp.suffix = suffix;
	if (suffix == "Hz"){ c_fp.isSI = true; }	//自动视为si写法
	this->paramList.replace(this->getIndexByName(paramNameChinese), c_fp);
}
/*-------------------------------------------------
		三设 - SI写法
*/
void C_FastClass::setParamSI(QString paramNameChinese, bool isSI){
	C_FastClassParam c_fp = this->getParamByName(paramNameChinese);
	if (c_fp.isNull()){ return; }
	c_fp.isSI = isSI;
	this->paramList.replace(this->getIndexByName(paramNameChinese), c_fp);
}
/*-------------------------------------------------
		三设 - SI写法
*/
void C_FastClass::setParamEditable(QString paramNameChinese, bool isEditable){
	C_FastClassParam c_fp = this->getParamByName(paramNameChinese);
	if (c_fp.isNull()){ return; }
	c_fp.isEditable = isEditable;
	this->paramList.replace(this->getIndexByName(paramNameChinese), c_fp);
}
/*-------------------------------------------------
		三设 - 默认值修改
*/
void C_FastClass::setParamDefaultValue(QString paramNameChinese, QString defaultValue){
	C_FastClassParam c_fp = this->getParamByName(paramNameChinese);
	if (c_fp.isNull()){ return; }
	c_fp.defaultValue = defaultValue;
	this->paramList.replace(this->getIndexByName(paramNameChinese), c_fp);
}

/*-------------------------------------------------
		高级 - 所在列数
*/
void C_FastClass::setParamColumn(QString paramNameChinese, int col){
	C_FastClassParam c_fp = this->getParamByName(paramNameChinese);
	c_fp.columnPos = col;
	this->paramList.replace(this->getIndexByName(paramNameChinese), c_fp);
}

/*-------------------------------------------------
		获取 - 类名
*/
QString C_FastClass::getClassNameChinese(){
	return classNameChinese;
}
QString C_FastClass::getClassNameEnglish(){
	return classNameEnglish;
}
QString C_FastClass::getClassNameShowing(){
	if (this->classNameShowing != ""){ return classNameShowing; }
	return this->classNameChinese.replace(QRegExp("<-[^>]*->"), "");
}
/*-------------------------------------------------
		获取 - 全部中文名/英文名
*/
QStringList C_FastClass::getParamChineseNameList(){
	return nameChinese_list;
}
QStringList C_FastClass::getParamEnglishNameList(){
	return nameEnglish_list;
}
/*-------------------------------------------------
		获取 - 快速类参数（含FastClass）
*/
QList<C_FastClassParam> C_FastClass::getParamList(){
	return paramList;
}
/*-------------------------------------------------
		获取 - 快速类参数（FastClass全部被摊开）
*/
QList<C_FastClassParam> C_FastClass::getConvertedParamList(){
	QList<C_FastClassParam> result_list = QList<C_FastClassParam>();
	for (int i = 0; i < this->paramList.count(); i++){
		C_FastClassParam c_fp = this->paramList.at(i);
		if (c_fp.type == "FastClass"){
			result_list.append(c_fp.fastClass.getConvertedParamList());
		}else{
			result_list.push_back(c_fp);
		}
	}
	return result_list;
}
/*-------------------------------------------------
		获取 - 子快速类
*/
C_FastClass C_FastClass::getChildClass(QString chineseName){
	C_FastClassParam c_fp = this->getParamByName(chineseName);
	if (c_fp.type == "FastClass"){
		return c_fp.fastClass;
	}
	return C_FastClass();
}
/*-------------------------------------------------
		获取 - 判断空
*/
bool C_FastClass::isNull(){
	if (this->classNameChinese == ""){ return true; }
	return false;
}

/*-------------------------------------------------
		特殊功能 - 将QJsonObject的数据，根据该类的格式，填充到table中
*/
void C_FastClass::fillDataInTableWidget(QTableWidget* tableWidget, QList<QJsonObject> data_list){
	QList<C_FastClassParam> temp_cols = this->getConvertedParamList();
	QList<QJsonObject> temp_rows = data_list;
	tableWidget->setColumnCount(temp_cols.size());
	tableWidget->setRowCount(temp_rows.size());

	// > 表头
	QStringList col_names = QStringList();
	for (int i = 0; i < temp_cols.size(); i++) {
		C_FastClassParam c_fp = temp_cols.at(i);
		col_names.append(c_fp.getShowingName());
	}
	tableWidget->setHorizontalHeaderLabels(col_names);

	// > 表数据
	for (int i = 0; i < temp_rows.size(); i++) {
		QJsonObject obj = temp_rows.at(i);
		for (int j = 0; j < temp_cols.size(); j++) {
			C_FastClassParam c_fp = temp_cols.at(j);
			QString data = "";		// > 中文参数，直接显示 
			data = obj.value(c_fp.nameChinese).toString();
			if (c_fp.isSI){			// > SI转换
				data = TTool::_to_QString_bySI_(data);
			}
			if (c_fp.type == "Double"){	// > double自动带后缀（单位）
				data += c_fp.suffix;
			}
			tableWidget->setItem(i, j, new QTableWidgetItem(data));
		}
	}
}
/*-------------------------------------------------
		特殊功能 - 将QJsonObject的数据，根据该类的格式，填充到table中
*/
void C_FastClass::fillDataInTableWidgetEnglish(QTableWidget* tableWidget, QList<QJsonObject> data_list){
	QList<C_FastClassParam> temp_cols = this->getConvertedParamList();
	QList<QJsonObject> temp_rows = data_list;
	tableWidget->setColumnCount(temp_cols.size());
	tableWidget->setRowCount(temp_rows.size());

	// > 表头
	QStringList col_names = QStringList();
	for (int i = 0; i < temp_cols.size(); i++) {
		C_FastClassParam c_fp = temp_cols.at(i);
		col_names.append(c_fp.getShowingName());
	}
	tableWidget->setHorizontalHeaderLabels(col_names);

	// > 表数据
	for (int i = 0; i < temp_rows.size(); i++) {
		QJsonObject obj = temp_rows.at(i);
		for (int j = 0; j < temp_cols.size(); j++) {
			C_FastClassParam c_fp = temp_cols.at(j);
			QString data = "";		// > 英文参数，double -> QString 显示 
			if (obj.value(c_fp.nameEnglish).isDouble()){
				data =  QString::number(obj.value(c_fp.nameEnglish).toDouble());
			}else{
				data = obj.value(c_fp.nameEnglish).toString();
			}
			if (c_fp.isSI){			// > SI转换
				data = TTool::_to_QString_bySI_(data);
			}
			if (c_fp.type == "Double"){	// > double自动带后缀（单位）
				data += c_fp.suffix;
			}
			tableWidget->setItem(i, j, new QTableWidgetItem(data));
		}
	}
}
/*-------------------------------------------------
		特殊功能 - jsonobj 英文 -> 中文
*/
QJsonObject C_FastClass::convertEnglishToChinese(QJsonObject data){
	QList<C_FastClassParam> param_list = this->getConvertedParamList();
	QJsonObject result_obj = QJsonObject();
	QStringList keys = data.keys();
	for (int i = 0; i < keys.length(); i++){
		QString key = keys.at(i);
		QJsonValue value = data.value(key);

		bool inserted = false;
		for (int j = 0; j < param_list.length(); j++){
			C_FastClassParam c_p = param_list.at(j);
			if (c_p.nameEnglish == key){

				// > 中文参数，double -> QString（中文全部QString） 
				if (value.isDouble()){
					result_obj.insert(c_p.nameChinese, QString::number(value.toDouble()));
				}
				else{
					result_obj.insert(c_p.nameChinese, value);
				}
				inserted = true;
			}
		}
		if (inserted == false){
			result_obj.insert(key, value);
		}
	}
	return result_obj;
}
/*-------------------------------------------------
		特殊功能 - jsonobj 英文 -> 中文（转不了的扔掉）
*/
QJsonObject C_FastClass::convertEnglishToChineseExcludely(QJsonObject data){
	QList<C_FastClassParam> param_list = this->getConvertedParamList();
	QJsonObject result_obj = QJsonObject();
	QStringList keys = data.keys();
	for (int i = 0; i < keys.length(); i++){
		QString key = keys.at(i);
		QJsonValue value = data.value(key);

		for (int j = 0; j < param_list.length(); j++){
			C_FastClassParam c_p = param_list.at(j);
			if (c_p.nameEnglish == key){

				// > 中文参数，double -> QString（中文全部QString） 
				if (value.isDouble()){
					result_obj.insert(c_p.nameChinese, QString::number(value.toDouble()));
				}
				else{
					result_obj.insert(c_p.nameChinese, value);
				}
			}
		}
	}
	return result_obj;
}
/*-------------------------------------------------
		特殊功能 - jsonobj 中文 -> 英文
*/
QJsonObject C_FastClass::convertChineseToEnglish(QJsonObject data){
	QList<C_FastClassParam> param_list = this->getConvertedParamList();
	QJsonObject result_obj = QJsonObject();
	QStringList keys = data.keys();
	for (int i = 0; i < keys.length(); i++){
		QString key = keys.at(i);
		QJsonValue value = data.value(key);
		if (key.contains("<-SPAN->")){ continue; }		//空占位符变量

		bool inserted = false;
		for (int j = 0; j < param_list.length(); j++){
			C_FastClassParam c_p = param_list.at(j);
			if (c_p.nameChinese == key){

				// > 英文参数， QString -> double
				if (value.isString() && c_p.type == "Double"){
					result_obj.insert(c_p.nameEnglish, value.toString().toDouble());
				}
				else if (value.isString() && c_p.type == "Int"){
					result_obj.insert(c_p.nameEnglish, value.toString().toInt());
				}
				else{
					result_obj.insert(c_p.nameEnglish, value);
				}
				inserted = true;
			}
		}
		if (inserted == false){
			result_obj.insert(key, value);
		}
	}
	return result_obj;
}
/*-------------------------------------------------
		特殊功能 - jsonobj 中文 -> 英文（转不了的扔掉）
*/
QJsonObject C_FastClass::convertChineseToEnglishExcludely(QJsonObject data){
	QList<C_FastClassParam> param_list = this->getConvertedParamList();
	QJsonObject result_obj = QJsonObject();
	QStringList keys = data.keys();
	for (int i = 0; i < keys.length(); i++){
		QString key = keys.at(i);
		QJsonValue value = data.value(key);
		if (key.contains("<-SPAN->")){ continue; }		//空占位符变量

		for (int j = 0; j < param_list.length(); j++){
			C_FastClassParam c_p = param_list.at(j);
			if (c_p.nameChinese == key){

				// > 英文参数， QString -> double
				if (value.isString() && c_p.type == "Double"){
					result_obj.insert(c_p.nameEnglish, value.toString().toDouble());
				}
				else if (value.isString() && c_p.type == "Int"){
					result_obj.insert(c_p.nameEnglish, value.toString().toInt());
				}
				else{
					result_obj.insert(c_p.nameEnglish, value);
				}
			}
		}
	}
	return result_obj;
}



/*
-----==========================================================-----
		类：		快速类参数.h
-----==========================================================-----
*/
C_FastClassParam::C_FastClassParam()
{
	this->nameChinese = "";
	this->nameEnglish = "";
	this->type = "";
	this->widgetType = "";
	this->defaultValue = "";
	this->chooseList = QStringList();

	this->notNull = false;
	this->suffix = "";
	this->isSI = false;
	this->isEditable = true;
	//this->fastClass;

	this->columnPos = 0;
	this->nameShowing = "";

	this->tempA = "";
	this->tempB = "";
	this->tempC = "";
	this->tempD = "";
	this->tempE = "";
	this->tempF = "";
}
C_FastClassParam::~C_FastClassParam(){
}

/*-------------------------------------------------
		运算符重载
*/
const bool C_FastClassParam::operator== (const C_FastClassParam& a)const {
	return this->nameChinese == a.nameChinese;
}
/*-------------------------------------------------
		判断空
*/
bool C_FastClassParam::isNull(){
	if (this->nameChinese == ""){ return true; }
	if (this->type == ""){ return true; }
	return false;
}
/*-------------------------------------------------
		获取 - 显示文本名
*/
QString C_FastClassParam::getShowingName(){
	if (this->nameShowing != ""){ return nameShowing; }
	return removeSpecialChineseCharacter(this->nameChinese);
}
/*-------------------------------------------------
		获取 - 去掉中文名的标识用字符
*/
QString C_FastClassParam::removeSpecialChineseCharacter(QString nameChinese){
	return nameChinese.replace(QRegExp("<-[^>]*->"), "");
}