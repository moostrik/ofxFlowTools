#include "ofMain.h"
#include "flowToolsApp.h"

//========================================================================
int main( ){
	
//	ofSetLogLevel(OF_LOG_VERBOSE);
//	ofSetCurrentRenderer(ofGLProgrammableRenderer::TYPE);
	ofSetupOpenGL(1920, 1080, OF_WINDOW);
	
	ofRunApp( new flowToolsApp());

}
