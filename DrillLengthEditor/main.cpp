#include "stdafx.h"
#include "Source/MainModule/DrillLengthEditor.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);

	// > Ö÷´°Ìå
	DrillLengthEditor::getInstance()->show();

	return a.exec();
}
