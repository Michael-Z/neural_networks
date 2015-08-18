/*
 * Graph.cpp
 *
 *  Created on: Aug 17, 2015
 *      Author: vostanin
 */

#include "Graph.h"
#include <cmath>

#define ZOOM_X  100.0
#define ZOOM_Y  100.0

Graph * Graph::m_Instance = NULL;

static gfloat f (gfloat x)
{
    return sin (x*8);
}

Graph::Graph( GtkWidget * window, uint32_t width, uint32_t height ) : m_parentWindow( window), m_Width( width ), m_Height( height ), m_Zoom( 100 ), m_MouseButtonPressed( false )
{
	directions.push_back( Axis( direction_left, -1, 0 ) );
	directions.push_back( Axis( direction_top, 0, 1 ) );
	directions.push_back( Axis( direction_right, 0, 1 ) );
	directions.push_back( Axis( direction_bottom, -1, 0 ) );

	createWidget();
}

Graph::Graph( GtkWidget * window ) : m_parentWindow( window), m_Width( 10 ), m_Height( 10 ), m_Zoom( 100 ), m_MouseButtonPressed( false )
{
	directions.push_back( Axis( direction_left, -1, 0 ) );
	directions.push_back( Axis( direction_top, 0, 1 ) );
	directions.push_back( Axis( direction_right, 0, 1 ) );
	directions.push_back( Axis( direction_bottom, -1, 0 ) );

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
	gtk_container_add(GTK_CONTAINER(m_parentWindow), m_GtkFixed);
	gtk_widget_show(m_GtkFixed);

	m_GtkObject = gtk_drawing_area_new ();

//	gtk_container_add (GTK_CONTAINER (m_parentWindow), m_GtkObject);
	gtk_fixed_put (GTK_FIXED (m_GtkFixed), m_GtkObject, 0, 0);

	gtk_drawing_area_size( (GtkDrawingArea*) m_GtkObject, m_Width, m_Height );

	gtk_widget_show( m_GtkObject );

	GdkRectangle da;            /* GtkDrawingArea size */
	gint unused = 0;

    /* Determine GtkDrawingArea dimensions */
    gdk_window_get_geometry (m_GtkObject->window,
            &da.x,
            &da.y,
            &da.width,
            &da.height,
            &unused);

    printf("da.width=%d, da.height=%d\n", da.width, da.height);fflush( stdout );

    m_Center_X = m_Width / 2.0;
    m_Center_Y = m_Height / 2.0;

	g_signal_connect( G_OBJECT (m_GtkObject), "expose-event", G_CALLBACK (on_expose_event), this);

	g_signal_connect (G_OBJECT (m_GtkObject), "motion-notify-event", G_CALLBACK(on_motion_event), this);

	g_signal_connect (G_OBJECT (m_GtkObject), "motion-notify-event", G_CALLBACK(on_motion_event), this);
	g_signal_connect (G_OBJECT (m_GtkObject), "button-press-event", G_CALLBACK(on_button_press_event), this);
	g_signal_connect (G_OBJECT (m_GtkObject), "button-release-event", G_CALLBACK(on_button_release_event), this);
	g_signal_connect (G_OBJECT (m_GtkObject), "scroll-event", G_CALLBACK(on_scroll_event), this);

	gtk_widget_set_events (m_GtkObject, gtk_widget_get_events (m_GtkObject) | GDK_BUTTON_PRESS_MASK | GDK_POINTER_MOTION_MASK | GDK_BUTTON_RELEASE_MASK | GDK_SCROLL_MASK);

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
	m_Points.push_back( Point( x, y ) );
	on_expose_event( m_GtkObject, NULL, this );
}

gboolean Graph::on_scroll_event (GtkWidget *widget, GdkEventScroll *event, gpointer user_data)
{
	printf("on_scroll_event state=%d\n", event->direction);fflush( stdout );

	Graph * lpThis = (Graph*)user_data;

	switch( event->direction )
	{
		case GDK_SCROLL_UP:
				lpThis->m_Zoom -= 5;
			break;
		case GDK_SCROLL_DOWN:
				lpThis->m_Zoom += 5;
			break;
	}

	on_expose_event( lpThis->m_GtkObject, NULL, lpThis );

	return FALSE;
}

