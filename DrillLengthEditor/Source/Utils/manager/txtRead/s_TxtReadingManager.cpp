#include "stdafx.h"
#include "S_TxtReadingManager.h"

#include "../../common/TTool.h"

/*
-----==========================================================-----
		类：		log文件管理器.cpp
		版本：		v1.07
		作者：		drill_up
		所属模块：	工具模块
		功能：		读取/规范/转换 文本文件的内容。
					【专门对一篇文本解析，速度较慢】

		目标：		1.避免字符串分割时出现的数组溢出。
					2.规范确认每行都能被读到。
					3.直接操作：直接在行中找到相关字符。
					  迭代操作：一条一条往下找，只遍历一次。
					4.作用于文本长短变化较大的log日志文件的翻译。

		注意事项：	1.该管理器只读文本，并返回规范好的文本。

		使用方法：
				>规范文本
					S_TxtReadingManager* _log_ = S_TxtReadingManager::getInstance();
					_log_->initLog();
					_log_->setLog(context);
	
					_log_->trimAllRows();
					_log_->clearEmptyRows();
					_log_->clearComment("#");

-----==========================================================-----
*/

S_TxtReadingManager::S_TxtReadingManager(){
	initLog();
}
S_TxtReadingManager::~S_TxtReadingManager() {
}
/* -------------单例------------ */
S_TxtReadingManager* S_TxtReadingManager::cur_instance = NULL;
S_TxtReadingManager* S_TxtReadingManager::getInstance() {
	if (cur_instance == NULL) {
		cur_instance = new S_TxtReadingManager();
	}
	return cur_instance;
}

/*-------------------------------------------------
		数据 - 初始化
*/
void S_TxtReadingManager::initLog() {
	this->log_context_org = "";
	this->log_context_list = QStringList();
}
/*-------------------------------------------------
		数据 - 设置内容
*/
void S_TxtReadingManager::setLog(QString log) {
	this->log_context_org = log;
	this->log_context_list = this->log_context_org.split(QRegExp("[\n\r]+"));
}

/*-------------------------------------------------
		全行 - 清理空格
*/
void S_TxtReadingManager::trimAllRows() {
	this->log_context_list.replaceInStrings(QRegExp("^[ \t]+"), "");
	this->log_context_list.replaceInStrings(QRegExp("[ \t]+$"), "");
}
/*-------------------------------------------------
		全行 - 替换字符
*/
void S_TxtReadingManager::replaceInStrings(QString re_str, QString str) {
	this->log_context_list.replaceInStrings(re_str, str);
}
void S_TxtReadingManager::replaceInStrings(QRegExp re, QString str) {
	this->log_context_list.replaceInStrings(re, str);
}
/*-------------------------------------------------
		全行 - 清理空行
*/
void S_TxtReadingManager::clearEmptyRows() {
	QStringList result = QStringList();
	for (int i = 0; i < this->log_context_list.count(); i++) {
		QString str = this->log_context_list.at(i);
		if (str != "") {
			result.push_back(str);
		}
	}
	this->log_context_list = result;
}
/*-------------------------------------------------
		全行 - 去除注释行（根据首字符）
*/
void S_TxtReadingManager::clearComment(QString prefix) {
	QStringList result = QStringList();
	for (int i = 0; i < this->log_context_list.count(); i++) {
		QString str = this->log_context_list.at(i);
		if (str.mid(0, prefix.length()) != prefix) {
			result.push_back(str);
		}
	}
	this->log_context_list = result;
}
/*-------------------------------------------------
		全行 - 获取全行
QStringList S_TxtReadingManager::getAllRows() {
	return this->log_context_list;
}
*/

