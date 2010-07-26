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

#ifndef MAPWIDGET_H_
#define MAPWIDGET_H_

#include <iomanip>
#include <gtkmm-2.4/gtkmm.h>
#include "connector.h"
#include "mapgenerator.h"

using namespace std;

class MapArea: public Gtk::DrawingArea {
public:
    // Constructor & Destructor
    MapArea();
    virtual ~MapArea();
    // Getters & Setters
    Cairo::RefPtr<Cairo::ImageSurface> getImage();
    void setImage ( Cairo::RefPtr<Cairo::ImageSurface> );
    // Signals
    sigc::signal<void, int, int> signalResize();
private:
    // Variables & Objects
    int width, height;
    Cairo::RefPtr<Cairo::ImageSurface> image;
    // Functions
    bool on_expose_event ( GdkEventExpose* );
    // Signals
    sigc::signal<void, int, int> resize;
};

class MapWidget: public Gtk::Table {
public:
    // Constructor & Destructor
    MapWidget();
    virtual ~MapWidget();
    // Getters & Setters
    void setFirstVisible ( LogEntry* );
    // Action calls
    void doRedraw();
    // Signals
    sigc::signal<void, int, int*, int*> signalAskTrack();
    sigc::signal<void, int*> signalPhotoClick();
private:
    // Variables & Objects
    LogEntry *firstVisible;
    int width, height;
    MapGenerator mapGenerator;
    Cairo::RefPtr<Cairo::ImageSurface> overlay;
    // Functions
    Cairo::RefPtr<Cairo::ImageSurface> drawOverlay ( Frame );
    // Dispatchers
    Glib::Dispatcher done;
    // Dispatcher signals
    void onDone();
    // Widgets
    Gtk::HScale scale;
    Gtk::VScrollbar vScrollBar;
    Gtk::HScrollbar hScrollBar;
    MapArea mapArea;
    // Widget signals
    void onZoomChange();
    void onPosChange();
    void onResize ( int, int );
    bool onButtonPress ( GdkEventButton* );
    bool onMotionNotify ( GdkEventMotion* );
    bool onButtonRelease ( GdkEventButton* );
    bool onScroll ( GdkEventScroll* );
    // Signals
    sigc::signal<void, int, int*, int*> askTrack;
    sigc::signal<void, int*> photoClick;
};

#endif /*MAPWIDGET_H_*/
