/*
 * Graph.h
 *
 *  Created on: Aug 17, 2015
 *      Author: vostanin
 */

#ifndef GRAPH_H_
#define GRAPH_H_

#include <gtk/gtk.h>
#include <math.h>
#include <cairo.h>
#include <stdio.h>
#include <vector>
#include <stdint.h>

typedef enum
{
	direction_left = 0,
	direction_top,
	direction_right,
	direction_bottom,

}directions_e;

class Axis
{
public:

	Axis( directions_e direction, uint32_t min, uint32_t max ): m_Direction( direction ), m_Min( min ), m_Max( max )
	{}


private:

	directions_e m_Direction;
	int32_t m_Min;
	int32_t m_Max;
};

class Point
{
public:
	Point( double x, double y) : m_X( x ), m_Y( y ){}
	Point() : m_X( 0 ), m_Y( 0 ){}

	double m_X;
	double m_Y;
};

class Graph
{
public:
	Graph( GtkWidget *window, uint32_t width, uint32_t height );
	Graph( GtkWidget *window );
	virtual ~Graph();

	static Graph * getInstance( GtkWidget * window, uint32_t width, uint32_t height );
	static Graph * getInstance();

	void setWidth( uint32_t width );
	void setHeight( uint32_t height );

	uint32_t getWidth() const;
	uint32_t getHeight() const;

	void addPoint( double x, double y );

private:

	void createWidget();
	static gboolean on_scroll_event( GtkWidget *widget, GdkEventScroll *event, gpointer user_data);
	static gboolean on_expose_event( GtkWidget *widget, GdkEventExpose *event, gpointer user_data );
	static gboolean on_motion_event( GtkWidget *widget, GdkEventMotion *event, gpointer user_data );
	static gboolean on_button_press_event( GtkWidget *widget, GdkEventButton *event, gpointer user_data );
	static gboolean on_button_release_event( GtkWidget *widget, GdkEventButton *event, gpointer user_data );

private:

	static Graph * m_Instance;

	std::vector<Axis> directions;

	GtkWidget * m_GtkFixed;
	GtkWidget * m_parentWindow;
	GtkWidget * m_GtkObject;

	uint32_t m_Width;
	uint32_t m_Height;

	double m_Center_X;
	double m_Center_Y;

	uint32_t m_Zoom;

	bool m_MouseButtonPressed;
	uint32_t m_ButtonPressCoord_X;
	uint32_t m_ButtonPressCoord_Y;

	std::vector<Point> m_Points;
};

#endif /* GRAPH_H_ */
