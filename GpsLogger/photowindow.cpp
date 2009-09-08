/*

	GPS Photo Tagger
	Copyright (C) 2009  Jakub Vaník <jakub.vanik@gmail.com>

	This program is free software; you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation; either version 2 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program; if not, write to the Free Software
	Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  US

*/

#include "photowindow.h"

PhotoArea::PhotoArea ( const char *path ) {
    // Variables & Objects
    Glib::RefPtr<Gdk::Pixbuf> pixbuf = Gdk::Pixbuf::create_from_file ( path );
    int width = pixbuf->get_width();
    int height = pixbuf->get_height();
    double scale = ( double ) 200 / width;
    double scale2 = ( double ) 150 / height;
    if ( scale2 < scale ) {
        scale = scale2;
    }
    image = Cairo::ImageSurface::create ( Cairo::FORMAT_RGB24, 200, 150 );
    Cairo::RefPtr<Cairo::Context> context = Cairo::Context::create ( image );
    context->scale ( scale, scale );
    int left = ( 200 / scale - width ) / 2;
    int top = ( 150 / scale - height ) / 2;
    Gdk::Cairo::set_source_pixbuf ( context, pixbuf, left, top );
    context->paint();
}

PhotoArea::~PhotoArea() {
}

bool PhotoArea::on_expose_event ( GdkEventExpose* ) {
    Glib::RefPtr<Gdk::Window> window = get_window();
    Cairo::RefPtr<Cairo::Context> context = window->create_cairo_context();
    context->set_source ( image, 0, 0 );
    context->paint();
    return true;
}

PhotoBox::PhotoBox ( string info, const char *path, PhotoBox *next ) :
        photoArea ( path ),
        alignment1 ( Gtk::ALIGN_LEFT, Gtk::ALIGN_TOP, 0, 0 ),
        alignment2 ( Gtk::ALIGN_LEFT, Gtk::ALIGN_TOP, 0, 0 ) {
    // Self cofigure
    set_size_request ( -1, 150 );
    set_border_width ( 2 );
    set_spacing ( 10 );
    // Variables & Objects
    this->next = next;
    // Widgets
    photoArea.set_size_request ( 200, 150 );
    pack_start ( photoArea, Gtk::PACK_SHRINK );
    pack_start ( alignment1, Gtk::PACK_SHRINK );
    label1.set_justify ( Gtk::JUSTIFY_LEFT );
    label1.set_label ( "ID:\nDate:\nTime:\nLatitude:\nLongitude:\n" );
    alignment1.add ( label1 );
    pack_start ( alignment2, Gtk::PACK_SHRINK );
    label2.set_label ( info );
    alignment2.add ( label2 );
}

PhotoBox::~PhotoBox() {
}

PhotoBox* PhotoBox::getNext() {
    return next;
}

PhotoWindow::PhotoWindow() {
    // Self cofigure
    set_title ( "GPS Photo Tagger" );
    set_resizable ( false );
    // Variables & Objects
    first = NULL;
    // Widgets
    scrolledWindow.set_size_request ( 400, 300 );
    scrolledWindow.set_policy ( Gtk::POLICY_NEVER, Gtk::POLICY_ALWAYS );
    scrolledWindow.set_border_width ( 2 );
    add ( scrolledWindow );
    box.set_spacing ( 2 );
    scrolledWindow.add ( box );
    // Finally
    show_all_children();
}

PhotoWindow::~PhotoWindow() {
    clearPhotos();
}

void PhotoWindow::clearPhotos() {
    PhotoBox *next = first;
    while ( next != NULL ) {
        PhotoBox *temp = next->getNext();
        box.remove ( *next );
        delete ( next );
        next = temp;
    }
    first = NULL;
    scrolledWindow.check_resize();
}

void PhotoWindow::addPhoto ( string info, const char *path ) {
    PhotoBox *temp = first;
    first = new PhotoBox ( info, path, temp );
    box.pack_start ( *first, Gtk::PACK_SHRINK );
    show_all_children();
}

void PhotoWindow::doShow() {
    show();
    raise();
}
