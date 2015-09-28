/*
 * Window.cpp
 *
 *  Created on: Aug 18, 2015
 *      Author: vostanin
 */

#include "Window.h"
#include <cstdlib>


#define WIDTH   300
#define HEIGHT  880

static void my_quit()
{
	exit( 0 );
}

Window::Window()
{
	int argc = 0;
	char ** argv = NULL;
	gtk_init( &argc, &argv);

	m_Window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
	gtk_window_set_default_size (GTK_WINDOW (m_Window), WIDTH, HEIGHT);
	gtk_window_set_title (GTK_WINDOW (m_Window), "Graph drawing");
	g_signal_connect (G_OBJECT (m_Window), "destroy", my_quit, NULL);
}

Window::~Window()
{
}

Window & Window::getInstance()
{
	static Window instance;
	return instance;
}

void Window::run()
{
	gtk_widget_show_all( getWindow() );
	gtk_main ();
}

uint32_t Window::getWidth() const
{
	return WIDTH;
}

uint32_t Window::getHeight() const
{
	return HEIGHT;
}

GtkWidget * Window::getWindow() const
{
	return m_Window;
}
