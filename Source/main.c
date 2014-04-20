/**
 * @file GUI.cpp
 * @brief OpenCV Demo.
 * @date Created on: 15/03/2010
 * @author tArKi
 * @version 1.0
 */

#include <gtk/gtk.h>
#include <cv.h>
#include <highgui.h>
#include <stdio.h>

gint t = 0;
IplImage* opencvImage;
GdkPixbuf* pix;


static gboolean on_draw_event(GtkWidget *widget, cairo_t *cr, gpointer user_data)
{    

	pix = gdk_pixbuf_new_from_data((guchar*) opencvImage->imageData,
			GDK_COLORSPACE_RGB, FALSE, opencvImage->depth, opencvImage->width,
			opencvImage->height, (opencvImage->widthStep), NULL, NULL);

	gdk_cairo_set_source_pixbuf (cr,pix,0,0);

	return FALSE;
}

int main(int argc, char *argv[])
{
	GtkWidget *window;
	GtkWidget *drawing_area;
	gtk_init (&argc, &argv);

	char *file_name = argv[1];
	if (argv[1] == NULL)
	{
		file_name = "/home/bijan/Pictures/nenit_by_kiko11-d5ka4zo (copy).jpg";
	}
	//Change this path in order to load any other image.

	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);

	gtk_window_set_title(GTK_WINDOW (window), "RESET DEMO");
	g_signal_connect (G_OBJECT (window), "destroy",
			G_CALLBACK (gtk_main_quit), NULL);

	/* Now add a child widget to the aspect frame */
	drawing_area = gtk_drawing_area_new();

	/* Ask for a 200x200 window, but the AspectFrame will give us a 200x100
	 * window since we are forcing a 2x1 aspect ratio */
	//gtk_widget_set_size_request(drawing_area, opencvImage->width, opencvImage->height);
	//gtk_container_add(GTK_CONTAINER (window), drawing_area);
	//gtk_widget_show(drawing_area);


	//g_signal_connect(G_OBJECT(drawing_area), "draw", 
	//		G_CALLBACK(on_draw_event), NULL); 

	gtk_widget_show(window);
	gtk_main();
	return 0;
}
