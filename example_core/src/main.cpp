#include "ofMain.h"
#include "ofApp.h"

//#define USE_PROGRAMMABLE_GL

//========================================================================
int main( ){
	
    ofGLFWWindowSettings windowSettings;
#ifdef USE_PROGRAMMABLE_GL
    windowSettings.setGLVersion(4, 1);
#endif
	windowSettings.setSize(1280, 720);
    windowSettings.windowMode = OF_WINDOW;
    
    ofCreateWindow(windowSettings);
    
    ofRunApp(new ofApp());
}
