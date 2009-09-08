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
#include <gtkmm.h>
#include "connector.h"

using namespace std;

class MapWidget : public Gtk::Table {
public:
    // Constructor & Destructor
    MapWidget();
    virtual ~MapWidget();
    // Getters & Setters
    void setFirstVisible ( LogEntry* );
    // Action calls
    void doRedraw();
    // Signals
    sigc::signal<void, int*> signalPhotoClick();
private:
    // Structs & Classes
    class Clickable {
    public:
        Clickable ( double, double, int );
        virtual ~Clickable();
        double x, y;
        int photoIndex;
        Clickable *subitem;
        Clickable *next;
    };
    class ExternalDrawingArea : public Gtk::DrawingArea {
    public:
        ExternalDrawingArea();
        virtual ~ExternalDrawingArea();
        sigc::signal<void, int, int, Cairo::RefPtr<Cairo::Context> > signalExpose();
    private:
        bool on_expose_event ( GdkEventExpose* );
        sigc::signal<void, int, int, Cairo::RefPtr<Cairo::Context> > expose;
    };
    // Variables & Objects
    LogEntry *firstVisible;
    int width, height;
    int zoom;
    double latitude, longitude;
    bool lastOut;
    double lastLat, lastLon;
    Clickable *firstClickable;
    bool buttonDown;
    int downX, downY;
    double downLat, downLon;
    // Functions
    void updateView();
    void drawLatLine ( int, double, Cairo::RefPtr<Cairo::Context> );
    void drawLonLine ( int, double, Cairo::RefPtr<Cairo::Context> );
    void drawPoint ( double, double, Cairo::RefPtr<Cairo::Context> );
    void addClickable ( double, double, int );
    void drawClickables ( Cairo::RefPtr<Cairo::Context> );
    // Widgets
    Gtk::HScale scale;
    Gtk::VScrollbar vScrollBar;
    Gtk::HScrollbar hScrollBar;
    ExternalDrawingArea mapArea;
    // Widget signals
    void onZoomChange();
    void onPosChange();
    void onExpose ( int, int, Cairo::RefPtr<Cairo::Context> );
    bool onButtonPress ( GdkEventButton* );
    bool onMotionNotify ( GdkEventMotion* );
    bool onButtonRelease ( GdkEventButton* );
    bool onScroll ( GdkEventScroll* );
    // Signals
    sigc::signal<void, int*> photoClick;
};

#endif /*MAPWIDGET_H_*/
