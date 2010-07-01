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

#include "trackwidget.h"

TrackWidget::TrackWidget() {
    // Self cofigure
    set_shadow_type ( Gtk::SHADOW_ETCHED_IN );
    set_policy ( Gtk::POLICY_AUTOMATIC, Gtk::POLICY_ALWAYS );
    // Variables & Objects
    first = NULL;
    lastId = -1;
    selectedId = -1;
    listStore = Gtk::ListStore::create ( modelColumns );
    listStore->set_sort_column ( modelColumns.time1, Gtk::SORT_ASCENDING );
    // Widgets
    treeView.set_model ( listStore );
    int columnN = treeView.append_column ( "Color", cellRenderer );
    Gtk::TreeViewColumn* column = treeView.get_column ( columnN - 1 );
    column->add_attribute ( cellRenderer, "color", modelColumns.color );
    treeView.append_column ( "Name", modelColumns.name );
    treeView.append_column ( "Date", modelColumns.dateStr );
    treeView.append_column ( "Start time", modelColumns.timeStr1 );
    treeView.append_column ( "Stop time", modelColumns.timeStr2 );
    treeView.append_column ( "Points", modelColumns.count );
    treeView.append_column ( "Length", modelColumns.length );
    treeView.signal_button_press_event().connect_notify ( sigc::mem_fun ( *this, &TrackWidget::onButtonPress ) );
    add ( treeView );
    Gtk::Menu::MenuList& menuList = menu.items();
    menuList.push_back ( Gtk::Menu_Helpers::MenuElem ( "Edit", sigc::mem_fun ( *this, &TrackWidget::onMenuEdit ) ) );
    menuList.push_back ( Gtk::Menu_Helpers::MenuElem ( "Delete", sigc::mem_fun ( *this, &TrackWidget::onMenuDelete ) ) );
    menu.accelerate ( *this );
    // Windows
    trackWindow.signalDone().connect ( sigc::mem_fun ( *this, &TrackWidget::onEditDone ) );
}

TrackWidget::~TrackWidget() {
}

void TrackWidget::setFirst ( LogEntry *first ) {
    this->first = first;
}

void TrackWidget::AddTrack() {
    if ( first != NULL ) {
        trackWindow.doShowAdd ( first );
    } else {
        Gtk::MessageDialog dialog ( trackWindow, "No data", false, Gtk::MESSAGE_ERROR, Gtk::BUTTONS_OK );
        dialog.set_secondary_text ( "Please load some GPS data first." );
        dialog.run();
        lock ( true );
    }
}

sigc::signal<void, bool> TrackWidget::signalLock() {
    return lock;
}

void TrackWidget::onButtonPress ( GdkEventButton *event ) {
    Glib::RefPtr<Gtk::TreeView::Selection> selection = treeView.get_selection();
    if ( selection ) {
        Gtk::TreeModel::iterator iter = selection->get_selected();
        if ( iter ) {
            Gtk::TreeModel::Row row = *iter;
            selectedId = row[modelColumns.id];
            int button = event->button;
            if ( ( button == 1 ) && ( event->type == GDK_2BUTTON_PRESS ) ) {
                onMenuEdit();
            }
            if ( button == 3 ) {
                menu.popup ( event->button, event->time );
            }
        } else {
            selectedId = -1;
        }
    } else {
        selectedId = -1;
    }
}

void TrackWidget::onMenuEdit() {
    typedef Gtk::TreeModel::Children Children;
    Children children = listStore->children();
    for ( Children::iterator iter = children.begin(); iter != children.end(); ++iter ) {
        Gtk::TreeModel::Row row = *iter;
        if ( row[modelColumns.id] == selectedId ) {
            Glib::ustring name = row[modelColumns.name];
            int color = row[modelColumns.color];
            time_t start = row[modelColumns.time1];
            time_t stop = row[modelColumns.time2];
            trackWindow.doShowEdit ( first, selectedId, name, color, start, stop );
            break;
        }
    }
}

void TrackWidget::onMenuDelete() {
    typedef Gtk::TreeModel::Children Children;
    Children children = listStore->children();
    for ( Children::iterator iter = children.begin(); iter != children.end(); ++iter ) {
        Gtk::TreeModel::Row row = *iter;
        if ( row[modelColumns.id] == selectedId ) {
            listStore->erase ( row );
            break;
        }
    }
}

void TrackWidget::onEditDone ( bool success, int itemId, string name, int color, long start, long stop ) {
    trackWindow.hide();
    if ( success ) {
        Gtk::TreeModel::Row row;
        if ( itemId == -1 ) {
            row = * ( listStore->append() );
            lastId++;
        } else {
            typedef Gtk::TreeModel::Children Children;
            Children children = listStore->children();
            for ( Children::iterator iter = children.begin(); iter != children.end(); ++iter ) {
                row = *iter;
                if ( row[modelColumns.id] == itemId ) {
                    break;
                }
            }
        }
        row[modelColumns.id] = lastId;
        row[modelColumns.name] = name;
        row[modelColumns.color] = color;
        row[modelColumns.time1] = start;
        row[modelColumns.time2] = stop;
        struct tm *timeinfo;
        char buffer [11];
        timeinfo = localtime ( &start );
        strftime ( buffer, 11, "%d.%m.%Y", timeinfo );
        row[modelColumns.dateStr] = buffer;
        strftime ( buffer, 11, "%H:%M:%S", timeinfo );
        row[modelColumns.timeStr1] = buffer;
        timeinfo = localtime ( &stop );
        strftime ( buffer, 11, "%H:%M:%S", timeinfo );
        row[modelColumns.timeStr2] = buffer;
    }
    lock ( true );
}