gboolean Graph::on_motion_event (GtkWidget *widget, GdkEventMotion *event, gpointer user_data)
{
	Graph * lpThis = (Graph*)user_data;

	if( false == lpThis->m_MouseButtonPressed )
	{
		return FALSE;
	}

//printf("lpThis->m_ButtonPressCoord_X=%d > event->x=%.2f\n", lpThis->m_ButtonPressCoord_X, event->x);fflush(stdout);

	if( lpThis->m_ButtonPressCoord_X > event->x )
	{
		lpThis->m_Center_X -= 2;
	}
	else if( lpThis->m_ButtonPressCoord_X < event->x )
	{
		lpThis->m_Center_X +=2 ;
	}

	if( lpThis->m_ButtonPressCoord_Y > event->y )
	{
		lpThis->m_Center_Y -= 2;
	}
	else if( lpThis->m_ButtonPressCoord_Y < event->y )
	{
		lpThis->m_Center_Y +=2 ;
	}

	lpThis->m_ButtonPressCoord_X = event->x;
	lpThis->m_ButtonPressCoord_Y = event->y;

	on_expose_event( lpThis->m_GtkObject, NULL, lpThis );

	return FALSE;
}

gboolean Graph::on_button_press_event (GtkWidget *widget, GdkEventButton *event, gpointer user_data)
{
	printf("on_button_press_event\n");fflush( stdout );

	Graph * lpThis = (Graph*)user_data;

	lpThis->m_MouseButtonPressed = true;

	printf("event->x=%.2f\n", event->x);fflush(stdout);

	lpThis->m_ButtonPressCoord_X = event->x;
	lpThis->m_ButtonPressCoord_Y = event->y;

	return FALSE;
}

gboolean Graph::on_button_release_event (GtkWidget *widget, GdkEventButton *event, gpointer user_data)
{
	printf("on_button_release_event\n");fflush( stdout );

	Graph * lpThis = (Graph*)user_data;

	lpThis->m_MouseButtonPressed = false;

	return FALSE;
}


void draw_dot(cairo_t *cr, double x, double y)
{
	double w=0.05;//line with, 2*radius

	cairo_set_line_width (cr, w);
	cairo_set_line_cap( cr, CAIRO_LINE_CAP_ROUND); /* Round dot*/
	cairo_move_to (cr, x, y); cairo_line_to (cr, x, y);/* a very short line is a dot */
	cairo_stroke (cr);
}