/*-------------------------------------------------
		行修改 - 添加行
*/
void S_TxtReadingManager::appendRow(QString context) {
	this->log_context_list.append(context);
}
/*-------------------------------------------------
		行修改 - 添加多行
*/
void S_TxtReadingManager::appendRows(QStringList context) {
	this->log_context_list.append(context);
}
/*-------------------------------------------------
		行修改 - 插入行
*/
void S_TxtReadingManager::insertRow(int index, QString context) {
	this->log_context_list.insert(index,context);
	QList<int> list = m_used_rows.toList();		//操作过的行修改下标
	for (int i = 0; i < list.count();i++) {
		if (list.at(i) >= index) { list[i] = list.at(i) + 1; }
	}
	m_used_rows = list.toSet();
}
/*-------------------------------------------------
		行修改 - 插入多行
*/
void S_TxtReadingManager::insertRows(int index, QStringList context) {
	for (int i = 0; i < context.count(); i++) {
		this->log_context_list.insert(index+i, context.at(i));
	}
	QList<int> list = m_used_rows.toList();		//操作过的行修改下标
	for (int i = 0; i < list.count(); i++) {
		if (list.at(i) >= index) {
			list[i] = list.at(i) + context.count();
		}
	}
	m_used_rows = list.toSet();
}
/*-------------------------------------------------
		行修改 - 替换行
*/
void S_TxtReadingManager::replaceRow(int index, QString context) {
	this->log_context_list.replace(index, context);
	QList<int> list = m_used_rows.toList();		//操作过的行修改下标
	for (int i = 0; i < list.count(); i++) {
		if (list.at(i) == index) {
			list.removeAt(i);
			break;
		}
	}
	m_used_rows = list.toSet();
}
/*-------------------------------------------------
		行修改 - 去除行
*/
void S_TxtReadingManager::removeRow(int index) {
	this->log_context_list.removeAt(index);
	QList<int> list = m_used_rows.toList();		//操作过的行修改下标
	for (int i = 0; i < list.count(); i++) {
		if (list.at(i) == index) {
			list.removeAt(i);
			break;
		}
	}
	m_used_rows = list.toSet();
}
/*-------------------------------------------------
		行修改 - 去除多行
*/
void S_TxtReadingManager::removeRows(QList<int> index_list) {
	qSort(index_list.begin(), index_list.end());
	for (int i = index_list.count()-1; i >= 0; i--) {
		this->log_context_list.removeAt(index_list.at(i));
	}
	QList<int> list = m_used_rows.toList();		//操作过的行修改下标
	for (int i = list.count()-1; i >= 0; i--) {
		if (index_list.contains(list.at(i))) {
			list.removeAt(i);
			break;
		}
	}
	m_used_rows = list.toSet();
}

/*-------------------------------------------------
		直接 - 第i行
*/
QString S_TxtReadingManager::d_rowAt(int i) {
	if (i<0 || i>=this->d_rowCount()) {
		return "";
	}
	m_used_rows.insert(i);
	return log_context_list.at(i);
}
/*-------------------------------------------------
		直接 - 行数
*/
int S_TxtReadingManager::d_rowCount() {
	return log_context_list.count();
}

/*-------------------------------------------------
		直接 - 获取包含行
*/
int S_TxtReadingManager::d_indexOf(QString contains) {
	for (int i = 0; i < this->log_context_list.count(); i++) {
		QString result = this->log_context_list.at(i);
		if (result.contains(contains)) {
			m_used_rows.insert(i);
			return i;
		}
	}
	return -1;
}
int S_TxtReadingManager::d_indexOf(QRegExp re_contains) {
	for (int i = 0; i < this->log_context_list.count(); i++) {
		QString result = this->log_context_list.at(i);
		if (result.contains(re_contains)) {
			m_used_rows.insert(i);
			return i;
		}
	}
	return -1;
}
/*-------------------------------------------------
		直接 - 获取包含行（从末尾找）
*/
int S_TxtReadingManager::d_lastIndexOf(QString contains) {
	for (int i = this->log_context_list.count()-1; i >=0 ; i++) {
		QString result = this->log_context_list.at(i);
		if (result.contains(contains)) {
			m_used_rows.insert(i);
			return i;
		}
	}
	return -1;
}
int S_TxtReadingManager::d_lastIndexOf(QRegExp re_contains) {
	for (int i = this->log_context_list.count() - 1; i >= 0; i++) {
		QString result = this->log_context_list.at(i);
		if (result.contains(re_contains)) {
			m_used_rows.insert(i);
			return i;
		}
	}
	return -1;
}

/*-------------------------------------------------
		直接 - 获取包含行（指定范围）
*/
int S_TxtReadingManager::d_indexOf(QString contains, int n, int m) {
	if (n == -1) { n = 0; }
	if (m == -1) { m = this->log_context_list.count(); }
	if (m >= this->log_context_list.count() ) { m = this->log_context_list.count(); }
	for (int i = n; i < m; i++) {
		QString result = this->log_context_list.at(i);
		if (result.contains(contains)) {
			m_used_rows.insert(i);
			return i;
		}
	}
	return -1;
}
int S_TxtReadingManager::d_indexOf(QRegExp re_contains, int n, int m) {
	if (n == -1) { n = 0; }
	if (m == -1) { m = this->log_context_list.count(); }
	if (m >= this->log_context_list.count()) { m = this->log_context_list.count(); }
	for (int i = n; i < m; i++) {
		QString result = this->log_context_list.at(i);
		if (result.contains(re_contains)) {
			m_used_rows.insert(i);
			return i;
		}
	}
	return -1;
}

/*-------------------------------------------------
		直接 - 获取包含行（指定范围）
*/
QStringList S_TxtReadingManager::d_getRows(int n, int row_count) {
	if (n == -1) { n = 0; }
	int m = n + row_count;
	if (row_count == -1) { m = this->log_context_list.count(); }
	if (m >= this->log_context_list.count()) { m = this->log_context_list.count(); }
	QStringList result_list = QStringList();
	for (int i = n; i < m; i++) {
		QString result = this->log_context_list.at(i);
		m_used_rows.insert(i);
		result_list.push_back(result);
	}
	return result_list;
}


