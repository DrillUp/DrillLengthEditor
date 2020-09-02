#pragma once


/*
-----==========================================================-----
		类：		插件参数.h（只读）
		所属模块：	核心模块
		功能：		插件的固定参数数据。

		说明：		参数真实只存几个需要的数据。
					其他结构调用此 实体类 都是数据的各种组合结果。
					【另外，尽可能保持数据唯一性，避免"阶段-%d"和"阶段"存成两个变量。】
-----==========================================================-----
*/
class C_LEPluginParam{
	public:
		C_LEPluginParam();
		~C_LEPluginParam();
	private:
		QString paramKey;				//变量键
		QString parentKey;				//变量组键
		QString var;					//定义名
		int varLen;						//定义最大值
		int realLen;					//实际最大值
	public:
										//参数 - 初始化
		void initParam(QString param, QString parentKey, QString var);
										//获取 - 变量键 "阶段-%d"
		QString getParamKey();
										//获取 - 变量组键 "--阶段组%d至%d--"
		QString getParentKey();
										//获取 - 定义名 "DrillUp.g_SSpE_list_length"
		QString getVarName();

										//参数 - 设置 定义最大值
		void setVarLen(int len);
										//参数 - 设置 实际最大值
		void setRealLen(int len);
										//获取 - 定义最大值
		int getVarLen();
										//获取 - 实际最大值
		int getRealLen();

										//获取 - 变量显示名 "阶段"
		QString getParamShowingName();
										//获取 - 变量名 "阶段-210"（从1开始计数）
		QString getParamName(int index);
										//获取 - 变量组名 "--阶段组21至40--"（从1开始计数）
		QString getParentName(int index);
										//获取 - 是否分组
		bool isParentGrouping();
};
/*
-----==========================================================-----
		类：		插件信息.h（只读）
		所属模块：	核心模块
		功能：		插件的固定信息数据。
-----==========================================================-----
*/
class C_LEPlugin{
	public:
		C_LEPlugin();
		~C_LEPlugin();
	public:
		QString pluginName;					//插件名（英文）
		QString pluginDesc;					//插件简介（中文名）
		QString pluginAuthor;				//插件作者
		QList<C_LEPluginParam> paramList;	//插件参数
		bool paramForbidden;				//禁止编辑

		QString fullPath;					//插件文件路径
		QString context;					//插件文本
		QString message;					//插件消息（读取后，显示的字符串）
		QString editMessage;				//插件修改消息（编辑后，编辑数据的字符串）
	public:
											//根据变量键获取参数
		C_LEPluginParam getParamByKey(QString paramKey);
											//判断 - 空判断
		bool isNull();
											//判断 - 是否被禁用
		bool isForbidden();
											//判断 - 是否可编辑
		bool isEditable();
											//判断 - 完整性检查
		bool isIntegrity();
};


/*
-----==========================================================-----
		类：		配置参数.h（存储）
		所属模块：	核心模块
		功能：		插件的配置参数数据。
-----==========================================================-----
*/
class C_LEConfigData {
	public:
		C_LEConfigData();
		~C_LEConfigData();
	private:
		QString pluginName;			//插件名
		QString paramKey;			//变量键
		int configLen;				//配置最大值
	public:
											//参数 - 初始化
		void initParam(QString pluginName, QString paramKey, int realLen);
											//获取 - 插件名（英文）
		QString getPluginName();
											//获取 - 变量键 "阶段-%d"
		QString getParamKey();
											//获取 - 变量显示名 "阶段"
		QString getParamShowingName();
											//获取 - 配置最大值
		int getConfigLen();

											//实体类 -> QJsonObject
		QJsonObject getJsonObject();
											//QJsonObject -> 实体类
		void setJsonObject(QJsonObject obj);
											//空判断
		bool isNull();
											//运算符重载
		const bool operator== (const C_LEConfigData& a)const;
};
inline uint qHash(C_LEConfigData &key, uint seed){	//qHasg全局重载
	return qHash(key.getPluginName(), seed) * qHash(key.getParamKey(), seed);
};