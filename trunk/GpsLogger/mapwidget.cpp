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
    firstClickable = NULL;
    width = 0;
    height = 0;
    latRange = 0;
    lonRange = 0;
    minLat = 0;
    minLon = 0;
    maxLat = 0;
    maxLon = 0;
    buttonDown = false;
    mapGenerator.setDone ( &done );
    // Widgets
    scale.set_draw_value ( false );
    scale.set_range ( 0, 10 );
    scale.set_inverted ( true );
    scale.set_update_policy ( Gtk::UPDATE_DELAYED );
    scaleCon = scale.signal_value_changed().connect ( sigc::mem_fun ( *this, &MapWidget::onZoomChange ) );
    attach ( scale, 0, 1, 0, 1, Gtk::FILL, Gtk::SHRINK );
    vScrollBar.set_inverted ( true );
    vScrollBar.set_update_policy ( Gtk::UPDATE_DELAYED );
    vScrollCon = vScrollBar.signal_value_changed().connect ( sigc::mem_fun ( *this, &MapWidget::onPosChange ) );
    attach ( vScrollBar, 1, 2, 1, 2, Gtk::SHRINK, Gtk::FILL );
    hScrollBar.set_update_policy ( Gtk::UPDATE_DELAYED );
    hScrollCon = hScrollBar.signal_value_changed().connect ( sigc::mem_fun ( *this, &MapWidget::onPosChange ) );
    attach ( hScrollBar, 0, 1, 2, 3, Gtk::FILL, Gtk::SHRINK );
    mapArea.signalResize().connect ( sigc::mem_fun ( *this, &MapWidget::onResize ) );
    mapArea.set_events ( Gdk::BUTTON_PRESS_MASK | Gdk::BUTTON_RELEASE_MASK );
    mapArea.signal_button_press_event().connect ( sigc::mem_fun ( *this, &MapWidget::onButtonPress ) );
    mapArea.signal_button_release_event().connect ( sigc::mem_fun ( *this, &MapWidget::onButtonRelease ) );
    mapArea.signal_scroll_event().connect ( sigc::mem_fun ( *this, &MapWidget::onScroll ) );
    attach ( mapArea, 0, 1, 1, 2 );
}

MapWidget::~MapWidget() {
}

void MapWidget::setFirstVisible ( LogEntry *firstVisible ) {
    this->firstVisible = firstVisible;
    LogEntry* next = firstVisible;
    if ( next != NULL ) {
        minLat = next->latitude;
        minLon = next->longitude;
        maxLat = next->latitude;
        maxLon = next->longitude;
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
    } else {
        minLat = 0;
        minLon = 0;
        maxLat = 0;
        maxLon = 0;
    }
    setupScroll();
    doRedraw();
    updateScroll();
}

