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

#ifndef PHOTOWINDOW_H_
#define PHOTOWINDOW_H_

#include <gtkmm-2.4/gtkmm.h>
#include "functions.h"

using namespace std;

class PhotoArea : public Gtk::DrawingArea {
public:
    // Constructor & Destructor
    PhotoArea ( const char* );
    virtual ~PhotoArea();
private:
    // Variables & Objects
    Cairo::RefPtr<Cairo::ImageSurface> image;
    // Functions
    bool on_expose_event ( GdkEventExpose* );
};

class PhotoBox : public Gtk::HBox {
public:
    // Constructor & Destructor
    PhotoBox ( string, const char*, PhotoBox* );
    virtual ~PhotoBox();
    // Getters & Setters
    PhotoBox* getNext();
private:
    // Variables & Objects
    PhotoBox *next;
    // Widgets
    PhotoArea photoArea;
    Gtk::Alignment alignment1;
    Gtk::Label label1;
    Gtk::Alignment alignment2;
    Gtk::Label label2;
};

class PhotoWindow : public Gtk::Window {
public:
    // Constructor & Destructor
    PhotoWindow();
    virtual ~PhotoWindow();
    // Getters & Setters
    void clearPhotos();
    void addPhoto ( string, const char* );
    // Action calls
    void doShow();
private:
    // Variables & Objects
    PhotoBox *first;
    // Widgets
    Gtk::ScrolledWindow scrolledWindow;
    Gtk::VBox box;
};

#endif /*PHOTOWINDOW_H_*/
