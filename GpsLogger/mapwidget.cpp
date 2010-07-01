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

MapWidget::MapWidget() {
    // Self cofigure
    Gtk::Table ( 2, 3 );
    // Variables & Objects
    firstVisible = NULL;
    zoom = 1;
    latitude = 0;
    longitude = 0;
    firstClickable = NULL;
    buttonDown = false;
    // Widgets
    scale.set_range ( 10, 500 );
    scale.set_value_pos ( Gtk::POS_LEFT );
    scale.set_draw_value ( true );
    scale.signal_value_changed().connect ( sigc::mem_fun ( *this, &MapWidget::onZoomChange ) );
    scale.set_value ( 30 );
    attach ( scale, 0, 2, 0, 1, Gtk::FILL, Gtk::SHRINK );
    vScrollBar.set_inverted ( true );
    vScrollBar.signal_value_changed().connect ( sigc::mem_fun ( *this, &MapWidget::onPosChange ) );
    attach ( vScrollBar, 1, 2, 1, 2, Gtk::SHRINK, Gtk::FILL );
    hScrollBar.signal_value_changed().connect ( sigc::mem_fun ( *this, &MapWidget::onPosChange ) );
    attach ( hScrollBar, 0, 1, 2, 3, Gtk::FILL, Gtk::SHRINK );
    mapArea.signalExpose().connect ( sigc::mem_fun ( *this, &MapWidget::onExpose ) );
    mapArea.set_events ( Gdk::BUTTON_PRESS_MASK | Gdk::POINTER_MOTION_MASK | Gdk::BUTTON_RELEASE_MASK );
    mapArea.signal_button_press_event().connect ( sigc::mem_fun ( *this, &MapWidget::onButtonPress ) );
    mapArea.signal_motion_notify_event().connect ( sigc::mem_fun ( *this, &MapWidget::onMotionNotify ) );
    mapArea.signal_button_release_event().connect ( sigc::mem_fun ( *this, &MapWidget::onButtonRelease ) );
    mapArea.signal_scroll_event().connect ( sigc::mem_fun ( *this, &MapWidget::onScroll ) );
    attach ( mapArea, 0, 1, 1, 2 );
}

MapWidget::~MapWidget() {
}

void MapWidget::setFirstVisible ( LogEntry *first ) {
    firstVisible = first;
    updateView();
    mapArea.queue_draw();
}

