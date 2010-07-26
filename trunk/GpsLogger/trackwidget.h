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

#ifndef TRACKWIDGET_H_
#define TRACKWIDGET_H_

#include <cstdlib>
#include <gtkmm-2.4/gtkmm.h>
#include "colorrenderrer.h"
#include "connector.h"
#include "functions.h"
#include "trackwindow.h"

using namespace std;

class TrackWidget: public Gtk::ScrolledWindow {
public:
    // Constructor & Destructor
    TrackWidget();
    virtual ~TrackWidget();
    // Getters & Setters
    void setFirst ( LogEntry* );
    // Action calls
    void AddTrack();
    void AutoTrack();
    void askTrack ( int, int*, int* );
    // Signals
    sigc::signal<void> signalChange();
    sigc::signal<void, bool> signalLock();
private:
    // Structs & Classes
    class TrackModelColumns: public Gtk::TreeModelColumnRecord {
    public:
        TrackModelColumns() {
            add ( id );
            add ( color );
            add ( name );
            add ( time1 );
            add ( time2 );
            add ( dateStr );
            add ( timeStr1 );
            add ( timeStr2 );
            add ( firstEntry );
            add ( count );
            add ( length );
        }
        Gtk::TreeModelColumn<int> id;
        Gtk::TreeModelColumn<int> color;
        Gtk::TreeModelColumn<Glib::ustring> name;
        Gtk::TreeModelColumn<time_t> time1;
        Gtk::TreeModelColumn<time_t> time2;
        Gtk::TreeModelColumn<Glib::ustring> dateStr;
        Gtk::TreeModelColumn<Glib::ustring> timeStr1;
        Gtk::TreeModelColumn<Glib::ustring> timeStr2;
        Gtk::TreeModelColumn<LogEntry*> firstEntry;
        Gtk::TreeModelColumn<int> count;
        Gtk::TreeModelColumn<int> length;
    };
    // Variables & Objects
    LogEntry* first;
    int lastId, selectedId;
    TrackModelColumns modelColumns;
    Glib::RefPtr<Gtk::ListStore> listStore;
    Glib::RefPtr<Gtk::TreeModelSort> sortedModel;
    ColorCellRenderer cellRenderer;
    // Functions
    void relocateAll();
    // Widgets
    Gtk::TreeView treeView;
    Gtk::Menu menu;
    // Widget signals
    void onButtonPress ( GdkEventButton* );
    void onMenuEdit();
    void onMenuDelete();
    // Windows
    TrackWindow trackWindow;
    // Window signals
    void onEditDone ( bool, int, string, int, long, long );
    // Signals
    sigc::signal<void> change;
    sigc::signal<void, bool> lock;
};

#endif /*TRACKWIDGET_H_*/
