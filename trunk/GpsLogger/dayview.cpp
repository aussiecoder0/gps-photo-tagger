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

#include "dayview.h"

DayView::DayView() {
    // Variables & Objects
    first = NULL;
    firstVisible = NULL;
    listStore = Gtk::ListStore::create ( modelColumns );
    // Widgets
    set_model ( listStore );
    append_column_editable ( "", modelColumns.enabled );
    append_column ( "Date", modelColumns.date );
    append_column ( "Points", modelColumns.count );
    Gtk::CellRendererToggle* cellRenderer = ( Gtk::CellRendererToggle* ) get_column_cell_renderer ( 0 );
    cellRenderer->signal_toggled().connect ( sigc::mem_fun ( *this, &DayView::onToggled ) );
}

DayView::~DayView() {
    clear();
}

void DayView::clear() {
    LogEntry* next = first;
    LogEntry* temp;
    while ( next != NULL ) {
        temp = next;
        next = next->next;
        delete ( temp );
    }
    first = NULL;
    update();
}

void DayView::append ( LogEntry* first ) {
    clear();
    this->first = first;
    update();
}

void DayView::save ( const char *fileName ) {
    FILE *file = fopen ( fileName, "w" );
    if ( file ) {
        char *buffer;
        LogEntry* next = first;
        while ( next != NULL ) {
            buffer = ( char* ) & next->time;
            fwrite ( buffer, 1, 4, file );
            buffer = ( char* ) & next->speed;
            fwrite ( buffer, 1, 4, file );
            buffer = ( char* ) & next->latitude;
            fwrite ( buffer, 1, 8, file );
            buffer = ( char* ) & next->longitude;
            fwrite ( buffer, 1, 8, file );
            buffer = ( char* ) & next->height;
            fwrite ( buffer, 1, 8, file );
            next = next->next;
        }
        fclose ( file );
    } else {
        string message = "Can't save file ";
        message += fileName;
        errorMsg ( message );
    }
}

LogEntry* DayView::load ( const char *fileName ) {
    clear();
    FILE *file = fopen ( fileName, "r" );
    if ( file ) {
        char buffer[8];
        long *type1 = ( long* ) & buffer;
        int *type2 = ( int* ) & buffer;
        double *type3 = ( double* ) & buffer;
        LogEntry *next;
        LogEntry *last = NULL;
        while ( true ) {
            int size = fread ( buffer, 1, 4, file );
            if ( size == 0 ) {
                break;
            }
            next = new LogEntry();
            next->time = *type1;
            fread ( buffer, 1, 4, file );
            next->speed = *type2;
            fread ( buffer, 1, 8, file );
            next->latitude = *type3;
            fread ( buffer, 1, 8, file );
            next->longitude = *type3;
            fread ( buffer, 1, 8, file );
            next->height = *type3;
            next->photo = -1;
            next->next = NULL;
            next->nextDay = NULL;
            next->nextVisible = NULL;
            if ( last == NULL ) {
                first = next;
            } else {
                last->next = next;
                last->nextVisible = next;
            }
            last = next;
        }
        fclose ( file );
        update();
        return first;
    }
    string message = "Can't open file ";
    message += fileName;
    errorMsg ( message );
    return NULL;
}

sigc::signal<void, LogEntry*> DayView::signalChange() {
    return change;
}

void DayView::update() {
    listStore->clear();
    Gtk::TreeModel::Row row;
    LogEntry *next = first;
    LogEntry *last = NULL;
    struct tm *timeinfo;
    char buffer [7];
    char buffer2 [11];
    char lastbuf [7] = "000000";
    int count = 0;
    while ( next != NULL ) {
        timeinfo = localtime ( &next->time );
        strftime ( buffer, 7, "%y%m%d", timeinfo );
        if ( strcmp ( lastbuf, buffer ) != 0 ) {
            if ( count != 0 ) {
                row[modelColumns.count] = count;
                LogEntry *temp = row[modelColumns.first];
                temp->nextDay = next;
                row[modelColumns.last] = last;
                count = 0;
            }
            strftime ( buffer2, 11, "%d.%m.%Y", timeinfo );
            row = * ( listStore->append() );
            row[modelColumns.date] = buffer2;
            row[modelColumns.first] = next;
        }
        strcpy ( lastbuf, buffer );
        count++;
        last = next;
        next = next->next;
    }
    if ( count != 0 ) {
        row[modelColumns.enabled] = true;
        row[modelColumns.count] = count;
        LogEntry *temp = row[modelColumns.first];
        temp->nextDay = NULL;
        row[modelColumns.last] = last;
    }
    onToggled ( "" );
}

void DayView::onToggled ( const Glib::ustring& path ) {
    typedef Gtk::TreeModel::Children Children;
    Children children = listStore->children();
    firstVisible = NULL;
    LogEntry *last = NULL;
    for ( Children::iterator iter = children.begin(); iter != children.end(); ++iter ) {
        Gtk::TreeModel::Row row = *iter;
        if ( row[modelColumns.enabled] ) {
            if ( firstVisible == NULL ) {
                firstVisible = row[modelColumns.first];
            } else {
                last->nextVisible = row[modelColumns.first];
            }
            last = row[modelColumns.last];
            last->nextVisible = NULL;
        }
    }
    change ( firstVisible );
}
