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

#include "mainwindow.h"

MainWindow::MainWindow() {
    // Self cofigure
    set_title ( "GPS Photo Tagger" );
    set_default_size ( 800, 600 );
    // Dispatchers
    progress.connect ( sigc::mem_fun ( *this, &MainWindow::onProgress ) );
    done.connect ( sigc::mem_fun ( *this, &MainWindow::onDone ) );
    // Widgets
    add ( box1 );
    actionGroup = Gtk::ActionGroup::create();
    actionGroup->add ( Gtk::Action::create ( "FileMenu", "File" ) );
    actionGroup->add ( Gtk::Action::create ( "FileNew", Gtk::Stock::NEW, "New file" ), sigc::mem_fun ( *this, &MainWindow::onMenuFileNew ) );
    actionGroup->add ( Gtk::Action::create ( "FileLoad", Gtk::Stock::OPEN, "Open file" ), sigc::mem_fun ( *this, &MainWindow::onMenuFileLoad ) );
    actionGroup->add ( Gtk::Action::create ( "FileSave", Gtk::Stock::SAVE_AS, "Save file" ), sigc::mem_fun ( *this, &MainWindow::onMenuFileSave ) );
    actionGroup->add ( Gtk::Action::create ( "DeviceMenu", "Device" ) );
    actionGroup->add ( Gtk::Action::create ( "DeviceSettings", Gtk::Stock::PREFERENCES, "Settings" ), sigc::mem_fun ( *this, &MainWindow::onMenuDeviceSettings ) );
    actionGroup->add ( Gtk::Action::create ( "DeviceLoad", Gtk::Stock::GO_DOWN, "Load GPS" ), sigc::mem_fun ( *this, &MainWindow::onMenuDeviceLoad ) );
    actionGroup->add ( Gtk::Action::create ( "TrackMenu", "Tracks" ) );
    actionGroup->add ( Gtk::Action::create ( "TrackAdd", Gtk::Stock::ADD, "Add track" ), sigc::mem_fun ( *this, &MainWindow::onMenuTrackAdd ) );
    actionGroup->add ( Gtk::Action::create ( "TrackAuto", Gtk::Stock::DND_MULTIPLE, "Automatic" ), sigc::mem_fun ( *this, &MainWindow::onMenuTrackAuto ) );
    actionGroup->add ( Gtk::Action::create ( "PhotosMenu", "Photos" ) );
    actionGroup->add ( Gtk::Action::create ( "PhotosLoad", Gtk::Stock::DND_MULTIPLE, "Add photos" ), sigc::mem_fun ( *this, &MainWindow::onMenuPhotoLoad ) );
    actionGroup->add ( Gtk::Action::create ( "PhotosSettings", Gtk::Stock::PROPERTIES, "Time settings" ), sigc::mem_fun ( *this, &MainWindow::onMenuPhotoSettings ) );
    actionGroup->add ( Gtk::Action::create ( "PhotosClear", Gtk::Stock::DELETE, "Delete all" ), sigc::mem_fun ( *this, &MainWindow::onMenuPhotoClear ) );
    actionGroup->add ( Gtk::Action::create ( "ExportMenu", "Export" ) );
    actionGroup->add ( Gtk::Action::create ( "ExportExif", Gtk::Stock::EDIT, "Write EXIF" ), sigc::mem_fun ( *this, &MainWindow::onMenuExportExif ) );
    actionGroup->add ( Gtk::Action::create ( "ExportPicasa", Gtk::Stock::NETWORK, "Picasa Web" ), sigc::mem_fun ( *this, &MainWindow::onMenuExportPicasa ) );
    uiManager = Gtk::UIManager::create();
    uiManager->insert_action_group ( actionGroup );
    add_accel_group ( uiManager->get_accel_group() );
    Glib::ustring menu = "<ui>"
                         "<menubar name='MenuBar'>"
                         "<menu action='FileMenu'>"
                         "<menuitem action='FileNew'/>"
                         "<menuitem action='FileLoad'/>"
                         "<menuitem action='FileSave'/>"
                         "</menu>"
                         "<menu action='DeviceMenu'>"
                         "<menuitem action='DeviceLoad'/>"
                         "<menuitem action='DeviceSettings'/>"
                         "</menu>"
                         "<menu action='TrackMenu'>"
                         "<menuitem action='TrackAdd'/>"
                         "<menuitem action='TrackAuto'/>"
                         "</menu>"
                         "<menu action='PhotosMenu'>"
                         "<menuitem action='PhotosLoad'/>"
                         "<menuitem action='PhotosSettings'/>"
                         "<menuitem action='PhotosClear'/>"
                         "</menu>"
                         "<menu action='ExportMenu'>"
                         "<menuitem action='ExportExif'/>"
                         "<menuitem action='ExportPicasa'/>"
                         "</menu>"
                         "</menubar>"
                         "<toolbar  name='ToolBar'>"
                         "<toolitem action='FileLoad'/>"
                         "<toolitem action='DeviceLoad'/>"
                         "<toolitem action='PhotosLoad'/>"
                         "<toolitem action='PhotosSettings'/>"
                         "<toolitem action='ExportPicasa'/>"
                         "</toolbar>"
                         "</ui>";
    uiManager->add_ui_from_string ( menu );
    menuBar = uiManager->get_widget ( "/MenuBar" );
    box1.pack_start ( *menuBar, Gtk::PACK_SHRINK );
    toolBar = uiManager->get_widget ( "/ToolBar" );
    box1.pack_start ( *toolBar, Gtk::PACK_SHRINK );
    box1.pack_start ( box2, Gtk::PACK_EXPAND_WIDGET );
    notebook.set_border_width ( 2 );
    box2.pack_start ( notebook, Gtk::PACK_EXPAND_WIDGET );
    mapWidget.set_border_width ( 2 );
    mapWidget.signalAskTrack().connect ( sigc::mem_fun ( *this, &MainWindow::onAskTrack ) );
    mapWidget.signalPhotoClick().connect ( sigc::mem_fun ( *this, &MainWindow::onPhotoClick ) );
    notebook.append_page ( mapWidget, "Map" );
    trackWidget.set_border_width ( 2 );
    trackWidget.signalChange().connect ( sigc::mem_fun ( *this, &MainWindow::onChange ) );
    trackWidget.signalLock().connect ( sigc::mem_fun ( *this, &MainWindow::onLockCall ) );
    notebook.append_page ( trackWidget, "Tracks" );
    photoWidget.set_border_width ( 2 );
    photoWidget.signalChange().connect ( sigc::mem_fun ( *this, &MainWindow::onChange ) );
    photoWidget.signalDone().connect ( sigc::mem_fun ( *this, &MainWindow::onUnlockCall ) );
    notebook.append_page ( photoWidget, "Photos" );
    box2.pack_start ( seperator, Gtk::PACK_SHRINK );
    scrolledWindow.set_border_width ( 2 );
    scrolledWindow.set_shadow_type ( Gtk::SHADOW_ETCHED_IN );
    scrolledWindow.set_policy ( Gtk::POLICY_NEVER, Gtk::POLICY_ALWAYS );
    scrolledWindow.set_size_request ( 200, -1 );
    box2.pack_start ( scrolledWindow, Gtk::PACK_SHRINK );
    dayView.signalChange().connect ( sigc::mem_fun ( *this, &MainWindow::onDayChange ) );
    scrolledWindow.add ( dayView );
    statusBar.push ( "No data" );
    box1.pack_start ( statusBar, Gtk::PACK_SHRINK );
    // Windows
    settingsWindow.setConnector ( &connector );
    settingsWindow.signalDone().connect ( sigc::mem_fun ( *this, &MainWindow::onUnlockCall ) );
    // Finally
    show_all_children();
}

