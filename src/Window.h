/*
 * Window.h
 *
 *  Created on: Aug 18, 2015
 *      Author: vostanin
 */

#ifndef WINDOW_H_
#define WINDOW_H_

#include <gtk/gtk.h>
#include <stdint.h>

class Window
{

	Window();
	virtual ~Window();

public:

	static Window & getInstance();

	uint32_t getWidth() const;
	uint32_t getHeight() const;
	GtkWidget * getWindow() const;

	void run();

private:

	GtkWidget * m_Window;

};

#endif /* WINDOW_H_ */
