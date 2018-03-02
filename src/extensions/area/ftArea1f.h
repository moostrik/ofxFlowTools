
#pragma once

#include "ofMain.h"
#include "ftBaseArea.h"

namespace flowTools {
	
	class ftArea1f : public ftBaseArea {
	public:
		ftArea1f(){ ; }
		virtual	~ftArea1f() { ; }
		
		void		setup(int _width = 32, int _height = 32, string _name = "");
		void		update();
	};
}
