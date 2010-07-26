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

#include "mapwidget.h"

MapArea::MapArea() {
    // Variables & Objects
    width = 0;
    height = 0;
}

MapArea::~MapArea() {
}

Cairo::RefPtr<Cairo::ImageSurface> MapArea::getImage() {
    return image;
}

void MapArea::setImage ( Cairo::RefPtr<Cairo::ImageSurface> image ) {
    this->image = image;
    queue_draw();
}

sigc::signal<void, int, int> MapArea::signalResize() {
    return resize;
}

bool MapArea::on_expose_event ( GdkEventExpose *event ) {
    Gtk::Allocation allocation = get_allocation();
    int width = allocation.get_width() - 1;
    int height = allocation.get_height() - 1;
    Glib::RefPtr<Gdk::Window> window = get_window();
    Cairo::RefPtr<Cairo::Context> context = window->create_cairo_context();
    context->rectangle ( 0, 0, width, height );
    context->clip();
    if ( image != NULL ) {
        context->set_source ( image, 0, 0 );
        context->paint();
    }
    if ( ( this->width != width ) || ( this->height != height ) ) {
        this->width = width;
        this->height = height;
        resize ( width, height );
    }
    return true;
}

MapWidget::MapWidget() {
    // Self cofigure
    Gtk::Table ( 2, 3 );
    // Dispatchers
    done.connect ( sigc::mem_fun ( *this, &MapWidget::onDone ) );
    // Variables & Objects
    firstVisible = NULL;
    width = 0;
    height = 0;
    mapGenerator.setDone ( &done );
    // Widgets
    scale.set_range ( 10, 500 );
    scale.set_value_pos ( Gtk::POS_LEFT );
    scale.set_draw_value ( true );
    scale.signal_value_changed().connect ( sigc::mem_fun ( *this, &MapWidget::onZoomChange ) );
    scale.set_value ( 500 );
    attach ( scale, 0, 2, 0, 1, Gtk::FILL, Gtk::SHRINK );
    vScrollBar.set_inverted ( true );
    vScrollBar.signal_value_changed().connect ( sigc::mem_fun ( *this, &MapWidget::onPosChange ) );
    attach ( vScrollBar, 1, 2, 1, 2, Gtk::SHRINK, Gtk::FILL );
    hScrollBar.signal_value_changed().connect ( sigc::mem_fun ( *this, &MapWidget::onPosChange ) );
    attach ( hScrollBar, 0, 1, 2, 3, Gtk::FILL, Gtk::SHRINK );
    mapArea.signalResize().connect ( sigc::mem_fun ( *this, &MapWidget::onResize ) );
    mapArea.set_events ( Gdk::BUTTON_PRESS_MASK | Gdk::POINTER_MOTION_MASK | Gdk::BUTTON_RELEASE_MASK );
    mapArea.signal_button_press_event().connect ( sigc::mem_fun ( *this, &MapWidget::onButtonPress ) );
    mapArea.signal_motion_notify_event().connect ( sigc::mem_fun ( *this, &MapWidget::onMotionNotify ) );
    mapArea.signal_button_release_event().connect ( sigc::mem_fun ( *this, &MapWidget::onButtonRelease ) );
    mapArea.signal_scroll_event().connect ( sigc::mem_fun ( *this, &MapWidget::onScroll ) );
    attach ( mapArea, 0, 1, 1, 2 );

    vScrollBar.set_range ( 49.0, 51.0 );
    vScrollBar.set_value ( 50.07449 );
    hScrollBar.set_range ( 12.0, 14.0 );
    hScrollBar.set_value ( 12.37085 );

}

MapWidget::~MapWidget() {
}

void MapWidget::setFirstVisible ( LogEntry *firstVisible ) {
    this->firstVisible = firstVisible;
    LogEntry* next = firstVisible;
    double minLat = 0;
    double minLon = 0;
    double maxLat = 0;
    double maxLon = 0;
    while ( next != NULL ) {
        if ( next->latitude < minLat ) {
            minLat = next->latitude;
        }
        if ( next->longitude < minLon ) {
            minLon = next->longitude;
        }
        if ( next->latitude > maxLat ) {
            maxLat = next->latitude;
        }
        if ( next->longitude > maxLon ) {
            maxLon = next->longitude;
        }
        next = next->nextVisible;
    }
    //vScrollBar.set_range ( minLon, maxLon );
    //hScrollBar.set_range ( minLat, maxLat );

    doRedraw();
}

void MapWidget::doRedraw() {
    Cairo::RefPtr<Cairo::ImageSurface> image;
    Frame frame = mapGenerator.doGenerate ( image );
    overlay = drawOverlay ( frame );
    Cairo::RefPtr<Cairo::Context> context = Cairo::Context::create ( image );
    context->set_source ( overlay, 0, 0 );
    context->paint();
    mapArea.setImage ( image );
}

sigc::signal<void, int, int*, int*> MapWidget::signalAskTrack() {
    return askTrack;
}

sigc::signal<void, int*> MapWidget::signalPhotoClick() {
    return photoClick;
}

Cairo::RefPtr<Cairo::ImageSurface> MapWidget::drawOverlay ( Frame frame ) {
    Cairo::RefPtr<Cairo::ImageSurface> image = Cairo::ImageSurface::create ( Cairo::FORMAT_ARGB32, width, height );
    Cairo::RefPtr<Cairo::Context> context = Cairo::Context::create ( image );
    LogEntry* next = firstVisible;
    double xKoef = width / ( frame.maxLon - frame.minLon );
    double yKoef = height / ( frame.maxLat - frame.minLat );
    bool lastOut = false;
    double lastX = 0;
    double lastY = 0;
    while ( next != NULL ) {
        double x = ( next->longitude - frame.minLon ) * xKoef;
        double y = ( next->latitude - frame.minLat ) * yKoef;
        if ( ( abs ( x - lastX ) > 1 ) || ( abs ( y - lastY ) > 1 ) ) {
            if ( ( x >= 0 ) && ( x < width ) && ( y >= 0 ) && ( y < height ) ) {
                if ( lastOut ) {
                    context->move_to ( lastX, lastY );
                    lastOut = false;
                }
                context->line_to ( x, y );
            } else {
                lastOut = true;
            }
            lastX = x;
            lastY = y;
        }
        next = next->nextVisible;
    }
    context->set_source_rgb ( 0, 0, 0 );
    context->set_line_width ( 1 );
    context->stroke();
    return image;
}

void MapWidget::onDone() {
    Cairo::RefPtr<Cairo::ImageSurface> image;
    Frame frame = mapGenerator.doTake ( image );
    Cairo::RefPtr<Cairo::Context> context = Cairo::Context::create ( image );
    context->set_source ( overlay, 0, 0 );
    context->paint();
    mapArea.setImage ( image );
}

void MapWidget::onZoomChange() {
    mapGenerator.setZoom ( scale.get_value() );
    doRedraw();
}

void MapWidget::onPosChange() {
    mapGenerator.setCenter ( vScrollBar.get_value(), hScrollBar.get_value() );
    doRedraw();
}

void MapWidget::onResize ( int width, int height ) {
    this->width = width;
    this->height = height;
    mapGenerator.setSize ( width, height );
    doRedraw();
}

bool MapWidget::onButtonPress ( GdkEventButton *event ) {

    return true;
}

bool MapWidget::onMotionNotify ( GdkEventMotion *event ) {

    return true;
}

bool MapWidget::onButtonRelease ( GdkEventButton *event ) {

    return true;
}

bool MapWidget::onScroll ( GdkEventScroll *event ) {

    return true;
}
