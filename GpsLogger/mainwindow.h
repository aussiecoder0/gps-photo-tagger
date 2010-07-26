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

#ifndef _MAINWINDOW_H
#define	_MAINWINDOW_H

#include <gtkmm-2.4/gtkmm.h>
#include "connector.h"
#include "dayview.h"
#include "downloadwindow.h"
#include "mapwidget.h"
#include "picasaexport.h"
#include "photowidget.h"
#include "settingswindow.h"
#include "trackwidget.h"

using namespace std;

class MainWindow : public Gtk::Window {
public:
    // Constructor & Destructor
    MainWindow();
    virtual ~MainWindow();
private:
    // Variables & Objects
    Connector connector;
    PicasaExport picasaExport;
    // Functions
    void disconnectedErr();
    // Dispatchers
    Glib::Dispatcher progress;
    Glib::Dispatcher done;
    // Dispatcher signals
    void onProgress();
    void onDone();
    // Widgets
    Gtk::VBox box1;
    Glib::RefPtr<Gtk::UIManager> uiManager;
    Glib::RefPtr<Gtk::ActionGroup> actionGroup;
    Gtk::Widget *menuBar;
    Gtk::Widget *toolBar;
    Gtk::HBox box2;
    Gtk::Notebook notebook;
    MapWidget mapWidget;
    TrackWidget trackWidget;
    PhotoWidget photoWidget;
    Gtk::VSeparator seperator;
    Gtk::ScrolledWindow scrolledWindow;
    DayView dayView;
    Gtk::Statusbar statusBar;
    // Widget signals
    void onMenuFileNew();
    void onMenuFileLoad();
    void onMenuFileSave();
    void onMenuDeviceSettings();
    void onMenuDeviceLoad();
    void onMenuTrackAdd();
    void onMenuTrackAuto();
    void onMenuPhotoLoad();
    void onMenuPhotoSettings();
    void onMenuPhotoClear();
    void onMenuExportExif();
    void onMenuExportPicasa();
    void onAskTrack ( int, int*, int* );
    void onPhotoClick ( int* );
    void onLockCall ( bool );
    void onChange();
    void onDayChange ( LogEntry* );
    // Windows
    SettingsWindow settingsWindow;
    DownloadWindow downloadWindow;
    // Window signals
    void onUnlockCall();
};

#endif	/* _MAINWINDOW_H */
