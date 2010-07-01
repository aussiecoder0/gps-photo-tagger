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

#ifndef TRACKWINDOW_H_
#define TRACKWINDOW_H_

#include <gtkmm.h>
#include "colorrenderrer.h"
#include "connector.h"
#include "functions.h"

using namespace std;

class TimeRow: public Gtk::HBox {
public:
    // Constructor & Destructor
    TimeRow();
    virtual ~TimeRow();
    // Getters & Setters
    long getTime();
    void setTime ( long );
private:
    // Variables & Objects
    long time;
    // Widgets
    Gtk::Adjustment adjustment1;
    Gtk::SpinButton spin1;
    Gtk::Label label1;
    Gtk::Adjustment adjustment2;
    Gtk::SpinButton spin2;
    Gtk::Label label2;
    Gtk::Adjustment adjustment3;
    Gtk::SpinButton spin3;
};

class TrackWindow: public Gtk::Dialog {
public:
    // Constructor & Destructor
    TrackWindow();
    virtual ~TrackWindow();
    // Action calls
    void doShowAdd ( LogEntry* );
    void doShowEdit ( LogEntry*, int, string, int, long, long );
    // Signals
    sigc::signal<void, bool, int, string, int, long, long> signalDone();
private:
    // Structs & Classes
    class ColorModelColumns : public Gtk::TreeModel::ColumnRecord {
    public:
        ColorModelColumns() {
            add ( color );
            add ( label );
        }
        Gtk::TreeModelColumn<int> color;
        Gtk::TreeModelColumn<Glib::ustring> label;
    };
    class DayModelColumns : public Gtk::TreeModel::ColumnRecord {
    public:
        DayModelColumns() {
            add ( date );
            add ( first );
        }
        Gtk::TreeModelColumn<Glib::ustring> date;
        Gtk::TreeModelColumn<LogEntry*> first;
    };
    // Variables & Objects
    int itemId;
    ColorModelColumns modelColumns1;
    DayModelColumns modelColumns2;
    Glib::RefPtr<Gtk::ListStore> listStore1;
    Glib::RefPtr<Gtk::ListStore> listStore2;
    ColorCellRenderer cellRenderer;
    // Widgets
    Gtk::Table table;
    Gtk::Label label1;
    Gtk::ComboBox combo1;
    Gtk::Label label2;
    Gtk::Entry entry;
    Gtk::Label label3;
    Gtk::ComboBox combo2;
    Gtk::Label label4;
    TimeRow startTime;
    Gtk::Label label5;
    TimeRow stopTime;
    Gtk::Button button1, button2;
    // Widget signals
    void onDayChange();
    void onButtonOk();
    void onButtonStorno();
    // Signals
    sigc::signal<void, bool, int, string, int, long, long> done;
protected:
    virtual bool on_delete_event ( GdkEventAny* G_GNUC_UNUSED ) {
        return true;
    }
};

#endif /*TRACKWINDOW_H_*/
