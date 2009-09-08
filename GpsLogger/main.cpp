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

#include <gtkmm.h>
#include <Magick++.h>
#include "mainwindow.h"

using namespace std;

int main ( int argc, char** argv ) {
    Gtk::Main main ( &argc, &argv );
    Magick::InitializeMagick ( *argv );
    MainWindow *mainWindow = new MainWindow;
    Gtk::Main::run ( *mainWindow );
    delete ( mainWindow );
    return ( EXIT_SUCCESS );
}