MainWindow::~MainWindow() {
    dayView.clear();
    trackWidget.setFirst ( NULL );
    photoWidget.setFirst ( NULL );
}

void MainWindow::disconnectedErr() {
    Gtk::MessageDialog dialog ( *this, "Device not found", false, Gtk::MESSAGE_ERROR, Gtk::BUTTONS_OK );
    dialog.set_secondary_text ( "Please connect GPS device and restart application." );
    dialog.run();
}

void MainWindow::onProgress() {
    downloadWindow.doIncrease();
}

void MainWindow::onDone() {
    LogEntry* first = connector.GetFirstEntry();
    dayView.append ( first );
    trackWidget.setFirst ( first );
    photoWidget.setFirst ( first );
    downloadWindow.hide();
    set_sensitive ( true );
    statusBar.push ( "Data downloaded from device" );
}

void MainWindow::onMenuFileNew() {
    dayView.clear();
    trackWidget.setFirst ( NULL );
    photoWidget.setFirst ( NULL );
    statusBar.push ( "No data" );
}

void MainWindow::onMenuFileLoad() {
    Gtk::FileChooserDialog dialog ( "Please choose a file", Gtk::FILE_CHOOSER_ACTION_OPEN );
    dialog.set_transient_for ( *this );
    dialog.set_default_size ( 400, 300 );
    dialog.add_button ( Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL );
    dialog.add_button ( Gtk::Stock::OPEN, Gtk::RESPONSE_OK );
    Gtk::FileFilter filter;
    filter.set_name ( "GPS files" );
    filter.add_pattern ( "*.gps" );
    dialog.add_filter ( filter );
    int result = dialog.run();
    if ( result == Gtk::RESPONSE_OK ) {
        string fileName = dialog.get_filename();
        LogEntry* first = dayView.load ( fileName.c_str() );
        trackWidget.setFirst ( first );
        photoWidget.setFirst ( first );
        statusBar.push ( "Data from file " + fileName );
    }
}

