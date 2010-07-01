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

#include "trackwindow.h"

TimeRow::TimeRow() :
        adjustment1 ( 0, 0, 23, 1, 1, 0 ),
        spin1 ( adjustment1 ),
        adjustment2 ( 0, 0, 59, 1, 1, 0 ),
        spin2 ( adjustment2 ),
        adjustment3 ( 0, 0, 59, 1, 1, 0 ),
        spin3 ( adjustment3 ) {
    // Widgets
    spin1.set_size_request ( 50, -1 );
    pack_start ( spin1, Gtk::PACK_SHRINK );
    label1.set_label ( ":" );
    pack_start ( label1, Gtk::PACK_SHRINK );
    spin2.set_size_request ( 50, -1 );
    pack_start ( spin2, Gtk::PACK_SHRINK );
    label2.set_label ( ":" );
    pack_start ( label2, Gtk::PACK_SHRINK );
    spin3.set_size_request ( 50, -1 );
    pack_start ( spin3, Gtk::PACK_SHRINK );
}

TimeRow::~TimeRow() {
}

long TimeRow::getTime() {
    struct tm *timeinfo = localtime ( &time );
    timeinfo->tm_hour = spin1.get_value_as_int();
    timeinfo->tm_min = spin2.get_value_as_int();
    timeinfo->tm_sec = spin3.get_value_as_int();
    long newTime = mktime ( timeinfo );
    return newTime;
}

void TimeRow::setTime ( long time ) {
    this->time = time;
    struct tm *timeinfo = localtime ( &time );
    spin1.set_value ( timeinfo->tm_hour );
    spin2.set_value ( timeinfo->tm_min );
    spin3.set_value ( timeinfo->tm_sec );
}

TrackWindow::TrackWindow() :
        table ( 2, 5 ) {
    // Self cofigure
    set_title ( "GPS Photo Tagger" );
    set_resizable ( false );
    set_deletable ( false );
    // Variables & Objects
    listStore1 = Gtk::ListStore::create ( modelColumns1 );
    Gtk::TreeModel::Row row;
    for ( int i = 0; i < 7; i++ ) {
        row = * ( listStore1->append() );
        row[modelColumns1.color] = i;
        row[modelColumns1.label] = "Color " + itos ( i + 1 );
    }
    listStore2 = Gtk::ListStore::create ( modelColumns2 );
    // Widgets
    Gtk::VBox *box = get_vbox();
    table.set_border_width ( 5 );
    table.set_spacings ( 5 );
    box->pack_start ( table, Gtk::PACK_SHRINK );
    label1.set_size_request ( 100, -1 );
    label1.set_alignment ( Gtk::ALIGN_LEFT );
    label1.set_label ( "Track name:" );
    table.attach ( label1, 0, 1, 0, 1 );
    table.attach ( entry, 1, 2, 0, 1 );
    label2.set_alignment ( Gtk::ALIGN_LEFT );
    label2.set_label ( "Track color:" );
    table.attach ( label2, 0, 1, 1, 2 );
    combo1.pack_start ( cellRenderer, false );
    combo1.add_attribute ( cellRenderer, "color", modelColumns1.color );
    combo1.pack_start ( modelColumns1.label, true );
    combo1.set_model ( listStore1 );
    combo1.set_active ( listStore1->children().begin() );
    table.attach ( combo1, 1, 2, 1, 2 );
    label3.set_alignment ( Gtk::ALIGN_LEFT );
    label3.set_label ( "Trip date:" );
    table.attach ( label3, 0, 1, 2, 3 );
    combo2.pack_start ( modelColumns2.date );
    combo2.set_model ( listStore2 );
    combo2.signal_changed().connect ( sigc::mem_fun ( *this, &TrackWindow::onDayChange ) );
    table.attach ( combo2, 1, 2, 2, 3 );
    label4.set_alignment ( Gtk::ALIGN_LEFT );
    label4.set_label ( "Start time:" );
    table.attach ( label4, 0, 1, 3, 4 );
    table.attach ( startTime, 1, 2, 3, 4 );
    label5.set_alignment ( Gtk::ALIGN_LEFT );
    label5.set_label ( "Stop time:" );
    table.attach ( label5, 0, 1, 4, 5 );
    table.attach ( stopTime, 1, 2, 4, 5 );
    Gtk::ButtonBox *buttonBox = get_action_area();
    button1.set_label ( "OK" );
    button1.signal_clicked().connect ( sigc::mem_fun ( *this, &TrackWindow::onButtonOk ) );
    buttonBox->pack_start ( button1, Gtk::PACK_EXPAND_WIDGET );
    button2.set_label ( "Storno" );
    button2.signal_clicked().connect ( sigc::mem_fun ( *this, &TrackWindow::onButtonStorno ) );
    buttonBox->pack_start ( button2, Gtk::PACK_EXPAND_WIDGET );
    // Finally
    show_all_children();
}

