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

#include "photowidget.h"

PhotoWidget::PhotoWidget() {
    // Self cofigure
    set_shadow_type ( Gtk::SHADOW_ETCHED_IN );
    set_policy ( Gtk::POLICY_AUTOMATIC, Gtk::POLICY_ALWAYS );
    // Variables & Objects
    first = NULL;
    lastId = -1;
    selectedId = -1;
    listStore = Gtk::ListStore::create ( modelColumns );
    listStore->set_sort_column ( modelColumns.time, Gtk::SORT_ASCENDING );
    // Widgets
    treeView.set_model ( listStore );
    treeView.append_column ( "Date", modelColumns.dateStr );
    treeView.append_column ( "Time", modelColumns.timeStr );
    treeView.append_column ( "Latitude", modelColumns.latStr );
    treeView.append_column ( "Longitude", modelColumns.lonStr );
    treeView.append_column ( "Path", modelColumns.path );
    treeView.signal_button_press_event().connect_notify ( sigc::mem_fun ( *this, &PhotoWidget::onButtonPress ) );
    add ( treeView );
    Gtk::Menu::MenuList& menuList = menu.items();
    menuList.push_back ( Gtk::Menu_Helpers::MenuElem ( "Show", sigc::mem_fun ( *this, &PhotoWidget::onMenuShow ) ) );
    menuList.push_back ( Gtk::Menu_Helpers::MenuElem ( "Delete", sigc::mem_fun ( *this, &PhotoWidget::onMenuDelete ) ) );
    menu.accelerate ( *this );
    // Windows
    timeWindow.signalDone().connect ( sigc::mem_fun ( *this, &PhotoWidget::onEditDone ) );
}

PhotoWidget::~PhotoWidget() {
}

void PhotoWidget::setFirst ( LogEntry *first ) {
    this->first = first;
    relocateAll ( false );
    change();
}

list<PhotoItem> PhotoWidget::getPhotos() {
    list<PhotoItem> photoList;
    PhotoItem photoItem;
    typedef Gtk::TreeModel::Children Children;
    Children children = listStore->children();
    for ( Children::iterator iter = children.begin(); iter != children.end(); ++iter ) {
        Gtk::TreeModel::Row row = *iter;
        photoItem.id = row[modelColumns.id];
        LogEntry *entry = row[modelColumns.entry];
        if ( entry != NULL ) {
            photoItem.hasPos = true;
            photoItem.latitude = entry->latitude;
            photoItem.longitude = entry->longitude;
        } else {
            photoItem.hasPos = false;
            photoItem.latitude = 0;
            photoItem.longitude = 0;
        }
        Glib::ustring path = row[modelColumns.path];
        photoItem.path = path;
        photoList.push_back ( photoItem );
    }
    return photoList;
}

void PhotoWidget::clearPhotos() {
    lastId = -1;
    selectedId = -1;
    typedef Gtk::TreeModel::Children Children;
    Children children = listStore->children();
    for ( Children::iterator iter = children.begin(); iter != children.end(); ++iter ) {
        Gtk::TreeModel::Row row = *iter;
        row[modelColumns.locationId] = -1;
        LogEntry* entry = row[modelColumns.entry];
        if ( entry != NULL ) {
            entry->photo = -1;
        }
    }
    listStore->clear();
    cameras.clear();
    change();
}

