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
		static DrillLengthEditor* cur_instance;		//����
		static DrillLengthEditor* getInstance();	//��������ȡ�Լ�
		void _init();								//��ʼ��

	//-----------------------------------
	//----����
	public:
		void toStepBatch();
		void toStepSingle();
		void openWord();
		void openUpdateLog();
		void openAbout();

	//-----------------------------------
	//----���ļ��޸�
	public:
		bool m_open_successed;
		QString m_single_filePath;
	public slots:
						//���ļ� - ��ȡ������ļ�
		void openSingleFile();
						//���ļ� - �༭���
		void openSingleParamEdit();
						//���ļ� - ��ˢui
		void refreshSingleUi();

						//���ļ� - ��һ��
		void toNextSingle();
						//���ļ� - ��һ��
		void toLastSingle();
						//���ļ� - �ύ
		void submitSingle();

	//-----------------------------------
	//----�����޸�
	public:
		QString m_batch_dirPath;
		bool m_isHideUneditableFile;
	public slots:
						//�����޸� - ��ȡ����ļ���
		void openBatchDir();
						//�����޸� - ѡ��һ���༭
		void openBatchParamOneEdit();
						//�����޸� - ��ˢui
		void refreshBatchUi();

						//�����޸� - ��һ��
		void toNextBatch();
						//�����޸� - ��һ��
		void toLastBatch();
						//�����޸� - �ύ
		void submitBatch();
						//�����޸� - ���ز����޸��ļ�
		void checkBoxChanged(bool b);
						//�����޸� - ��������
		void importBatch();
						//�����޸� - ��������
		void exportBatch();

		
	//-----------------------------------
	//----����
	private:
		Ui::DrillLengthEditorClass ui;
};

#endif // DRILLENCRYPTOR_H