TrackWindow::~TrackWindow() {
}

void TrackWindow::doShowAdd ( LogEntry* first ) {
    itemId = -1;
    entry.set_text ( "" );
    combo1.set_active ( listStore1->children().begin() );
    listStore2->clear();
    Gtk::TreeModel::Row row;
    LogEntry *next = first;
    LogEntry *last = NULL;
    struct tm *timeinfo;
    char buffer [11];
    while ( next != NULL ) {
        timeinfo = localtime ( &next->time );
        strftime ( buffer, 11, "%d.%m.%Y", timeinfo );
        row = * ( listStore2->append() );
        row[modelColumns2.date] = buffer;
        row[modelColumns2.first] = next;
        last = next;
        next = next->nextDay;
    }
    if ( last != NULL ) {
        combo2.set_active ( row );
        startTime.setTime ( last->time );
        next = last;
        while ( next != NULL ) {
            last = next;
            next = next->next;
        }
        stopTime.setTime ( last->time );
    }
    show();
    raise();
}

void TrackWindow::doShowEdit ( LogEntry* first, int itemId, string name, int color, long start, long stop ) {
    this->itemId = itemId;
    entry.set_text ( name );
    typedef Gtk::TreeModel::Children Children;
    Children children = listStore1->children();
    for ( Children::iterator iter = children.begin(); iter != children.end(); ++iter ) {
        Gtk::TreeModel::Row row = *iter;
        if ( row[modelColumns1.color] == color ) {
            combo1.set_active ( row );
            break;
        }
    }
    listStore2->clear();
    Gtk::TreeModel::Row row;
    LogEntry *next = first;
    struct tm *timeinfo;
    char buffer [11];
    timeinfo = localtime ( &start );
    int year = timeinfo->tm_year;
    int day = timeinfo->tm_yday;
    while ( next != NULL ) {
        timeinfo = localtime ( &next->time );
        strftime ( buffer, 11, "%d.%m.%Y", timeinfo );
        row = * ( listStore2->append() );
        row[modelColumns2.date] = buffer;
        row[modelColumns2.first] = next;
        next = next->nextDay;
        if ( ( timeinfo->tm_year == year ) && ( timeinfo->tm_yday == day ) ) {
            combo2.set_active ( row );
        }
    }
    startTime.setTime ( start );
    stopTime.setTime ( stop );
    show();
    raise();
}

sigc::signal<void, bool, int, string, int, long, long> TrackWindow::signalDone() {
    return done;
}

void TrackWindow::onDayChange() {
    Gtk::TreeModel::iterator iter = combo2.get_active();
    if ( iter ) {
        Gtk::TreeModel::Row row = *iter;
        LogEntry *next = row[modelColumns2.first];
        LogEntry *behind = next->nextDay;
        LogEntry *last = NULL;
        startTime.setTime ( next->time );
        while ( next != behind ) {
            last = next;
            next = next->next;
        }
        stopTime.setTime ( last->time );
    }

}

void TrackWindow::onButtonOk() {
    string name = entry.get_text();
    int color = 0;
    Gtk::TreeModel::iterator iter = combo1.get_active();
    if ( iter ) {
        Gtk::TreeModel::Row row = *iter;
        color = row[modelColumns1.color];
    }
    long start = startTime.getTime();
    long stop = stopTime.getTime();
    done ( true, itemId, name, color, start, stop );
}

void TrackWindow::onButtonStorno() {
    done ( false, -1, "", 0, 0, 0 );
}
