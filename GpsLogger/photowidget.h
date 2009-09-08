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

#ifndef PHOTOWIDGET_H_
#define PHOTOWIDGET_H_

#include <cstdlib>
#include <gtkmm.h>
#include <image.hpp>
#include <exif.hpp>
#include "connector.h"
#include "functions.h"
#include "photowindow.h"
#include "timewindow.h"

using namespace std;

struct PhotoItem {
    int id;
    bool hasPos;
    double latitude;
    double longitude;
    string path;
};

class PhotoWidget : public Gtk::ScrolledWindow {
public:
    // Constructor & Destructor
    PhotoWidget();
    virtual ~PhotoWidget();
    // Getters & Setters
    void setFirst ( LogEntry* );
    list<PhotoItem> getPhotos();
    void clearPhotos();
    void addPhoto ( const char* );
    // Action calls
    void showPhotos ( int* );
    void showSettings();
    // Signals
    sigc::signal<void> signalChange();
    sigc::signal<void> signalDone();
private:
    // Structs & Classes
    struct CameraTime {
        string cameraName;
        int timeShift;
    };
    class PhotoModelColumns : public Gtk::TreeModelColumnRecord {
    public:
        PhotoModelColumns() {
            add ( id );
            add ( cameraId );
            add ( time );
            add ( dateStr );
            add ( timeStr );
            add ( locationId );
            add ( entry );
            add ( latStr );
            add ( lonStr );
            add ( path );
        }
        Gtk::TreeModelColumn<int> id;
        Gtk::TreeModelColumn<int> cameraId;
        Gtk::TreeModelColumn<time_t> time;
        Gtk::TreeModelColumn<Glib::ustring> dateStr;
        Gtk::TreeModelColumn<Glib::ustring> timeStr;
        Gtk::TreeModelColumn<int> locationId;
        Gtk::TreeModelColumn<LogEntry*> entry;
        Gtk::TreeModelColumn<Glib::ustring> latStr;
        Gtk::TreeModelColumn<Glib::ustring> lonStr;
        Gtk::TreeModelColumn<Glib::ustring> path;
    };
    // Variables & Objects
    LogEntry* first;
    int lastId;
    PhotoModelColumns modelColumns;
    Glib::RefPtr<Gtk::ListStore> listStore;
    list<CameraTime> cameras;
    // Functions
    void relocateAll ( bool );
    void locatePhoto ( Gtk::TreeModel::Row );
    // Widgets
    Gtk::TreeView treeView;
    // Windows
    PhotoWindow photoWindow;
    TimeWindow timeWindow;
    // Window signals
    void onEditDone ( bool, int* );
    // Signals
    sigc::signal<void> change;
    sigc::signal<void> done;
};

#endif /*PHOTOWIDGET_H_*/
