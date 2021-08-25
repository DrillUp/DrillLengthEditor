#pragma once
#include <QObject>

#include "c_LEConfigData.h"
#include "c_LEAnnotation.h"

#include "Source/Utils/common/TTool.h"
#include "Source/Utils/manager/txtRead/p_TxtFastReader.h"

/*
-----==========================================================-----
		类：		文本扫描器.h
		所属模块：	核心模块
		功能：		提供扫描、覆写直接功能。
					（详细见cpp）
-----==========================================================-----
*/
class S_LEConfigWriter : public QObject
{
	Q_OBJECT

	public:
		S_LEConfigWriter();
		~S_LEConfigWriter();
		static S_LEConfigWriter* cur_manager;			//单例
		static S_LEConfigWriter* getInstance();			//单例，获取自己（必须要拿到全局配置才能进行计算）
		void init();									//初始化

	//-----------------------------------
	//----覆写
	public:
												//覆写 - 最大值修改
												//		【说明1】：每次只能将一个注解参数覆盖写入（多个注解需要多次修改）
												//		【说明2】：失败返回空字符串
		QString doOverwritePlugin(QString context, C_LEAnnotation_Param param, C_LEConfigData config);

	private:
												//获取 - 符合条件的指定行
		int indexOfRow(QStringList contextList, QRegExp re);
												//获取 - 根据参数名 "阶段-1" 获取到一连串@参数列表
												//		【说明】：从"@param"开始，获取连续的"@"行。
		QStringList getParamGroup(QStringList contextList, QRegExp param_re);
		
												//获取 - 获取指定位置的连续数据
												//		【参数】：start_row开始搜索行， len连续搜索行数（-1表示全部）。
												//		【说明】：获取每个"@param"后面的param_name数据。
		QStringList getParamList(QStringList contextList, QString param_name, int start_row = 0, int len = -1);
												//获取 - 获取指定位置的连续default数据
												//		【说明】：获取每个"@param"后面的"@default"数据。
		QStringList getParamList_default(QStringList contextList, int start_row = 0, int len = -1);
												//获取 - 获取指定位置的连续parent数据
												//		【说明】：获取每个"@param"后面的"@parent"数据。
		QStringList getParamList_parent(QStringList contextList, int start_row = 0, int len = -1);
		
};

