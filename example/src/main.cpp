#include "ofMain.h"
#include "flowToolsApp.h"
#include "ofAppGlutWindow.h"

//========================================================================
int main( ){
	
	// For some reason the visualization modes don't work this way :(
	//	ofSetupOpenGL(1920, 1080, OF_WINDOW);
	
	ofAppGlutWindow window; // create a window
	ofSetupOpenGL(&window, 1280, 720, OF_WINDOW);

	ofRunApp( new flowToolsApp());

}