gboolean Graph::on_expose_event (GtkWidget *widget, GdkEventExpose *event, gpointer user_data)
{
//	printf("on_expose_event\n");fflush( stdout );

	Graph * lpThis = (Graph*)user_data;

    cairo_t *cr = gdk_cairo_create (widget->window);
    cairo_surface_t * cs = cairo_get_group_target(cr);

    gdouble dx = 1.0, dy = 1.0; /* Pixels between each point */
    gdouble i, clip_x1 = 0.0, clip_y1 = 0.0, clip_x2 = 0.0, clip_y2 = 0.0;


    /* Define a clipping zone to improve performance */
//    cairo_rectangle (cr,
//            event->area.x,
//            event->area.y,
//            event->area.width,
//            event->area.height);
//    cairo_clip (cr);



//    printf("x=%d, y=%d, width=%d, height=%d\n", da.x, da.y, da.width, da.height);fflush(stdout);

    /* Draw on a black background */
    cairo_set_source_rgb (cr, 0.0, 0.0, 0.0);
    cairo_paint (cr);
//
//    /* Change the transformation matrix */
    cairo_translate( cr, lpThis->m_Center_X, lpThis->m_Center_Y );
//    cairo_translate (cr, da.width / 2, da.height / 2);
    cairo_scale (cr, (double)lpThis->m_Zoom, (double)(-1) * lpThis->m_Zoom);
//
//    /* Determine the data points to calculate (ie. those in the clipping zone */
    cairo_device_to_user_distance (cr, &dx, &dy);
    cairo_clip_extents (cr, &clip_x1, &clip_y1, &clip_x2, &clip_y2);
    cairo_set_line_width (cr, dx);
    cairo_set_line_cap( cr, CAIRO_LINE_CAP_ROUND );
//
//    cairo_move_to (cr, 0.0, 0);
//
//    cairo_set_source_rgba (cr, 1.0, 1.0, 0.0, 1.0);

//    cairo_select_font_face(cr, "Purisa",
//          CAIRO_FONT_SLANT_NORMAL,
//          CAIRO_FONT_WEIGHT_BOLD);
//    cairo_set_font_size(cr, 0.3);



    /* Draws x and y axis */
    cairo_set_source_rgba (cr, 0.0, 1.0, 0.0, 1.0);

//    printf("clip_x1=%.2f, clip_y1=%.2f, clip_x2=%.2f, clip_y2=%.2f\n", clip_x1, clip_y1, clip_x2, clip_y2);fflush(stdout);

    cairo_move_to (cr, clip_x1, 0.0);
    cairo_line_to (cr, clip_x2, 0.0);
    cairo_move_to (cr, 0.0, clip_y1);
    cairo_line_to (cr, 0.0, clip_y2);
    cairo_stroke (cr);

    const uint8_t actualTotalDegreeCount = (int)( std::abs( clip_x2 ) + std::abs( clip_x2 ) ) / 0.5;
    const uint8_t maxVisibleDegreeCount = 10;//std::abs( (int)clip_x2 ) + std::abs( (int)clip_x2 );
    const double degreeStep = actualTotalDegreeCount / maxVisibleDegreeCount;

//    printf("actualTotalDegreeCount=%d\n", actualTotalDegreeCount);fflush(stdout);

    for( uint8_t degree_i = 0 ; degree_i < maxVisibleDegreeCount ; degree_i++ )
    {
    	cairo_move_to (cr, ((int)clip_x1) + degree_i * degreeStep,   0.1);
		cairo_line_to (cr, ((int)clip_x1) + degree_i * degreeStep, - 0.1);

		char strDegree[BUFSIZ] = { '\0' };
		sprintf( strDegree, "%.2f", ((int)clip_x1) + degree_i * degreeStep );
		cairo_move_to (cr, ((int)clip_x1) + degree_i * degreeStep,   0.2);
		cairo_set_font_size(cr, 0.060);

		cairo_show_text(cr, strDegree);
    }
    cairo_stroke (cr);

    /* Link each data point */
//    for (i = clip_x1; i < clip_x2; i += dx)
//    {
////        cairo_line_to (cr, i, f (i) - 0.1);
//        cairo_line_to (cr, i, f (i));
////        cairo_line_to (cr, i, f (i) + 0.1 );
//    }

//
    /* Draw the curve */
    cairo_set_source_rgba (cr, 1.0, 0.0, 0.0, 1.0);
    cairo_stroke (cr);
//
//    /* Link each data point */
//	for (i = clip_x1; i < clip_x2; i += dx)
//		cairo_line_to (cr, i, f2 (i));
//
//	/* Draw the curve */
//	cairo_set_source_rgba (cr, 0.0, 0.0, 1.0, 1.0);
//	cairo_stroke (cr);

//	cairo_rotate( cr, sin(M_PI*180/180));

//	cairo_move_to (cr, 0.0, 0.0);

//	cairo_set_source_rgba (cr, 1.0, 1.0, 1.0, 1.0);

//	cairo_matrix_t matrix;
//	cairo_matrix_init_identity (&matrix);
//	cairo_matrix_rotate(&matrix, M_PI*180.0/180.0);
//	cairo_matrix_invert (&matrix);
//	cairo_set_font_matrix(cr, &matrix);
//	cairo_show_text( cr, "Hello" );

//	cairo_surface_write_to_png( cs, "jhgj");

    cairo_set_source_rgba (cr, 1.0, 1.0, 1.0, 1.0);

	for( uint32_t point_i = 0 ; point_i < lpThis->m_Points.size() ; point_i++ )
	{
//		cairo_rectangle( cr, lpThis->m_Points[point_i].m_X, lpThis->m_Points[point_i].m_Y, 0.1, 0.1 );

//		cairo_arc (cr, 0.5, 0.5, 1.0, 2*M_PI, 2*M_PI);
//		cairo_fill (cr);

		draw_dot( cr, lpThis->m_Points[point_i].m_X, lpThis->m_Points[point_i].m_Y );
		cairo_line_to (cr, lpThis->m_Points[point_i].m_X, lpThis->m_Points[point_i].m_Y );
		cairo_stroke( cr );
	}

	if( lpThis->m_Points.size() > 0 )
	{
		cairo_move_to (cr, lpThis->m_Points[0].m_X, lpThis->m_Points[0].m_Y );
		for( uint32_t point_i = 1 ; point_i < lpThis->m_Points.size() ; point_i++ )
		{
			cairo_line_to (cr, lpThis->m_Points[point_i].m_X, lpThis->m_Points[point_i].m_Y );
			cairo_move_to (cr, lpThis->m_Points[point_i].m_X, lpThis->m_Points[point_i].m_Y );
			cairo_stroke( cr );
		}
	}

    cairo_destroy (cr);

    return FALSE;
}
