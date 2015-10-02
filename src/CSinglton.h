/*
 * CSinglton.h
 *
 *  Created on: Oct 2, 2015
 *      Author: root
 */

#ifndef CSINGLTON_H_
#define CSINGLTON_H_

template< typename T >
class Singlton
{
public:
	static T & Instance()
	{
		static T instance;
		return instance;
	}
};


#endif /* CSINGLTON_H_ */
