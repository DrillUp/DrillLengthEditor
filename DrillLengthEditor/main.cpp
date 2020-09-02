#include "stdafx.h"
#include "Source/DrillLengthEditor.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	DrillLengthEditor w;
	w.show();
	return a.exec();
}