void PhotoWidget::addPhoto ( const char* fileName ) {
    Gtk::TreeModel::Row row = * ( listStore->append() );
    lastId++;
    row[modelColumns.id] = lastId;
    row[modelColumns.locationId] = -1;
    row[modelColumns.entry] = NULL;
    row[modelColumns.path] = fileName;
    string cameraName = "Unknown";
    time_t takeTime = 0;
    try {
        Exiv2::Image::AutoPtr image = Exiv2::ImageFactory::open ( fileName );
        image->readMetadata();
        Exiv2::ExifData &exifData = image->exifData();
        for ( Exiv2::ExifData::const_iterator i = exifData.begin(); i != exifData.end(); ++i ) {
            int tag = i->tag();
            if ( tag == 272 ) {
                stringstream out;
                out << i->value();
                cameraName = out.str();
            }
            if ( tag == 306 ) {
                stringstream out;
                out << i->value();
                const char *buffer = out.str().c_str();
                struct tm *timeinfo = ( tm* ) malloc ( sizeof ( tm ) );
                timeinfo->tm_year = atoi ( buffer ) - 1900;
                buffer += 5;
                timeinfo->tm_mon = atoi ( buffer ) - 1;
                buffer += 3;
                timeinfo->tm_mday = atoi ( buffer );
                buffer += 3;
                timeinfo->tm_hour = atoi ( buffer );
                buffer += 3;
                timeinfo->tm_min = atoi ( buffer );
                buffer += 3;
                timeinfo->tm_sec = atoi ( buffer );
                timeinfo->tm_isdst = 0;
                takeTime = mktime ( timeinfo );
                free ( timeinfo );
            }
        }
    } catch ( Exiv2::Error& ) {
        string message = "Can't load exif dtata from ";
        message += fileName;
        errorMsg ( message );
    }
    int index = 0;
    bool found = false;
    list<CameraTime>::iterator i;
    for ( i = cameras.begin(); i != cameras.end(); ++i ) {
        CameraTime record = *i;
        if ( record.cameraName == cameraName ) {
            row[modelColumns.cameraId] = index;
            found = true;
            break;
        }
        index++;
    }
    if ( found == false ) {
        CameraTime record;
        record.cameraName = cameraName;
        record.timeShift = 0;
        cameras.push_back ( record );
        row[modelColumns.cameraId] = index;
    }
    row[modelColumns.time] = takeTime;
    locatePhoto ( row );
    change();
}

void PhotoWidget::showPhotos ( int *photos ) {
    int *photos2 = photos;
    photoWindow.clearPhotos();
    while ( *photos2 != -1 ) {
        typedef Gtk::TreeModel::Children Children;
        Children children = listStore->children();
        for ( Children::iterator iter = children.begin(); iter != children.end(); ++iter ) {
            Gtk::TreeModel::Row row = *iter;
            if ( row[modelColumns.locationId] == *photos2 ) {
                Glib::ustring path = row[modelColumns.path];
                string info = extractFileName ( path ) + "\n";
                info += row[modelColumns.dateStr] + "\n";
                info += row[modelColumns.timeStr] + "\n";
                info += row[modelColumns.latStr] + "\n";
                info += row[modelColumns.lonStr] + "\n";
                photoWindow.addPhoto ( info, path.c_str() );
            }
        }
        photos2++;
    }
    photoWindow.doShow();
}

void PhotoWidget::showSettings() {
    bool exists = false;
    timeWindow.clearCameras();
    list<CameraTime>::iterator i;
    for ( i = cameras.begin(); i != cameras.end(); ++i ) {
        exists = true;
        CameraTime record = *i;
        timeWindow.addCamera ( record.cameraName, record.timeShift );
    }
    if ( exists ) {
        timeWindow.show();
    } else {
        Gtk::MessageDialog dialog ( timeWindow, "No photos", false, Gtk::MESSAGE_ERROR, Gtk::BUTTONS_OK );
        dialog.set_secondary_text ( "Please add some photos first." );
        dialog.run();
        done();
    }
}

sigc::signal<void> PhotoWidget::signalChange() {
    return change;
}

sigc::signal<void> PhotoWidget::signalDone() {
    return done;
}

void PhotoWidget::relocateAll ( bool clearLog ) {
    typedef Gtk::TreeModel::Children Children;
    Children children = listStore->children();
    for ( Children::iterator iter = children.begin(); iter != children.end(); ++iter ) {
        Gtk::TreeModel::Row row = *iter;
        if ( clearLog ) {
            LogEntry* entry = row[modelColumns.entry];
            if ( entry != NULL ) {
                entry->photo = -1;
            }
        }
        locatePhoto ( row );
    }
}