/*-------------------------------------------------
		直接 - 获取包含的所有行
*/
QList<int> S_TxtReadingManager::d_getAllRowIndexsContains(QString contains){
	QList<int> result_list = QList<int>();
	for (int i = 0; i < this->log_context_list.count(); i++) {
		QString result = this->log_context_list.at(i);
		if (result.contains(contains)) {
			m_used_rows.insert(i);
			result_list.push_back(i);
		}
	}
	return result_list;
}
QList<int> S_TxtReadingManager::d_getAllRowIndexsContains(QRegExp re_contains){
	QList<int> result_list = QList<int>();
	for (int i = 0; i < this->log_context_list.count(); i++) {
		QString result = this->log_context_list.at(i);
		if (result.contains(re_contains)) {
			m_used_rows.insert(i);
			result_list.push_back(i);
		}
	}
	return result_list;
}
QStringList S_TxtReadingManager::d_getAllRowsContains(QString contains) {
	QStringList result_list = QStringList();
	for (int i = 0; i < this->log_context_list.count(); i++) {
		QString result = this->log_context_list.at(i);
		if (result.contains(contains)) {
			m_used_rows.insert(i);
			result_list.push_back(result);
		}
	}
	return result_list;
}
QStringList S_TxtReadingManager::d_getAllRowsContains(QRegExp re_contains) {
	QStringList result_list = QStringList();
	for (int i = 0; i < this->log_context_list.count(); i++) {
		QString result = this->log_context_list.at(i);
		if (result.contains(re_contains)) {
			m_used_rows.insert(i);
			result_list.push_back(result);
		}
	}
	return result_list;
}

/*-------------------------------------------------
		直接 - 获取上述函数操作到的行
*/
QList<int> S_TxtReadingManager::d_getUsedIndexs(){
	return m_used_rows.toList();
}
void S_TxtReadingManager::d_clearUsedIndexs() {
	m_used_rows.clear();
}
/*-------------------------------------------------
		直接 - 获取上述函数操作到的行
*/
QStringList S_TxtReadingManager::d_getUnusedRows(QString mode) {
	QStringList result_list = QStringList();
	for (int i = 0; i < this->log_context_list.count() ; i++) {
		QString str = this->log_context_list.at(i);
		if ( !m_used_rows.contains(i)) {
			result_list.push_back(str);
		}
		else {
			if (mode == "") { continue; }
			if (mode == "empty_mode"){
				result_list.push_back("");
			}
			if (mode == "color_mode") {
				result_list.push_back("<span style=\" color:#bbbbbb; \">" + str + "</span>");	//彩色显示
			}
		}
	}
	return result_list;
}

/*-------------------------------------------------
		字符串 - 获取整数
*/
QList<int> S_TxtReadingManager::s_getIntInRow(QString row) {
	QList<int> result_list = QList<int>();
	QStringList s_list = row.split(QRegExp("[ \t]+"));
	for (int i = 0; i < s_list.count(); i++) {
		QString result = s_list.at(i);
		if (TTool::_match_re_int_(result)) {
			result_list.push_back(result.toInt());
		}
	}
	return result_list;
}
QList<int> S_TxtReadingManager::s_getIntForceInRow(QString row) {
	QList<int> result_list = QList<int>();
	QStringList s_list = row.split(QRegExp("[ \t]+"));
	for (int i = 0; i < s_list.count(); i++) {
		QString result = s_list.at(i);
		QList<int> l = TTool::_to_ints_(result);
		if (l.count() >= 1){
			result_list.push_back(l.at(0));
		}else{
			result_list.push_back(0);
		}
	}
	return result_list;
}

/*-------------------------------------------------
		字符串 - 获取浮点数
*/
QList<double> S_TxtReadingManager::s_getDoubleInRow(QString row) {
	QList<double> result_list = QList<double>();
	QStringList s_list = row.split(QRegExp("[ \t]+"));
	for (int i = 0; i < s_list.count(); i++) {
		QString result = s_list.at(i);
		if (TTool::_match_re_double_e_(result)) {
			result_list.push_back(result.toDouble());
		}
	}
	return result_list;
}
QList<double> S_TxtReadingManager::s_getDoubleForceInRow(QString row) {
	QList<double> result_list = QList<double>();
	QStringList s_list = row.split(QRegExp("[ \t]+"));
	for (int i = 0; i < s_list.count(); i++) {
		QString result = s_list.at(i);
		QList<double> l = TTool::_to_doubles_e_(result);
		if (l.count() >= 1){
			result_list.push_back(l.at(0));
		}else{
			result_list.push_back(0);
		}
	}
	return result_list;
}