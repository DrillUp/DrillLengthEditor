#pragma once

#include <QWidget>

/*
-----==========================================================-----
		类：		快速类定义.h
		版本：		v1.07
		作者：		drill_up
		所属模块：	工具模块
		功能：		定义一个表单填写用的类定义，快速表单、快速窗口通用。
					【这里类似于定义model，所有字段都是字符串】
					【类名、参数名，用中文】
					（详细见cpp）
-----==========================================================-----
*/
class C_FastClassParam;
class C_FastClass{

	public:
		C_FastClass();
		C_FastClass(QString className);
		~C_FastClass();
		
	//-----------------------------------
	//----设置
	public:
							//设置 - 类名
		void setClassName(QString classNameChinese);
		void setClassNameEnglish(QString classNameEnglish);
		void setClassNameShowing(QString classNameShowing);
							//设置 - double
		void addDouble(QString paramNameChinese);
		void addDouble(QString paramNameChinese, QString defaultValue);
							//设置 - Int
		void addInt(QString paramNameChinese);
		void addInt(QString paramNameChinese, QString defaultValue);
							//设置 - QString
		void addQString(QString paramNameChinese);
		void addQString(QString paramNameChinese, QString defaultValue);
							//设置 - QString（含选项）
		void addQStringWithChoose(QString paramNameChinese, QString defaultValue, QStringList chooseList);
							//高级 - 类嵌套
		void addFastClassQWidget(C_FastClass fastClass);
		void addFastClassOneLine(C_FastClass fastClass);
		void addFastClassQGroupBox(C_FastClass fastClass);

	//-----------------------------------
	//----获取
	public:
							//获取 - 类名
		QString getClassNameChinese();
		QString getClassNameEnglish();
		QString getClassNameShowing();
							//获取 - 快速类参数（含FastClass）
		QList<C_FastClassParam> getParamList();
							//获取 - 快速类参数（FastClass全部被摊开）
		QList<C_FastClassParam> getConvertedParamList();
							//获取 - 所有参数中文名（包括类套类的参数）
		QStringList getParamChineseNameList();
							//获取 - 所有参数英文名（包括类套类的参数）
		QStringList getParamEnglishNameList();
							//获取 - 子快速类
		C_FastClass getChildClass(QString chineseName);
							//获取 - 判断空
		bool isNull();

	//-----------------------------------
	//----二设
	public:
							//二设 - 英文名
		void setParamEnglishName(QString paramNameChinese, QString paramNameEnglish);
							//二设 - 显示名
		void setParamShowingName(QString paramNameChinese, QString showingName);
							//二设 - 必填项
		void setParamNotNull(QString paramNameChinese, bool notNull);
							//二设 - 后缀
		void setParamSuffix(QString paramNameChinese, QString suffix);
		
	//-----------------------------------
	//----三设
	public:
							//三设 - SI写法
		void setParamSI(QString paramNameChinese, bool isSI);
							//三设 - 不可编辑
		void setParamEditable(QString paramNameChinese, bool isEditable);
							//三设 - 默认值修改
		void setParamDefaultValue(QString paramNameChinese, QString defaultValue);
		
	//-----------------------------------
	//----高级
	public:
							//高级 - 所在列数
		void setParamColumn(QString paramNameChinese, int col);
		
		
	//-----------------------------------
	//----特殊功能
	public:
							//特殊功能 - 将QJsonObject的数据，根据该类的格式，填充到table中（中文）
		void fillDataInTableWidget(QTableWidget* tableWidget, QList<QJsonObject> data_list);
							//特殊功能 - 将QJsonObject的数据，根据该类的格式，填充到table中（英文）
		void fillDataInTableWidgetEnglish(QTableWidget* tableWidget, QList<QJsonObject> data_list);
							//特殊功能 - jsonobj 英文 -> 中文
		QJsonObject convertEnglishToChinese(QJsonObject data);
							//特殊功能 - jsonobj 英文 -> 中文（转不了的扔掉）
		QJsonObject convertEnglishToChineseExcludely(QJsonObject data);
							//特殊功能 - jsonobj 中文 -> 英文
		QJsonObject convertChineseToEnglish(QJsonObject data);
							//特殊功能 - jsonobj 中文 -> 英文（转不了的扔掉）
		QJsonObject convertChineseToEnglishExcludely(QJsonObject data);
							//特殊功能 - 去掉中文名的标识格式
		QString removeSpecialChineseCharacter(QString paramNameChinese);

	//-----------------------------------
	//----私有参数
	protected:
		QString classNameChinese;			//类中文名
		QString classNameEnglish;			//类英文名
		QString classNameShowing;			//类显示名
		QList<C_FastClassParam> paramList;	//类参数集
		QStringList nameChinese_list;		//全部中文名
		QStringList nameEnglish_list;		//全部英文名
	protected:
		C_FastClassParam getParamByName(QString nameChinese);
		int getIndexByName(QString nameChinese);
		void checkDuplicatedChineseValue(QString nameChinese);
		void checkDuplicatedEnglishValue(QString nameEnglish);
};
/*
-----==========================================================-----
		类：		快速类参数.h
-----==========================================================-----
*/
class C_FastClassParam{

	public:
		C_FastClassParam();
		~C_FastClassParam();

	public:
		QString nameChinese;		//中文名
		QString nameEnglish;		//英文名
		QString type;				//类型
		QString widgetType;			//表单类型
		QString defaultValue;		//默认值
		QStringList chooseList;		//选项列表

		QString nameShowing;		//实际显示名
		bool notNull;				//必填
		QString suffix;				//后缀（单位）
		bool isSI;					//SI写法
		bool isEditable;			//可编辑

		int columnPos;				//所在列
		C_FastClass fastClass;		//嵌套体

		QString tempA;				//缓冲数据A
		QString tempB;				//缓冲数据B
		QString tempC;				//缓冲数据C
		QString tempD;				//缓冲数据D
		QString tempE;				//缓冲数据E
		QString tempF;				//缓冲数据F

	//-----------------------------------
	//----获取
	public:
									//运算符重载
		const bool operator== (const C_FastClassParam& a)const;
									//判断空
		bool isNull();
									//获取 - 显示文本名
		QString getShowingName();
	public:
									//获取 - 去掉中文名的标识用字符（静态函数）
		static QString removeSpecialChineseCharacter(QString nameChinese);

};