void PhotoWidget::locatePhoto ( Gtk::TreeModel::Row row ) {
    int index = 0;
    int cameraId = row[modelColumns.cameraId];
    int timeShift = 0;
    list<CameraTime>::iterator i;
    for ( i = cameras.begin(); i != cameras.end(); ++i ) {
        CameraTime record = *i;
        if ( index == cameraId ) {
            timeShift = record.timeShift;
            break;
        }
        index++;
    }
    long time = row[modelColumns.time] + timeShift;
    struct tm *timeinfo = localtime ( &time );
    char bufferOut [11];
    strftime ( bufferOut, 11, "%d.%m.%Y", timeinfo );
    row[modelColumns.dateStr] = bufferOut;
    strftime ( bufferOut, 11, "%H:%M:%S", timeinfo );
    row[modelColumns.timeStr] = bufferOut;
    row[modelColumns.locationId] = -1;
    row[modelColumns.entry] = NULL;
    row[modelColumns.latStr] = "";
    row[modelColumns.lonStr] = "";
    LogEntry* next = first;
    while ( next != NULL ) {
        int relative = difftime ( next->time, time );
        if ( ( relative > 0 ) && ( relative < 60 ) ) {
            row[modelColumns.entry] = next;
            row[modelColumns.latStr] = coordinate ( false, next->latitude );
            row[modelColumns.lonStr] = coordinate ( true, next->longitude );
            if ( next->photo == -1 ) {
                int id = row[modelColumns.id];
                row[modelColumns.locationId] = id;
                next->photo = id;
            } else {
                row[modelColumns.locationId] = next->photo;
            }
            break;
        }
        if ( next->nextDay == NULL ) {
            next = next->next;
        } else {
            int diff = difftime ( next->nextDay->time, time );
            if ( diff > 0 ) {
                next = next->next;
            } else {
                next = next->nextDay;
            }
        }
    }
}

void PhotoWidget::onButtonPress ( GdkEventButton *event ) {
    Glib::RefPtr<Gtk::TreeView::Selection> selection = treeView.get_selection();
    if ( selection ) {
        Gtk::TreeModel::iterator iter = selection->get_selected();
        if ( iter ) {
            Gtk::TreeModel::Row row = *iter;
            selectedId = row[modelColumns.id];
            int button = event->button;
            if ( ( button == 1 ) && ( event->type == GDK_2BUTTON_PRESS ) ) {
                onMenuShow();
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

void PhotoWidget::onMenuShow() {
    photoWindow.clearPhotos();
    typedef Gtk::TreeModel::Children Children;
    Children children = listStore->children();
    for ( Children::iterator iter = children.begin(); iter != children.end(); ++iter ) {
        Gtk::TreeModel::Row row = *iter;
        if ( row[modelColumns.id] == selectedId ) {
            Glib::ustring path = row[modelColumns.path];
            string info = extractFileName ( path ) + "\n";
            info += row[modelColumns.dateStr] + "\n";
            info += row[modelColumns.timeStr] + "\n";
            info += row[modelColumns.latStr] + "\n";
            info += row[modelColumns.lonStr] + "\n";
            photoWindow.addPhoto ( info, path.c_str() );
            break;
        }
    }
    photoWindow.doShow();
}

void PhotoWidget::onMenuDelete() {
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

void PhotoWidget::onEditDone ( bool success, int *times ) {
    timeWindow.hide();
    if ( success ) {
        int *times2 = times;
        list<CameraTime>::iterator i;
        for ( i = cameras.begin(); i != cameras.end(); ++i ) {
            CameraTime record = *i;
            record.timeShift = *times2;
            *i = record;
            times2++;
        }
    }
    relocateAll ( true );
    change();
    done();
}
