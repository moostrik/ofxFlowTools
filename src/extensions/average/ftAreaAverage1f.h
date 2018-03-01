
#pragma once

#include "ofMain.h"
#include "ftAreaAverage.h"

namespace flowTools {
	
	class ftAreaAverage1f : public ftAreaAverage {
	public:
		ftAreaAverage1f(){ ; }
		virtual	~ftAreaAverage1f() { ; }
		
		void		setup(int _width = 32, int _height = 32, string _name = "");
		void		update();
		
	private:
		
		void allocate(int _width, int _height) ;

		void pRoiXListener(float& _value)		{ ftAreaAverage::pRoiXListener(_value); }
		void pRoiYListener(float& _value)		{ ftAreaAverage::pRoiYListener(_value); }
	};
}
