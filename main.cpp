#include "songsorter.h"
#include <QtGui/QApplication>
#include <QtPlugin>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	SongSorter w;
	w.show();
	return a.exec();
}
