#include "ofMain.h"
#include "ofApp.h"

//========================================================================
int main( ){
	
    ofGLFWWindowSettings windowSettings;
#ifdef USE_PROGRAMMABLE_GL
    windowSettings.setGLVersion(4, 1);
#endif
    windowSettings.width = 1280;
    windowSettings.height = 720;
    windowSettings.windowMode = OF_WINDOW;
    
    ofCreateWindow(windowSettings);
    
    ofRunApp(new ofApp());
}
