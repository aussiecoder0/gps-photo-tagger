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

#ifndef PICASAWINDOWS_H_
#define PICASAWINDOWS_H_

#include <gtkmm.h>
#include "functions.h"

using namespace std;

struct AlbumItem {
    string title;
    string link;
};

class LoginWindow : public Gtk::Dialog {
public:
    // Constructor & Destructor
    LoginWindow();
    virtual ~LoginWindow();
    // Signals
    sigc::signal<void, bool, string, string, LoginWindow*> signalDone();
private:
    // Widgets
    Gtk::Table table;
    Gtk::Label label1, label2;
    Gtk::Entry entry1, entry2;
    Gtk::Button button1, button2;
    // Widget signals
    void onButtonOk();
    void onButtonStorno();
    // Signals
    sigc::signal<void, bool, string, string, LoginWindow*> done;
protected:
    virtual bool on_delete_event ( GdkEventAny* G_GNUC_UNUSED ) {
        return true;
    }
};

class SetupWindow : public Gtk::Dialog {
public:
    // Constructor & Destructor
    SetupWindow ( list<AlbumItem>, int );
    virtual ~SetupWindow();
    // Action calls
    void doIncrease();
    // Signals
    sigc::signal<void, bool, bool, string, bool, int, SetupWindow*> signalDone();
private:
    // Structs & Classes
    class AlbumModelColumns : public Gtk::TreeModel::ColumnRecord {
    public:
        AlbumModelColumns() {
            add ( title );
            add ( link );
        }
        Gtk::TreeModelColumn<Glib::ustring> title;
        Gtk::TreeModelColumn<Glib::ustring> link;
    };
    // Variables & Objects
    AlbumModelColumns modelColumns;
    Glib::RefPtr<Gtk::ListStore> listStore;
    int count, position;
    // Widgets
    Gtk::Frame frame1;
    Gtk::Table table1;
    Gtk::RadioButton radio1, radio2;
    Gtk::Entry entry;
    Gtk::ComboBox combo;
    Gtk::Frame frame2;
    Gtk::Table table2;
    Gtk::RadioButton radio3, radio4;
    Gtk::Frame frame3;
    Gtk::Table table3;
    Gtk::RadioButton radio5, radio6, radio7, radio8;
    Gtk::Frame frame4;
    Gtk::Table table4;
    Gtk::ProgressBar progressBar;
    Gtk::Button button1, button2;
    // Widget signals
    void onButtonOk();
    void onButtonStorno();
    // Signals
    sigc::signal<void, bool, bool, string, bool, int, SetupWindow*> done;
protected:
    virtual bool on_delete_event ( GdkEventAny* G_GNUC_UNUSED ) {
        return true;
    }
};

#endif /*PICASAWINDOWS_H_*/
