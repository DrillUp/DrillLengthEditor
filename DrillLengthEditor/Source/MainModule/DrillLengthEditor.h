#ifndef DRILLENCRYPTOR_H
#define DRILLENCRYPTOR_H

#include <QtWidgets/QMainWindow>
#include "ui_DrillLengthEditor.h"

class DrillLengthEditor : public QMainWindow
{
	Q_OBJECT

	public:
		DrillLengthEditor(QWidget *parent = 0);
		~DrillLengthEditor();
		static DrillLengthEditor* cur_instance;		//单例
		static DrillLengthEditor* getInstance();	//单例，获取自己
		void _init();								//初始化

	//-----------------------------------
	//----步骤
	public:
		void toStepBatch();
		void toStepSingle();
		void openWord();
		void openUpdateLog();
		void openAbout();

	//-----------------------------------
	//----单文件修改
	public:
		bool m_open_successed;
		QString m_single_filePath;
	public slots:
						//单文件 - 读取插件单文件
		void openSingleFile();
						//单文件 - 编辑插件
		void openSingleParamEdit();
						//单文件 - 重刷ui
		void refreshSingleUi();

						//单文件 - 下一步
		void toNextSingle();
						//单文件 - 上一步
		void toLastSingle();
						//单文件 - 提交
		void submitSingle();

	//-----------------------------------
	//----批量修改
	public:
		QString m_batch_dirPath;
		bool m_isHideUneditableFile;
	public slots:
						//批量修改 - 读取插件文件夹
		void openBatchDir();
						//批量修改 - 选择一个编辑
		void openBatchParamOneEdit();
						//批量修改 - 重刷ui
		void refreshBatchUi();

						//批量修改 - 下一步
		void toNextBatch();
						//批量修改 - 上一步
		void toLastBatch();
						//批量修改 - 提交
		void submitBatch();
						//批量修改 - 隐藏不可修改文件
		void checkBoxChanged(bool b);
						//批量修改 - 导入配置
		void importBatch();
						//批量修改 - 导出配置
		void exportBatch();

		
	//-----------------------------------
	//----窗口
	private:
		Ui::DrillLengthEditorClass ui;
};

#endif // DRILLENCRYPTOR_H
