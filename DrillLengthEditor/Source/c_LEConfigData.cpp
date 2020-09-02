#include "stdafx.h"

#include "c_LEConfigData.h"
#include "Utils/common/TTool.h"

/*
-----==========================================================-----
		类：		插件参数.cpp（只读）
-----==========================================================-----
*/
C_LEPluginParam::C_LEPluginParam(){
	this->paramKey = "";
	this->parentKey = "";
	this->var = "";
	this->varLen = 0;
	this->realLen = 0;
}
C_LEPluginParam::~C_LEPluginParam(){
}

/*-------------------------------------------------
		参数 - 初始化
*/
void C_LEPluginParam::initParam(QString param, QString parentKey, QString var){
	this->paramKey = param;
	this->parentKey = parentKey;
	this->var = var;
}
/*-------------------------------------------------
		获取 - 变量键
*/
QString C_LEPluginParam::getParamKey(){
	return this->paramKey;
}
/*-------------------------------------------------
		获取 - 变量组键
*/
QString C_LEPluginParam::getParentKey(){
	return this->parentKey;
}
/*-------------------------------------------------
		获取 - 定义名
*/
QString C_LEPluginParam::getVarName(){
	return this->var;
}
/*-------------------------------------------------
		获取 - 定义最大值
*/
void C_LEPluginParam::setVarLen(int len){
	this->varLen = len;
}
int C_LEPluginParam::getVarLen(){
	return varLen;
}
/*-------------------------------------------------
		获取 - 实际最大值
*/
void C_LEPluginParam::setRealLen(int len){
	this->realLen = len;
}
int C_LEPluginParam::getRealLen(){
	return realLen;
}
/*-------------------------------------------------
		获取 - 变量显示名
*/
QString C_LEPluginParam::getParamShowingName(){
	return this->paramKey.split("-").at(0);
}
/*-------------------------------------------------
		获取 - 变量名（从1开始计数）
*/
QString C_LEPluginParam::getParamName(int index){
	QString i_str = QString::number(index);
	QString result = this->paramKey;
	result = result.replace("%d", i_str);
	return result;
}
/*-------------------------------------------------
		获取 - 变量组名（从1开始计数）
*/
QString C_LEPluginParam::getParentName(int index){
	if (this->isParentGrouping() == false){ return ""; }
	
	index--;
	int i = floor(index / 20);
	int start = 20 * i + 1;
	int end = 20 * (i + 1);
	QString str_start = QString::number(start);
	if (start == 1){ str_start = " 1"; };
	QString result = this->getParentKey();
	result.replace(result.lastIndexOf("%d"), 2, QString::number(end));
	result.replace("%d", str_start);

	return result;
}
/*-------------------------------------------------
		获取 - 是否分组
*/
bool C_LEPluginParam::isParentGrouping(){
	return this->parentKey.contains("%d");
}

/*
-----==========================================================-----
		类：		插件信息.cpp（只读）
-----==========================================================-----
*/
C_LEPlugin::C_LEPlugin(){
	this->pluginName = "";
	this->pluginDesc = "";
	this->pluginAuthor = "";
	this->paramList = QList<C_LEPluginParam>();
	this->paramForbidden = false;
	this->context = "";
	this->message = "";
	this->editMessage = "";
}
C_LEPlugin::~C_LEPlugin(){
}
C_LEPluginParam C_LEPlugin::getParamByKey(QString paramKey){
	for (int i = 0; i < this->paramList.count(); i++){
		C_LEPluginParam p = this->paramList.at(i);
		if (p.getParamKey() == paramKey){
			return p;
		}
	}
	return C_LEPluginParam();
}
/*-------------------------------------------------
		判断 - 空判断
*/
bool C_LEPlugin::isNull(){
	return this->pluginName == "" || this->pluginDesc == "";
}
/*-------------------------------------------------
		判断 - 是否被禁用
*/
bool C_LEPlugin::isForbidden(){
	return this->paramForbidden;
}
/*-------------------------------------------------
		判断 - 是否可编辑
*/
bool C_LEPlugin::isEditable(){
	if (this->paramList.count() == 0){ return false; }
	return true;
}
/*-------------------------------------------------
		判断 - 完整性检查
*/
bool C_LEPlugin::isIntegrity(){
	for (int i = 0; i < this->paramList.count(); i++){
		C_LEPluginParam pluginParam = this->paramList.at(i);

		if (pluginParam.getRealLen() == 0){ return false; }
		if (pluginParam.getVarName() == "null"){ continue; }
		if (pluginParam.getVarLen() == 0){ return false; }

		// > 对比
		if (pluginParam.getRealLen() != pluginParam.getVarLen()){
			return false;
		}
	}
	return true;
}


/*
-----==========================================================-----
		类：		配置参数.cpp（存储）
-----==========================================================-----
*/
C_LEConfigData::C_LEConfigData(){
	this->pluginName = "";
	this->paramKey = "";
	this->configLen = -1;
}
C_LEConfigData::~C_LEConfigData(){
}

/*-------------------------------------------------
		参数 - 初始化
*/
void C_LEConfigData::initParam(QString pluginName, QString paramKey, int realLen){
	this->pluginName = pluginName;
	this->paramKey = paramKey;
	this->configLen = realLen;
}
/*-------------------------------------------------
		获取 - 插件名（英文）
*/
QString C_LEConfigData::getPluginName(){
	return this->pluginName;
}
/*-------------------------------------------------
		获取 - 变量键 "阶段-%d"
*/
QString C_LEConfigData::getParamKey(){
	return this->paramKey;
}
/*-------------------------------------------------
		获取 - 变量显示名 "阶段"
*/
QString C_LEConfigData::getParamShowingName(){
	return this->paramKey.split("-").at(0);
}
/*-------------------------------------------------
		获取 - 配置最大值
*/
int C_LEConfigData::getConfigLen(){
	return this->configLen;
}
/*-------------------------------------------------
		实体类 -> QJsonObject
*/
QJsonObject C_LEConfigData::getJsonObject(){
	QJsonObject obj = QJsonObject();
	obj.insert("pluginName", this->pluginName);
	obj.insert("paramKey", this->paramKey);
	obj.insert("realLen", this->configLen);

	return obj;
}
/*-------------------------------------------------
		QJsonObject -> 实体类
*/
void C_LEConfigData::setJsonObject(QJsonObject obj){
	this->pluginName = obj.value("pluginName").toString();
	this->paramKey = obj.value("paramKey").toString();
	this->configLen = obj.value("realLen").toInt();
}
/*-------------------------------------------------
		空判断
*/
bool C_LEConfigData::isNull(){
	return this->paramKey == "" || this->pluginName == "";
}
/*-------------------------------------------------
		运算符重载
*/
const bool C_LEConfigData::operator== (const C_LEConfigData& a)const {
	return this->pluginName == a.pluginName && this->paramKey == a.paramKey;
}