#include "ofMain.h"
#include "flowToolsApp.h"

#ifdef USE_PROGRAMMABLE_GL
#include "ofGLProgrammableRenderer.h"
#endif

//========================================================================
int main( ){
	
#ifdef 	USE_PROGRAMMABLE_GL	
	ofPtr<ofBaseRenderer> renderer(new ofGLProgrammableRenderer(false));
	ofSetCurrentRenderer(renderer, false);
#endif
	ofSetupOpenGL(1280, 720, OF_WINDOW);
	
	ofRunApp( new flowToolsApp());

}
