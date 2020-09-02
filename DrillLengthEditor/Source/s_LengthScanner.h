#pragma once

#include <QWidget>
#include "Utils/common/TTool.h"
#include "Utils/manager/txtRead/p_TxtFastReader.h"
#include "c_LEConfigData.h"

/*
-----==========================================================-----
		类：		文本扫描器.h
		所属模块：	核心模块
		功能：		提供扫描、覆写直接功能。
					（详细见cpp）
-----==========================================================-----
*/
class S_LengthScanner : public QObject
{
	Q_OBJECT

	public:
		S_LengthScanner();
		~S_LengthScanner();
		static S_LengthScanner* cur_manager;			//单例
		static S_LengthScanner* getInstance();			//单例，获取自己（必须要拿到全局配置才能进行计算）
		void init();									//初始化
		
	//-----------------------------------
	//----扫描
	public:
												//扫描 - 获取参数 + 最大值
		C_LEPlugin* doScanPlugin(QString file_name);
												//扫描 - 刷新参数最大值
		void refreshScanPluginLenth(C_LEPlugin* plugin);

												//扫描 - 文本是否存在"xxx-10"序列格式
		bool isPluginIncludedLengthParam(QString context);


	//-----------------------------------
	//----覆写
	public:
												//覆写 - 最大值修改（失败返回空字符串）
		QString doOverwritePlugin(QString context, C_LEPluginParam param, C_LEConfigData config);
	private:
												//覆写 - 根据 "阶段-1" 获取到 "@param 阶段-1" 的全部参数字符串
		QStringList getParamStringList(QStringList contextList, QString paramName);
		
};

