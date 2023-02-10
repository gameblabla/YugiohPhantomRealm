#pragma once
//#include <Qt/qobject.h>
//#include <Qt/qtimer.h>
//#include <QtGui/qapplication.h>
#include <Base/RenderUnit.h>
class YugiohGame
{
	//QApplication* containingApp;
	//QTimer myTimer;

public:

	RenderUnit renderer;
	bool prepareStartOfGame();
	bool prepareGlobalPointers();
	bool initializeCards();
	bool shutdownCards();
	void update();

	int loop();

	void takeAppPointer();
	void junkFunction();
	bool initialize();
	bool shutdown();
	void go();
};