void MapWidget::doRedraw() {
    mapArea.queue_draw();
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

MapWidget::ExternalDrawingArea::ExternalDrawingArea() {
}

MapWidget::ExternalDrawingArea::~ExternalDrawingArea() {
}

sigc::signal<void, int, int, Cairo::RefPtr<Cairo::Context> > MapWidget::ExternalDrawingArea::signalExpose() {
    return expose;
}

bool MapWidget::ExternalDrawingArea::on_expose_event ( GdkEventExpose *event ) {
    Gtk::Allocation allocation = get_allocation();
    int width = allocation.get_width() - 1;
    int height = allocation.get_height() - 1;
    Glib::RefPtr<Gdk::Window> window = get_window();
    Cairo::RefPtr<Cairo::Context> context = window->create_cairo_context();
    context->rectangle ( 0, 0, width, height );
    context->clip();
    context->set_line_width ( 1.0 );
    context->rectangle ( 0, 0, width, height );
    context->set_source_rgb ( 1, 1, 1 );
    context->fill_preserve();
    expose ( width, height, context );
    return true;
}

void MapWidget::updateView() {
    if ( firstVisible != NULL ) {
        double minLat = 360;
        double maxLat = 0;
        double minLon = 360;
        double maxLon = 0;
        LogEntry* next = firstVisible;
        while ( next != NULL ) {
            if ( next->latitude < minLat ) {
                minLat = next->latitude;
            }
            if ( next->latitude > maxLat ) {
                maxLat = next->latitude;
            }
            if ( next->longitude < minLon ) {
                minLon = next->longitude;
            }
            if ( next->longitude > maxLon ) {
                maxLon = next->longitude;
            }
            next = next->nextVisible;
        }
        this->minLat = minLat;
        this->maxLat = maxLat;
        this->minLon = minLon;
        this->maxLon = maxLon;
        updateScrollBars();
        double latitude = ( minLat + maxLat ) / 2;
        double longitude = ( minLon + maxLon ) / 2;
        vScrollBar.set_value ( latitude );
        hScrollBar.set_value ( longitude );
        int zoom1 = width / ( maxLon - minLon );
        int zoom2 = height / ( maxLat - minLat );
        if ( zoom1 < zoom2 ) {
            scale.set_value ( pow ( zoom1, 0.5 ) );
        } else {
            scale.set_value ( pow ( zoom2, 0.5 ) );
        }
    }
}

void MapWidget::updateScrollBars() {
    Gtk::Adjustment *adjustment;
    double pagesize;
    adjustment = vScrollBar.get_adjustment();
    pagesize = ( double ) height / zoom;
    if ( pagesize != 0 ) {
        adjustment->set_page_size ( pagesize );
        vScrollBar.set_range ( minLat, maxLat + pagesize );
    } else {
        adjustment->set_page_size ( 1 );
        vScrollBar.set_range ( 0, 1 );
    }
    adjustment = hScrollBar.get_adjustment();
    pagesize = ( double ) width / zoom;
    if ( pagesize != 0 ) {
        adjustment->set_page_size ( pagesize );
        hScrollBar.set_range ( minLon, maxLon + pagesize );
    } else {
        adjustment->set_page_size ( 1 );
        hScrollBar.set_range ( 0, 1 );
    }
}

void MapWidget::drawLatLine ( int deg, double min, Cairo::RefPtr<Cairo::Context> context ) {
    double latitude = deg + ( double ) min / 60;
    double y = ( height / 2 ) + ( this->latitude - latitude ) * zoom;
    context->move_to ( 0, y );
    context->line_to ( width, y );
    context->move_to ( 2, y - 2 );
    stringstream out;
    int deg2 = abs ( deg );
    int min2 = abs ( min );
    int sec2 = ( abs ( min ) - min2 ) * 60 + 0.5;
    if ( sec2 == 60 ) {
        min2++;
        sec2 = 0;
    }
    out << deg2 << "°";
    out << setw ( 2 ) << setfill ( '0' ) << right << min2 << "'";
    if ( sec2 != 0 ) {
        out << setw ( 2 ) << setfill ( '0' ) << right << sec2 << "\"";
    }
    context->show_text ( out.str() );
}

void MapWidget::drawLonLine ( int deg, double min, Cairo::RefPtr<Cairo::Context> context ) {
    double longitude = deg + ( double ) min / 60;
    double x = ( width / 2 ) + ( longitude - this->longitude ) * zoom;
    context->move_to ( x, 0 );
    context->line_to ( x, height );
    context->move_to ( x + 2, height - 2 );
    stringstream out;
    int deg2 = abs ( deg );
    int min2 = abs ( min );
    int sec2 = ( abs ( min ) - min2 ) * 60 + 0.5;
    if ( sec2 == 60 ) {
        min2++;
        sec2 = 0;
    }
    out << deg2 << "°";
    out << setw ( 2 ) << setfill ( '0' ) << right << min2 << "'";
    if ( sec2 != 0 ) {
        out << setw ( 2 ) << setfill ( '0' ) << right << sec2 << "\"";
    }
    context->show_text ( out.str() );
}

void MapWidget::drawPoint ( double latitude, double longitude, Cairo::RefPtr<Cairo::Context> context ) {
    double x = ( width / 2 ) + ( longitude - this->longitude ) * zoom;
    double y = ( height / 2 ) + ( this->latitude - latitude ) * zoom;
    if ( ( x >= 0 ) && ( x < width ) && ( y >= 0 ) && ( y < height ) ) {
        if ( lastOut ) {
            double lastX = width / 2 + ( lastLon - this->longitude ) * zoom;
            double lastY = height / 2 + ( this->latitude - lastLat ) * zoom;
            context->move_to ( lastX, lastY );
        }
        context->line_to ( x, y );
        lastOut = false;
    } else {
        if ( lastOut == false ) {
            context->line_to ( x, y );
        }
        lastLat = latitude;
        lastLon = longitude;
        lastOut = true;
    }
}

void MapWidget::addClickable ( double latitude, double longitude, int photoIndex ) {
    double x = ( width / 2 ) + ( longitude - this->longitude ) * zoom;
    double y = ( height / 2 ) + ( this->latitude - latitude ) * zoom;
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

void MapWidget::onZoomChange() {
    zoom = pow ( scale.get_value(), 2 );
    updateScrollBars();
    mapArea.queue_draw();
}

void MapWidget::onPosChange() {
    latitude = vScrollBar.get_value();
    longitude = hScrollBar.get_value();
    mapArea.queue_draw();
}

void MapWidget::onExpose ( int width, int height, Cairo::RefPtr<Cairo::Context> context ) {
    this->width = width;
    this->height = height;
    if ( firstClickable != NULL ) {
        delete ( firstClickable );
        firstClickable = NULL;
    }
    context->set_source_rgb ( 0.2, 0.2, 0.2 );
    context->set_line_width ( 0.5 );
    double distance = ( double ) 100 * 60 / zoom;
    double step = 60;
    if ( step / 2 > distance ) {
        step = step / 2;
        if ( step / 3 > distance ) {
            step = step / 3;
            if ( step / 2 > distance ) {
                step = step / 2;
                if ( step / 5 > distance ) {
                    step = step / 5;
                    if ( step / 2 > distance ) {
                        step = step / 2;
                        if ( step / 3 > distance ) {
                            step = step / 3;
                        }
                    }
                }
            }
        }
    }
    int minLat = ( int ) ( this->latitude - ( double ) height / zoom / 2 - 1 );
    int maxLat = ( int ) ( this->latitude + ( double ) height / zoom / 2 + 1 );
    for ( int deg = minLat; deg <= maxLat; deg++ ) {
        if ( deg >= 0 ) {
            for ( double min = 0; min < 60; min += step ) {
                drawLatLine ( deg, min, context );
            }
        }
        if ( deg <= 0 ) {
            for ( double min = 0; min > -60; min -= step ) {
                if ( ( deg != 0 ) || ( min != 0 ) ) {
                    drawLatLine ( deg, min, context );
                }
            }
        }
    }
    int minLon = ( int ) ( this->longitude - ( double ) width / zoom / 2 - 1 );
    int maxLon = ( int ) ( this->longitude + ( double ) width / zoom / 2 + 1 );
    for ( int deg = minLon; deg <= maxLon; deg++ ) {
        if ( deg >= 0 ) {
            for ( double min = 0; min < 60; min += step ) {
                drawLonLine ( deg, min, context );
            }
        }
        if ( deg <= 0 ) {
            for ( double min = 0; min > -60; min -= step ) {
                if ( ( deg != 0 ) || ( min != 0 ) ) {
                    drawLonLine ( deg, min, context );
                }
            }
        }
    }
    context->set_source_rgb ( 0, 0, 0 );
    context->set_line_width ( 1 );
    LogEntry* next = firstVisible;
    if ( next != NULL ) {
        lastLat = next->latitude;
        lastLon = next->longitude;
        lastOut = true;
    }
    while ( next != NULL ) {
        drawPoint ( next->latitude, next->longitude, context );
        if ( next->photo != -1 ) {
            addClickable ( next->latitude, next->longitude, next->photo );
        }
        next = next->nextVisible;
    }
    context->stroke();
    drawClickables ( context );
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
                break;
            }
            next = next->next;
        }
        buttonDown = true;
        downX = x;
        downY = y;
        downLat = latitude;
        downLon = longitude;
    }
    return true;
}

bool MapWidget::onMotionNotify ( GdkEventMotion *event ) {
    int x = event->x;
    int y = event->y;
    if ( buttonDown ) {
        vScrollBar.set_value ( downLat + ( double ) ( y - downY ) / zoom );
        hScrollBar.set_value ( downLon + ( double ) ( downX - x ) / zoom );
    }
    return true;
}

bool MapWidget::onButtonRelease ( GdkEventButton *event ) {
    int button = event->button;
    if ( button == 1 ) {
        buttonDown = false;
    }
    return true;
}

bool MapWidget::onScroll ( GdkEventScroll *event ) {
    int direction = event->direction;
    int value = scale.get_value();
    if ( direction == GDK_SCROLL_UP ) {
        scale.set_value ( value + 10 );
    }
    if ( direction == GDK_SCROLL_DOWN ) {
        scale.set_value ( value - 10 );
    }
    return true;
}
