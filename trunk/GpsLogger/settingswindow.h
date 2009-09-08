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

#ifndef _SETTINGSWINDOW_H
#define	_SETTINGSWINDOW_H

#include <gtkmm.h>
#include "connector.h"
#include "functions.h"

using namespace std;

class SettingsWindow : public Gtk::Dialog {
public:
    // Constructor & Destructor
    SettingsWindow();
    virtual ~SettingsWindow();
    // Getters & Setters
    void setConnector ( Connector* );
    // Action calls
    void doShow();
    // Signals
    sigc::signal<void> signalDone();
private:
    // Variables & Objects
    Connector *connector;
    // Widgets
    Gtk::Label label1;
    Gtk::ProgressBar progress1;
    Gtk::CheckButton check1;
    Gtk::HSeparator separator1;
    Gtk::Label label2;
    Gtk::HBox box1;
    Gtk::RadioButton radio1, radio2;
    Gtk::HBox box2;
    Gtk::Button button1;
    Gtk::HSeparator separator2;
    Gtk::Label label3;
    Gtk::HBox box3, box4;
    Gtk::Entry entry1, entry2;
    Gtk::Label label4, label5;
    Gtk::CheckButton check2, check3;
    Gtk::Button button2, button3;
    // Widget signals
    void onButtonClear();
    void onButtonOk();
    void onButtonStorno();
    // Signals
    sigc::signal<void> done;
};

#endif	/* _SETTINGSWINDOW_H */

