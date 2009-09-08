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

#include "timewindow.h"

CameraRow::CameraRow ( string cameraName, int timeShift, CameraRow *next ) :
        adjustment1 ( 0, 0, 24, 1, 1, 0 ),
        spin1 ( adjustment1 ),
        adjustment2 ( 0, 0, 59, 1, 1, 0 ),
        spin2 ( adjustment2 ),
        adjustment3 ( 0, 0, 59, 1, 1, 0 ),
        spin3 ( adjustment3 ) {
    // Self cofigure
    set_border_width ( 5 );
    set_size_request ( 400, -1 );
    // Variables & Objects
    this->next = next;
    listStore = Gtk::ListStore::create ( modelColumns );
    Gtk::TreeModel::Row row1 = * ( listStore->append() );
    row1[modelColumns.value] = 1;
    row1[modelColumns.title] = "+";
    Gtk::TreeModel::Row row2 = * ( listStore->append() );
    row2[modelColumns.value] = -1;
    row2[modelColumns.title] = "-";
    // Widgets
    label1.set_alignment ( Gtk::ALIGN_LEFT );
    label1.set_label ( cameraName );
    pack_start ( label1, Gtk::PACK_EXPAND_WIDGET );
    combo.set_size_request ( 50, 25 );
    combo.pack_start ( modelColumns.title );
    combo.set_model ( listStore );
    if ( timeShift >= 0 ) {
        combo.set_active ( row1 );
    } else {
        combo.set_active ( row2 );
    }
    pack_start ( combo, Gtk::PACK_SHRINK );
    int hours = abs ( timeShift ) / 3600;
    spin1.set_size_request ( 50, -1 );
    spin1.set_value ( hours );
    pack_start ( spin1, Gtk::PACK_SHRINK );
    label2.set_label ( ":" );
    pack_start ( label2, Gtk::PACK_SHRINK );
    int mins = ( abs ( timeShift ) % 3600 ) / 60;
    spin2.set_size_request ( 50, -1 );
    spin2.set_value ( mins );
    pack_start ( spin2, Gtk::PACK_SHRINK );
    label3.set_label ( ":" );
    pack_start ( label3, Gtk::PACK_SHRINK );
    int secs = abs ( timeShift ) % 60;
    spin3.set_size_request ( 50, -1 );
    spin3.set_value ( secs );
    pack_start ( spin3, Gtk::PACK_SHRINK );
}

CameraRow::~CameraRow() {
}

int CameraRow::getShift() {
    int shift = 0;
    shift += spin1.get_value_as_int() * 3600;
    shift += spin2.get_value_as_int() * 60;
    shift += spin3.get_value_as_int();
    Gtk::TreeModel::iterator iter = combo.get_active();
    if ( iter ) {
        Gtk::TreeModel::Row row = *iter;
        int sign = row[modelColumns.value];
        shift *= sign;
    }
    return shift;
}

CameraRow* CameraRow::getNext() {
    return next;
}

TimeWindow::TimeWindow() {
    // Self cofigure
    set_title ( "Time shift settings" );
    set_resizable ( false );
    set_deletable ( false );
    // Variables & Objects
    first = NULL;
    count = 0;
    // Widgets
    Gtk::VBox *box = get_vbox();
    box->set_size_request ( -1, 200 );
    Gtk::HButtonBox *buttonBox = get_action_area();
    button1.set_label ( "OK" );
    button1.signal_clicked().connect ( sigc::mem_fun ( *this, &TimeWindow::onButtonOk ) );
    buttonBox->pack_start ( button1, Gtk::PACK_EXPAND_WIDGET );
    button2.set_label ( "Storno" );
    button2.signal_clicked().connect ( sigc::mem_fun ( *this, &TimeWindow::onButtonStorno ) );
    buttonBox->pack_start ( button2, Gtk::PACK_EXPAND_WIDGET );
    // Finally
    show_all_children();
}

TimeWindow::~TimeWindow() {
    // Variables & Objects
    clearCameras();
}

void TimeWindow::clearCameras() {
    CameraRow *next = first;
    while ( next != NULL ) {
        CameraRow *temp = next->getNext();
        Gtk::VBox *box = get_vbox();
        box->remove ( *next );
        delete ( next );
        next = temp;
    }
    first = NULL;
    count = 0;
}

void TimeWindow::addCamera ( string cameraName, int timeShift ) {
    CameraRow *temp = first;
    first = new CameraRow ( cameraName, timeShift, temp );
    count++;
    Gtk::VBox *box = get_vbox();
    box->pack_start ( *first, Gtk::PACK_SHRINK );
    show_all_children();
}

sigc::signal<void, bool, int*> TimeWindow::signalDone() {
    return done;
}

void TimeWindow::onButtonOk() {
    CameraRow *next = first;
    int *times = ( int* ) malloc ( count * sizeof ( int ) );
    times += count;
    for ( int i = 0; i < count; i++ ) {
        times--;
        *times = next->getShift();
        next = next->getNext();
    }
    done ( true, times );
    free ( times );
}

void TimeWindow::onButtonStorno() {
    done ( false, NULL );
}