void MainWindow::onMenuFileSave() {
    Gtk::FileChooserDialog dialog ( "Please choose a file", Gtk::FILE_CHOOSER_ACTION_SAVE );
    dialog.set_transient_for ( *this );
    dialog.set_default_size ( 400, 300 );
    dialog.add_button ( Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL );
    dialog.add_button ( Gtk::Stock::SAVE, Gtk::RESPONSE_OK );
    Gtk::FileFilter filter;
    filter.set_name ( "GPS files" );
    filter.add_pattern ( "*.gps" );
    dialog.add_filter ( filter );
    int result = dialog.run();
    if ( result == Gtk::RESPONSE_OK ) {
        string fileName = dialog.get_filename();
        dayView.save ( fileName.c_str() );
        statusBar.push ( "Data from file " + fileName );
    }
}

void MainWindow::onMenuDeviceSettings() {
    if ( connector.isConnected() ) {
        set_sensitive ( false );
        settingsWindow.doShow();
    } else {
        disconnectedErr();
    }
}

void MainWindow::onMenuDeviceLoad() {
    int sectors = connector.doRead ( &progress, &done );
    if ( sectors != -1 ) {
        set_sensitive ( false );
        downloadWindow.doInicialize ( sectors );
    } else {
        disconnectedErr();
    }
}

void MainWindow::onMenuTrackAdd() {
    set_sensitive ( false );
    trackWidget.AddTrack();
}

void MainWindow::onMenuTrackAuto() {
    trackWidget.AutoTrack();
}

void MainWindow::onMenuPhotoLoad() {
    Gtk::FileChooserDialog dialog ( "Please choose photos", Gtk::FILE_CHOOSER_ACTION_OPEN );
    dialog.set_transient_for ( *this );
    dialog.set_default_size ( 400, 300 );
    dialog.add_button ( Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL );
    dialog.add_button ( Gtk::Stock::OPEN, Gtk::RESPONSE_OK );
    Gtk::FileFilter filter;
    filter.set_name ( "JPG files" );
    filter.add_pattern ( "*.jpg" );
    filter.add_pattern ( "*.JPG" );
    dialog.add_filter ( filter );
    dialog.set_select_multiple ( true );
    int result = dialog.run();
    if ( result == Gtk::RESPONSE_OK ) {
        list<string> fileList = dialog.get_filenames();
        for ( list<string>::iterator i = fileList.begin(); i != fileList.end(); ++i ) {
            string fileName = *i;
            photoWidget.addPhoto ( fileName.c_str() );
        }
    }
}

void MainWindow::onMenuPhotoSettings() {
    set_sensitive ( false );
    photoWidget.showSettings();
}

void MainWindow::onMenuPhotoClear() {
    photoWidget.clearPhotos();
}

void MainWindow::onMenuExportExif() {
    photoWidget.writeExif();
}

void MainWindow::onMenuExportPicasa() {
    list<PhotoItem> photoList = photoWidget.getPhotos();
    picasaExport.doExport ( photoList );
}

void MainWindow::onAskTrack ( int trackId, int *color, int *count ) {
    return trackWidget.askTrack ( trackId, color, count );
}

void MainWindow::onPhotoClick ( int *photos ) {
    photoWidget.showPhotos ( photos );
}

void MainWindow::onLockCall ( bool lock ) {
    set_sensitive ( lock );
}

void MainWindow::onChange() {
    mapWidget.doRedraw();
}

void MainWindow::onDayChange ( LogEntry *first ) {
    mapWidget.setFirstVisible ( first );
}

void MainWindow::onUnlockCall() {
    set_sensitive ( true );
}
