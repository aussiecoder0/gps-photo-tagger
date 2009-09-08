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

#include "picasawindows.h"

LoginWindow::LoginWindow() : table ( 2, 2 ) {
    // Self cofigure
    set_title ( "Google Account login" );
    set_resizable ( false );
    set_deletable ( false );
    // Widgets
    Gtk::VBox *box = get_vbox();
    table.set_border_width ( 5 );
    box->pack_start ( table, Gtk::PACK_SHRINK );
    label1.set_label ( "Username:" );
    table.attach ( label1, 0, 1, 0, 1 );
    table.attach ( entry1, 1, 2, 0, 1 );
    label2.set_label ( "Password:" );
    table.attach ( label2, 0, 1, 1, 2 );
    entry2.set_visibility ( false );
    table.attach ( entry2, 1, 2, 1, 2 );
    Gtk::HButtonBox *buttonBox = get_action_area();
    button1.set_label ( "Login" );
    button1.signal_clicked().connect ( sigc::mem_fun ( *this, &LoginWindow::onButtonOk ) );
    buttonBox->pack_start ( button1, Gtk::PACK_EXPAND_WIDGET );
    button2.set_label ( "Cancel" );
    button2.signal_clicked().connect ( sigc::mem_fun ( *this, &LoginWindow::onButtonStorno ) );
    buttonBox->pack_start ( button2, Gtk::PACK_EXPAND_WIDGET );
    // Finally
    show_all_children();
    show();
}

LoginWindow::~LoginWindow() {
    hide();
}

sigc::signal<void, bool, string, string, LoginWindow*> LoginWindow::signalDone() {
    return done;
}

void LoginWindow::onButtonOk() {
    string name = entry1.get_text();
    string password = entry2.get_text();
    done ( true, name, password, this );
}

void LoginWindow::onButtonStorno() {
    done ( false, "", "", this );
}