void MapWidget::doRedraw() {
    Cairo::RefPtr<Cairo::ImageSurface> image;
    Frame frame = mapGenerator.doGenerate ( image );
    latRange = frame.maxLat - frame.minLat;
    lonRange = frame.maxLon - frame.minLon;
    drawOverlay ( frame );
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

MapWidget::Clickable::Clickable ( double x, double y, int photoIndex ) {
    this->x = x;
    this->y = y;
    this->photoIndex = photoIndex;
    subitem = NULL;
    next = NULL;
}

MapWidget::Clickable::~Clickable() {
    if ( subitem != NULL ) {
        delete ( subitem );
        subitem = NULL;
    }
    if ( next != NULL ) {
        delete ( next );
        next = NULL;
    }
}

void MapWidget::setupScroll () {
    double zoom = mapGenerator.doCalcZoom ( minLat, minLon, maxLat, maxLon );
    double latitude = ( minLat + maxLat ) / 2;
    double longitude = ( minLon + maxLon ) / 2;
    scaleCon.disconnect();
    vScrollCon.disconnect();
    hScrollCon.disconnect();
    scale.set_value ( zoom );
    mapGenerator.setZoom ( scale.get_value() );
    vScrollBar.get_adjustment()->set_page_size ( 0 );
    vScrollBar.set_range ( -90, 90 );
    vScrollBar.set_value ( latitude );
    hScrollBar.get_adjustment()->set_page_size ( 0 );
    hScrollBar.set_range ( -180, 180 );
    hScrollBar.set_value ( longitude );
    mapGenerator.setCenter ( vScrollBar.get_value(), hScrollBar.get_value() );
    scaleCon = scale.signal_value_changed().connect ( sigc::mem_fun ( *this, &MapWidget::onZoomChange ) );
    vScrollCon = vScrollBar.signal_value_changed().connect ( sigc::mem_fun ( *this, &MapWidget::onPosChange ) );
    hScrollCon = hScrollBar.signal_value_changed().connect ( sigc::mem_fun ( *this, &MapWidget::onPosChange ) );
}

void MapWidget::updateScroll() {
    if ( ( minLat != 0 ) || ( minLon != 0 ) || ( maxLat != 0 ) || ( maxLon != 0 ) ) {
        vScrollBar.set_sensitive ( true );
        vScrollBar.get_adjustment()->set_page_size ( 0 );
        vScrollBar.set_range ( minLat, maxLat + latRange );
        vScrollBar.get_adjustment()->set_page_size ( latRange );
        hScrollBar.set_sensitive ( true );
        hScrollBar.get_adjustment()->set_page_size ( 0 );
        hScrollBar.set_range ( minLon, maxLon + lonRange );
        hScrollBar.get_adjustment()->set_page_size ( lonRange );
    } else {
        vScrollBar.set_sensitive ( false );
        vScrollBar.get_adjustment()->set_page_size ( 0 );
        vScrollBar.set_range ( -90, 90 );
        hScrollBar.set_sensitive ( false );
        hScrollBar.get_adjustment()->set_page_size ( 0 );
        hScrollBar.set_range ( -180, 180 );
    }
}

void MapWidget::drawOverlay ( Frame frame ) {
    Cairo::RefPtr<Cairo::ImageSurface> image = Cairo::ImageSurface::create ( Cairo::FORMAT_ARGB32, width, height );
    Cairo::RefPtr<Cairo::Context> context = Cairo::Context::create ( image );
    LogEntry* next = firstVisible;
    double xKoef = width / ( frame.maxLon - frame.minLon );
    double yKoef = height / ( frame.maxLat - frame.minLat );
    if ( firstClickable != NULL ) {
        delete ( firstClickable );
        firstClickable = NULL;
    }
    bool lastOut = false;
    double lastX = 0;
    double lastY = 0;
    int color = -1;
    int count = 0;
    while ( next != NULL ) {
        double x = ( next->longitude - frame.minLon ) * xKoef;
        double y = ( frame.maxLat - next->latitude ) * yKoef;
        if ( ( abs ( x - lastX ) > 1 ) || ( abs ( y - lastY ) > 1 ) ) {
            if ( ( x >= 0 ) && ( x < width ) && ( y >= 0 ) && ( y < height ) ) {
                if ( lastOut ) {
                    context->move_to ( lastX, lastY );
                    lastOut = false;
                }
                context->line_to ( x, y );
            } else {
                if ( lastOut == false ) {
                    context->line_to ( x, y );
                }
                lastOut = true;
            }
            lastX = x;
            lastY = y;
        }
        if ( count > 0 ) {
            count--;
            if ( count == 0 ) {
                prepareColor ( context, color );
                context->stroke();
                if ( lastOut == false ) {
                    context->move_to ( x, y );
                }
                color = -1;
            }
        }
        if ( next->track != -1 ) {
            prepareColor ( context, color );
            context->stroke();
            if ( lastOut == false ) {
                context->move_to ( x, y );
            }
            askTrack ( next->track, &color, &count );
        }
        if ( next->photo != -1 ) {
            addClickable ( x, y, next->photo );
        }
        next = next->nextVisible;
    }
    prepareColor ( context, color );
    context->stroke();
    drawClickables ( context );
    overlay = image;
}

void MapWidget::prepareColor ( Cairo::RefPtr<Cairo::Context> context, int color ) {
    if ( color != -1 ) {
        int palete[] = { 0xED, 0xD4, 0x00, 0xF5, 0x79, 0x00, 0xC1, 0x7D, 0x11, 0x73, 0xd2, 0x16, 0x34, 0x65, 0xA4, 0x75, 0x50, 0x7B, 0xCC, 0x00, 0x00 };
        int index = ( color % 7 ) * 3;
        double rChannel = ( double ) palete[index + 0] / 255;
        double gChannel = ( double ) palete[index + 1] / 255;
        double bChannel = ( double ) palete[index + 2] / 255;
        context->set_source_rgb ( rChannel, gChannel, bChannel );
        context->set_line_width ( 2 );
    } else {
        context->set_source_rgb ( 0, 0, 0 );
        context->set_line_width ( 2 );
    }
}

void MapWidget::addClickable ( double x, double y, int photoIndex ) {
    Clickable *newOne = new Clickable ( x, y, photoIndex );
    if ( firstClickable == NULL ) {
        firstClickable = newOne;
    } else {
        Clickable *next = firstClickable;
        bool inSublist = false;
        while ( next != NULL ) {
            if ( ( inSublist == false ) && ( abs ( x - next->x ) < 32 ) && ( abs ( y - next->y ) < 32 ) ) {
                if ( next->subitem == NULL ) {
                    next->subitem = newOne;
                    next = NULL;
                } else {
                    next = next->subitem;
                }
                inSublist = true;
            } else {
                if ( next->next == NULL ) {
                    next->next = newOne;
                    next = NULL;
                } else {
                    next = next->next;
                }
            }
        }
    }
}

void MapWidget::drawClickables ( Cairo::RefPtr<Cairo::Context> context ) {
    Clickable *next = firstClickable;
    Cairo::RefPtr<Cairo::ImageSurface> image = Cairo::ImageSurface::create_from_png ( "image.png" );
    while ( next != NULL ) {
        context->save();
        context->set_source ( image, next->x - 16, next->y - 16 );
        context->rectangle ( next->x - 16, next->y - 16, 32, 32 );
        context->clip();
        context->paint();
        context->restore();
        next = next->next;
    }
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
    updateScroll();
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
    updateScroll();
}

bool MapWidget::onButtonPress ( GdkEventButton *event ) {
    int button = event->button;
    int x = event->x;
    int y = event->y;
    if ( button == 1 ) {
        Clickable *next = firstClickable;
        while ( next != NULL ) {
            if ( ( abs ( x - next->x ) < 16 ) && ( abs ( y - next->y ) < 16 ) ) {
                Clickable *next2 = next->subitem;
                int count = 1;
                while ( next2 != NULL ) {
                    count++;
                    next2 = next2->next;
                }
                int *photos = ( int* ) malloc ( ( count + 1 ) * sizeof ( int ) );
                photos[0] = next->photoIndex;
                next2 = next->subitem;
                int *photos2 = photos;
                while ( next2 != NULL ) {
                    photos2++;
                    *photos2 = next2->photoIndex;
                    next2 = next2->next;
                }
                photos2++;
                *photos2 = -1;
                photoClick ( photos );
                free ( photos );
                return true;
            }
            next = next->next;
        }
        buttonDown = true;
        downX = x;
        downY = y;
        downLat = vScrollBar.get_value();
        downLon = hScrollBar.get_value();
        mapArea.get_window()->set_cursor ( Gdk::Cursor ( Gdk::FLEUR ) );
    }
    return true;
}

bool MapWidget::onButtonRelease ( GdkEventButton *event ) {
    int button = event->button;
    int x = event->x;
    int y = event->y;
    if ( buttonDown ) {
        if ( button == 1 ) {
            buttonDown = false;
            vScrollCon.disconnect();
            hScrollCon.disconnect();
            vScrollBar.set_value ( downLat + ( double ) ( y - downY ) * latRange / height );
            hScrollBar.set_value ( downLon + ( double ) ( downX - x ) * lonRange / width );
            vScrollCon = vScrollBar.signal_value_changed().connect ( sigc::mem_fun ( *this, &MapWidget::onPosChange ) );
            hScrollCon = hScrollBar.signal_value_changed().connect ( sigc::mem_fun ( *this, &MapWidget::onPosChange ) );
            onPosChange();
        }
    }
    mapArea.get_window()->set_cursor ( Gdk::Cursor ( Gdk::LEFT_PTR ) );
    return true;
}

bool MapWidget::onScroll ( GdkEventScroll *event ) {
    int direction = event->direction;
    double value = scale.get_value();
    if ( direction == GDK_SCROLL_UP ) {
        scale.set_value ( value - 0.1 );
    }
    if ( direction == GDK_SCROLL_DOWN ) {
        scale.set_value ( value + 0.1 );
    }
    return true;
}
