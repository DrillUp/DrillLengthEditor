#pragma once

#include <QWidget>
#include <QSet>
#include "c_LEConfigData.h"

/*
-----==========================================================-----
		类：		最大值编辑 流程管理.h
		所属模块：	流程模块
		功能：		属于 核心 与 ui 的中介。
					存储内部的配置数据，ui根据流程中的数据可以随时刷新。
					（详细见cpp）
-----==========================================================-----
*/
class S_LengthFlowManager : public QObject
{
	Q_OBJECT

	public:
		S_LengthFlowManager();
		~S_LengthFlowManager();
		static S_LengthFlowManager* cur_manager;			//单例
		static S_LengthFlowManager* getInstance();			//单例，获取自己（必须要拿到全局配置才能进行计算）
		void init();										//初始化

	//-----------------------------------
	//----单文件
	private:
		C_LEPlugin* m_single_plugin;			//插件信息（只读）
		QList<C_LEConfigData> m_single_data;	//配置参数（临时）
	public:
												//单文件 - 打开单文件（返回成功情况）
		bool openSingle(QString file_name);
												//单文件 - 获取打开后的插件数据
		C_LEPlugin* getLastSinglePlugin();
												//单文件 - 编辑插件
		void editSingle();
												//单文件 - 执行修改
		void executeSingle();


	//-----------------------------------
	//----批量文件
	private:
		QList<C_LEPlugin*> m_batch_pluginList;		//插件信息（只读）
		QList<C_LEConfigData> m_batch_dataList;		//配置参数（临时）
		QList<C_LEConfigData> m_batch_storageList;	//配置参数（存储，只用来导出）
	public:
												//批量文件 - 打开文件夹
		void openBatch(QString dir_name);
												//批量文件 - 获取打开后的插件数据
		QList<C_LEPlugin*> getLastBatchPlugin();
												//批量文件 - 编辑插件
		void editBatchOne( C_LEPlugin* plugin );
												//批量文件 - 执行修改
		void executeBatch();
	private:
												//批量文件 - 打开一个插件
		C_LEPlugin* openBatchOne(QString file_name);
		
	public:
												//批量文件 - 导入配置
		void importConfig();
												//批量文件 - 导出配置
		void exportConfig();

	private:
												//批量文件 - 打开窗口编辑插件
		QJsonObject editOneWithWindow(C_LEPlugin* plugin, QList<C_LEConfigData> data);
												//批量文件 - 根据配置获取到插件
		QList<C_LEConfigData> getBatchConfigDataByPlugin(C_LEPlugin* plugin);
												//批量文件 - 刷新编辑信息
		void refreshBatchEditMessage();
		
};