SetupWindow::SetupWindow ( list<AlbumItem> albums, int count ) :
        table1 ( 2, 2 ),
        table2 ( 2, 1 ),
        table3 ( 2, 2 ),
        table4 ( 1, 1 ) {
    // Self cofigure
    set_title ( "Picasa uploader" );
    set_resizable ( false );
    set_deletable ( false );
    // Variables & Objects
    listStore = Gtk::ListStore::create ( modelColumns );
    list<AlbumItem>::iterator i;
    for ( i = albums.begin(); i != albums.end(); ++i ) {
        AlbumItem item = *i;
        Gtk::TreeModel::Row row = * ( listStore->append() );
        row[modelColumns.title] = item.title;
        row[modelColumns.link] = item.link;
    }
    this->count = count;
    this->position = 0;
    // Widgets
    Gtk::VBox *box = get_vbox();
    frame1.set_label ( "Select album" );
    frame1.set_border_width ( 2 );
    box->pack_start ( frame1, Gtk::PACK_SHRINK );
    table1.set_border_width ( 2 );
    frame1.add ( table1 );
    Gtk::RadioButton::Group group1 = radio1.get_group();
    radio1.set_label ( "Create new album" );
    table1.attach ( radio1, 0, 1, 0, 1 );
    radio2.set_group ( group1 );
    radio2.set_label ( "Add to existing album" );
    table1.attach ( radio2, 0, 1, 1, 2 );
    table1.attach ( entry, 1, 2, 0, 1 );
    combo.pack_start ( modelColumns.title );
    combo.set_model ( listStore );
    combo.set_active ( listStore->children().begin() );
    table1.attach ( combo, 1, 2, 1, 2 );
    frame2.set_label ( "Photo titles" );
    frame2.set_border_width ( 2 );
    box->pack_start ( frame2, Gtk::PACK_SHRINK );
    table2.set_border_width ( 2 );
    frame2.add ( table2 );
    Gtk::RadioButton::Group group2 = radio3.get_group();
    radio3.set_label ( "No title" );
    table2.attach ( radio3, 0, 1, 0, 1 );
    radio4.set_group ( group2 );
    radio4.set_label ( "Filename" );
    table2.attach ( radio4, 1, 2, 0, 1 );
    frame3.set_label ( "Photo size" );
    frame3.set_border_width ( 2 );
    box->pack_start ( frame3, Gtk::PACK_SHRINK );
    table3.set_border_width ( 2 );
    frame3.add ( table3 );
    Gtk::RadioButton::Group group3 = radio5.get_group();
    radio5.set_label ( "800 x 600" );
    table3.attach ( radio5, 0, 1, 0, 1 );
    radio6.set_group ( group3 );
    radio6.set_label ( "1024 x 768" );
    table3.attach ( radio6, 0, 1, 1, 2 );
    radio7.set_group ( group3 );
    radio7.set_label ( "1200 x 1600" );
    table3.attach ( radio7, 1, 2, 0, 1 );
    radio8.set_group ( group3 );
    radio8.set_label ( "Original size" );
    table3.attach ( radio8, 1, 2, 1, 2 );
    frame4.set_label ( "Upload progress" );
    frame4.set_border_width ( 2 );
    box->pack_start ( frame4, Gtk::PACK_SHRINK );
    table4.set_border_width ( 2 );
    frame4.add ( table4 );
    progressBar.set_text ( itos ( 0 ) + " %" );
    table4.attach ( progressBar, 0, 1, 0, 1 );
    Gtk::HButtonBox *buttonBox = get_action_area();
    button1.set_label ( "Upload" );
    button1.signal_clicked().connect ( sigc::mem_fun ( *this, &SetupWindow::onButtonOk ) );
    buttonBox->pack_start ( button1, Gtk::PACK_EXPAND_WIDGET );
    button2.set_label ( "Cancel" );
    button2.signal_clicked().connect ( sigc::mem_fun ( *this, &SetupWindow::onButtonStorno ) );
    buttonBox->pack_start ( button2, Gtk::PACK_EXPAND_WIDGET );
    // Finally
    show_all_children();
    show();
}

SetupWindow::~SetupWindow() {
    hide();
}

void SetupWindow::doIncrease() {
    position++;
    float percentage = ( float ) position / count;
    progressBar.set_fraction ( percentage );
    progressBar.set_text ( itos ( percentage*100 ) + " %" );
}

sigc::signal<void, bool, bool, string, bool, int, SetupWindow*> SetupWindow::signalDone() {
    return done;
}

void SetupWindow::onButtonOk() {
    bool existing = radio2.get_active();
    string album;
    if ( existing ) {
        Gtk::TreeModel::iterator iter = combo.get_active();
        if ( iter ) {
            Gtk::TreeModel::Row row = *iter;
            Glib::ustring temp = row[modelColumns.link];
            album = temp;
        } else {
            return;
        }
    } else {
        if ( entry.get_text().empty() == false ) {
            album = entry.get_text();
        } else {
            Gtk::MessageDialog dialog ( *this, "Name is required", false, Gtk::MESSAGE_ERROR, Gtk::BUTTONS_OK );
            dialog.set_secondary_text ( "Please fill in name of new album." );
            dialog.run();
            return;
        }
    }
    bool titles = radio4.get_active();
    int dimension = -1;
    if ( radio5.get_active() ) {
        dimension = 800;
    }
    if ( radio6.get_active() ) {
        dimension = 1024;
    }
    if ( radio7.get_active() ) {
        dimension = 1200;
    }
    table1.set_sensitive ( false );
    table2.set_sensitive ( false );
    table3.set_sensitive ( false );
    button1.set_sensitive ( false );
    button2.set_sensitive ( false );
    done ( true, existing, album, titles, dimension, this );
}

void SetupWindow::onButtonStorno() {
    done ( false, false, "", false, -1, this );
}
