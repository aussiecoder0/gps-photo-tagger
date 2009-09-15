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

#ifndef DAYVIEW_H_
#define DAYVIEW_H_

#include <gtkmm.h>
#include "connector.h"
#include "functions.h"

using namespace std;

class DayView : public Gtk::TreeView {
public:
    // Constructor & Destructor
    DayView();
    virtual ~DayView();
    // Getters & Setters
    void clear();
    void append ( LogEntry* );
    // Action calls
    void save ( const char* );
    LogEntry* load ( const char* );
    // Signals
    sigc::signal<void, LogEntry*> signalChange();
private:
    // Structs & Classes
    class DayModelColumns : public Gtk::TreeModelColumnRecord {
    public:
        DayModelColumns() {
            add ( enabled );
            add ( date );
            add ( first );
            add ( count );
            add ( last );
        }
        Gtk::TreeModelColumn<bool> enabled;
        Gtk::TreeModelColumn<Glib::ustring> date;
        Gtk::TreeModelColumn<LogEntry*> first;
        Gtk::TreeModelColumn<int> count;
        Gtk::TreeModelColumn<LogEntry*> last;
    };
    // Variables & Objects
    LogEntry *first;
    LogEntry *firstVisible;
    DayModelColumns modelColumns;
    Glib::RefPtr<Gtk::ListStore> listStore;
    // Functions
    void update();
    // Widget signals
    void onToggled ( const Glib::ustring& path );
    // Signals
    sigc::signal<void, LogEntry*> change;
};

#endif /*DAYVIEW_H_*/
