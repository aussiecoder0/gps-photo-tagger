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

#ifndef TIMEWINDOW_H_
#define TIMEWINDOW_H_

#include <gtkmm.h>

using namespace std;

class CameraRow : public Gtk::HBox {
public:
    // Constructor & Destructor
    CameraRow ( string, int, CameraRow* );
    virtual ~CameraRow();
    // Getters & Setters
    int getShift();
    CameraRow* getNext();
private:
    // Structs & Classes
    class TimeModelColumns : public Gtk::TreeModel::ColumnRecord {
    public:
        TimeModelColumns() {
            add ( value );
            add ( title );
        }
        Gtk::TreeModelColumn<int> value;
        Gtk::TreeModelColumn<Glib::ustring> title;
    };
    // Variables & Objects
    CameraRow *next;
    TimeModelColumns modelColumns;
    Glib::RefPtr<Gtk::ListStore> listStore;
    // Widgets
    Gtk::Label label1;
    Gtk::ComboBox combo;
    Gtk::Adjustment adjustment1;
    Gtk::SpinButton spin1;
    Gtk::Label label2;
    Gtk::Adjustment adjustment2;
    Gtk::SpinButton spin2;
    Gtk::Label label3;
    Gtk::Adjustment adjustment3;
    Gtk::SpinButton spin3;
};

class TimeWindow : public Gtk::Dialog {
public:
    // Constructor & Destructor
    TimeWindow();
    virtual ~TimeWindow();
    // Getters & Setters
    void clearCameras();
    void addCamera ( string, int );
    // Signals
    sigc::signal<void, bool, int*> signalDone();
private:
    // Variables & Objects
    CameraRow *first;
    int count;
    // Widgets
    Gtk::Button button1, button2;
    // Widget signals
    void onButtonOk();
    void onButtonStorno();
    // Signals
    sigc::signal<void, bool, int*> done;
protected:
    virtual bool on_delete_event ( GdkEventAny* G_GNUC_UNUSED ) {
        return true;
    }
};

#endif /*TIMEWINDOW_H_*/
