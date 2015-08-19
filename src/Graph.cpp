/*
 * Graph.cpp
 *
 *  Created on: Aug 17, 2015
 *      Author: vostanin
 */

#include "Graph.h"
#include <cmath>
#include <cstdlib>

#include "gsgraph/gs-graph.h"
#include "gsgraph/gs-function-plot.h"
#include "gsgraph/gs-dataset-plot.h"

#define ZOOM_X  100.0
#define ZOOM_Y  100.0

Graph * Graph::m_Instance = NULL;

static gfloat f (gfloat x)
{
    return sin (x*8);
}

Graph::Graph( GtkWidget * window, uint32_t width, uint32_t height ) : m_parentWindow( window), m_Width( width ), m_Height( height ), m_Zoom( 100 ), m_MouseButtonPressed( false )
{
	createWidget();
}

Graph::Graph( GtkWidget * window ) : m_parentWindow( window), m_Width( 10 ), m_Height( 10 ), m_Zoom( 100 ), m_MouseButtonPressed( false )
{
	createWidget();
}

Graph::~Graph()
{

}

Graph * Graph::getInstance( GtkWidget * window, uint32_t width, uint32_t height )
{
	if( Graph::m_Instance == NULL )
	{
		Graph::m_Instance = new Graph( window, width, height );
	}
	return Graph::m_Instance;
}

Graph * Graph::getInstance()
{
	return Graph::m_Instance;
}

void Graph::createWidget()
{
	m_GtkFixed = gtk_fixed_new();
	m_GtkObject = gs_graph_new ();

	gtk_container_add(GTK_CONTAINER(m_parentWindow), m_GtkFixed);
	gtk_widget_show(m_GtkFixed);
	gtk_fixed_put (GTK_FIXED (m_GtkFixed), m_GtkObject, 0, 0);

	gtk_widget_show( m_GtkObject );

	gtk_widget_show_all (m_parentWindow);
}

void Graph::setWidth( uint32_t width )
{
	m_Width = width;
	gtk_drawing_area_size( (GtkDrawingArea*) m_GtkObject, m_Width, m_Height );
}

void Graph::setHeight( uint32_t height )
{
	m_Height = height;
	gtk_drawing_area_size( (GtkDrawingArea*) m_GtkObject, m_Width, m_Height );
}

uint32_t Graph::getWidth() const
{
	return m_Width;
}
uint32_t Graph::getHeight() const
{
	return m_Height;
}

void Graph::addPoint( double x, double y )
{
	m_PointsX.push_back( x );
	m_PointsY.push_back( y );
}

void Graph::drawPoints()
{
	double * p_X = &m_PointsX[0];
	double * p_Y = &m_PointsY[0];

	GsPlot * plot = gs_dataset_plot_new_with_data( p_X, p_Y, m_PointsX.size() );
	GdkColor color = { 0, 1280, 128, 128 };
	gs_plot_set_color( plot, &color );
	gs_graph_attach_plot( GS_GRAPH( m_GtkObject ), plot );
}

