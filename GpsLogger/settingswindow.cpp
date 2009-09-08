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

#include "settingswindow.h"

SettingsWindow::SettingsWindow() {
    // Self cofigure
    set_title ( "Settings" );
    set_resizable ( false );
    set_deletable ( false );
    // Variables & Objects
    connector = NULL;
    // Widgets
    Gtk::VBox *box = get_vbox();
    box->set_spacing ( 5 );
    label1.set_label ( "Memory usage" );
    box->pack_start ( label1, Gtk::PACK_SHRINK );
    box->pack_start ( progress1, Gtk::PACK_SHRINK );
    check1.set_label ( "Enable data logging" );
    box->pack_start ( check1, Gtk::PACK_SHRINK );
    box->pack_start ( separator1, Gtk::PACK_SHRINK );
    label2.set_label ( "When memory is full" );
    box->pack_start ( label2, Gtk::PACK_SHRINK );
    box->pack_start ( box1, Gtk::PACK_SHRINK );
    Gtk::RadioButton::Group group1 = radio1.get_group();
    radio1.set_label ( "stop logging" );
    box1.pack_start ( radio1, Gtk::PACK_EXPAND_WIDGET );
    radio2.set_group ( group1 );
    radio2.set_label ( "revrite oldest" );
    box1.pack_start ( radio2, Gtk::PACK_EXPAND_WIDGET );
    box->pack_start ( box2, Gtk::PACK_SHRINK );
    button1.set_label ( "Clear now" );
    button1.signal_clicked().connect ( sigc::mem_fun ( *this, &SettingsWindow::onButtonClear ) );
    box2.pack_start ( button1, Gtk::PACK_SHRINK );
    box->pack_start ( separator2, Gtk::PACK_SHRINK );
    label3.set_label ( "Logging" );
    box->pack_start ( label3, Gtk::PACK_SHRINK );
    box->pack_start ( box3, Gtk::PACK_SHRINK );
    check2.set_label ( "log every" );
    box3.pack_start ( check2, Gtk::PACK_SHRINK );
    box3.pack_start ( entry1, Gtk::PACK_SHRINK );
    label4.set_label ( "seconds" );
    box3.pack_start ( label4, Gtk::PACK_SHRINK );
    box->pack_start ( box4, Gtk::PACK_SHRINK );
    check3.set_label ( "log every" );
    box4.pack_start ( check3, Gtk::PACK_SHRINK );
    box4.pack_start ( entry2, Gtk::PACK_SHRINK );
    label5.set_label ( "meters" );
    box4.pack_start ( label5, Gtk::PACK_SHRINK );
    Gtk::HButtonBox *buttonBox = get_action_area();
    button2.set_label ( "OK" );
    button2.signal_clicked().connect ( sigc::mem_fun ( *this, &SettingsWindow::onButtonOk ) );
    buttonBox->pack_start ( button2, Gtk::PACK_EXPAND_WIDGET );
    button3.set_label ( "Storno" );
    button3.signal_clicked().connect ( sigc::mem_fun ( *this, &SettingsWindow::onButtonStorno ) );
    buttonBox->pack_start ( button3, Gtk::PACK_EXPAND_WIDGET );
    // Finally
    show_all_children();
}

SettingsWindow::~SettingsWindow() {
}

void SettingsWindow::setConnector ( Connector *connector ) {
    this->connector = connector;
}

void SettingsWindow::doShow() {
    int total, left;
    connector->getSectors ( total, left );
    progress1.set_fraction ( 1 - ( float ) left / total );
    progress1.set_text ( itos ( ( 1 - ( float ) left / total ) *100 ) + " %" );
    bool datalog = connector->getDatalog();
    check1.set_active ( datalog );
    bool fifo = connector->getFifo();
    if ( fifo ) {
        radio2.set_active ( true );
    } else {
        radio1.set_active ( true );
    }
    int time = connector->getTime();
    check2.set_active ( time != 0 );
    entry1.set_text ( itos ( time ) );
    int distance = connector->getDistance();
    check3.set_active ( distance != 0 );
    entry2.set_text ( itos ( distance ) );
    show();
}

sigc::signal<void> SettingsWindow::signalDone() {
    return done;
}

void SettingsWindow::onButtonClear() {
    connector->doClear();
    progress1.set_fraction ( 0 );
    progress1.set_text ( "Memory was cleared" );
}

void SettingsWindow::onButtonOk() {
    bool datalog = check1.get_active();
    connector->setDatalog ( datalog );
    bool fifo = radio2.get_active();
    connector->setFifo ( fifo );
    int time = atoi ( entry1.get_text().c_str() );
    if ( check2.get_active() == false ) {
        time = 0;
    }
    connector->setTime ( time );
    int distance = atoi ( entry2.get_text().c_str() );
    if ( check3.get_active() == false ) {
        distance = 0;
    }
    connector->setDistance ( distance );
    connector->doWrite();
    hide();
    done();
}

void SettingsWindow::onButtonStorno() {
    hide();
    done();
}
