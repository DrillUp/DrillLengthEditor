#pragma once
#include <QObject>

#include "c_LEConfigData.h"
#include "c_LEAnnotation.h"

#include "Source/Utils/common/TTool.h"
#include "Source/Utils/manager/txtRead/p_TxtFastReader.h"

/*
-----==========================================================-----
		�ࣺ		�ı�ɨ����.h
		����ģ�飺	����ģ��
		���ܣ�		�ṩɨ�衢��дֱ�ӹ��ܡ�
					����ϸ��cpp��
-----==========================================================-----
*/
class S_LEConfigWriter : public QObject
{
	Q_OBJECT

	public:
		S_LEConfigWriter();
		~S_LEConfigWriter();
		static S_LEConfigWriter* cur_manager;			//����
		static S_LEConfigWriter* getInstance();			//��������ȡ�Լ�������Ҫ�õ�ȫ�����ò��ܽ��м��㣩
		void init();									//��ʼ��

	//-----------------------------------
	//----��д
	public:
												//��д - ���ֵ�޸�
												//		��˵��1����ÿ��ֻ�ܽ�һ��ע���������д�루���ע����Ҫ����޸ģ�
												//		��˵��2����ʧ�ܷ��ؿ��ַ���
		QString doOverwritePlugin(QString context, C_LEAnnotation_Param param, C_LEConfigData config);

	private:
												//��ȡ - ����������ָ����
		int indexOfRow(QStringList contextList, QRegExp re);
												//��ȡ - ���ݲ����� "�׶�-1" ��ȡ��һ����@�����б�
												//		��˵��������"@param"��ʼ����ȡ������"@"�С�
		QStringList getParamGroup(QStringList contextList, QRegExp param_re);
		
												//��ȡ - ��ȡָ��λ�õ���������
												//		����������start_row��ʼ�����У� len��������������-1��ʾȫ������
												//		��˵��������ȡÿ��"@param"�����param_name���ݡ�
		QStringList getParamList(QStringList contextList, QString param_name, int start_row = 0, int len = -1);
												//��ȡ - ��ȡָ��λ�õ�����default����
												//		��˵��������ȡÿ��"@param"�����"@default"���ݡ�
		QStringList getParamList_default(QStringList contextList, int start_row = 0, int len = -1);
												//��ȡ - ��ȡָ��λ�õ�����parent����
												//		��˵��������ȡÿ��"@param"�����"@parent"���ݡ�
		QStringList getParamList_parent(QStringList contextList, int start_row = 0, int len = -1);
		
};

