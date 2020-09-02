#pragma once

#include <QObject>

/*
-----==========================================================-----
		类：		log文件管理器.h
		版本：		v1.07
		作者：		drill_up
		所属模块：	工具模块
		功能：		读取/转换 log文件的内容。
					【专门对一篇文本解析，速度较慢】
					（详细见cpp）
-----==========================================================-----
*/

class S_TxtReadingManager : public QObject
{
	Q_OBJECT

	//-----------------------------------
	//----基础结构
	public:
		S_TxtReadingManager();
		~S_TxtReadingManager();
		static S_TxtReadingManager* cur_instance;		//单例
		static S_TxtReadingManager* getInstance();		//单例，获取自己（必须要拿到全局配置才能进行计算）
		
	//-----------------------------------
	//----数据
	private:
		QString log_context_org;		//原log
		QStringList log_context_list;	//log行字符（处理后的）
	public:
										//数据 - 初始化
		void initLog();
										//数据 - 设置内容
		void setLog(QString log);

	//-----------------------------------
	//----全行
										//全行 - 清理空格
		void trimAllRows();
										//全行 - 替换字符
		void replaceInStrings(QString re_str, QString str);
		void replaceInStrings(QRegExp re, QString str);
										//全行 - 清理空行
		void clearEmptyRows();
										//全行 - 去除注释行（根据首字符）
		void clearComment(QString prefix);
										//全行 - 获取全行（暂不建议直接获取）
		//QStringList getAllRows();

	//-----------------------------------
	//----行修改
										//行修改 - 添加行
		void appendRow(QString context);
										//行修改 - 添加多行
		void appendRows(QStringList context);
										//行修改 - 插入行
		void insertRow(int index, QString context);
										//行修改 - 插入多行
		void insertRows(int index, QStringList context);
										//行修改 - 替换行
		void replaceRow(int index, QString context);
										//行修改 - 去除行
		void removeRow(int index);
										//行修改 - 去除多行
		void removeRows(QList<int> index_list);

	//-----------------------------------
	//----直接操作（directly）
	private:
		QSet<int> m_used_rows;			//操作过的行
	public:
										//直接 - 第i行
		QString d_rowAt(int i);
										//直接 - 获取全部行数
		int d_rowCount();
										//直接 - 获取包含行
										//		【参数1】：包含的索引字符串
										//		【返回】：索引值（未找到返回-1）
		int d_indexOf(QString contains);
		int d_indexOf(QRegExp re_contains);
										//直接 - 获取包含行（从末尾找）
										//		【参数1】：包含的索引字符串
										//		【返回】：索引值（未找到返回-1）
		int d_lastIndexOf(QString contains);
		int d_lastIndexOf(QRegExp re_contains);
										//直接 - 获取包含行（指定范围）
										//		【参数1】起始行
										//		【参数2】终止行
										//		【返回】索引值（未找到返回-1）
		int d_indexOf(QString contains, int n, int m = -1);
		int d_indexOf(QRegExp re_contains, int n, int m = -1);
										//直接 - 获取包含行下的所有行
										//		【参数1】起始行
										//		【参数2】长度
										//		【返回】行列表（未找到/光标超过总行数 返回空列表）
		QStringList d_getRows(int n, int row_count = -1);
										//直接 - 获取包含的所有行
										//		【参数1】包含的索引字符串
										//		【返回】所有包含的行（未找到返回空列表）
		QList<int> d_getAllRowIndexsContains(QString contains);
		QList<int> d_getAllRowIndexsContains(QRegExp re_contains);
		QStringList d_getAllRowsContains(QString contains);
		QStringList d_getAllRowsContains(QRegExp re_contains);

										//直接 - 获取上述函数操作到的行
										//		【返回】操作到的行（如果有数据修改，行下标也会跟着变）
		QList<int> d_getUsedIndexs();
										//直接 - 清除记录的操作行
		void d_clearUsedIndexs();
										//直接 - 获取所有上述函数未操作到的行
										//		【参数1】返回模式（见cpp函数）
										//		【返回】未操作到的行列表
		QStringList d_getUnusedRows(QString mode = "");
		
	//-----------------------------------
	//----迭代操作（orderly）
	private:
		int cur_pos;
	public:
										//顺序操作 - 开始遍历
		//void o_start();
										//顺序操作 - 判断遍历光标
		//bool o_isAtEnd();

		//bool o_nextRow();
	public:
		
	//-----------------------------------
	//----字符串
	public:
										//字符串 - 获取浮点数（非榨取， 1.0e3 浮点数两边有空格才被获取）
										//		【参数1】单行字符串
										//		【返回】数列表
		QList<double> s_getDoubleInRow(QString row);
										//字符串 - 获取浮点数（榨取）
		QList<double> s_getDoubleForceInRow(QString row);
										//字符串 - 获取整数（非榨取， 23 整数数两边有空格才被获取）
										//		【参数1】单行字符串
										//		【返回】数列表
		QList<int> s_getIntInRow(QString row);
										//字符串 - 获取整数（榨取）
		QList<int> s_getIntForceInRow(QString row);

};

