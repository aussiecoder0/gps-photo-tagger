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

#include "downloadwindow.h"

DownloadWindow::DownloadWindow() {
    // Self cofigure
    set_title ( "Reading data from device" );
    set_resizable ( false );
    set_deletable ( false );
    set_border_width ( 20 );
    // Widgets
    Gtk::VBox *box = get_vbox();
    progressBar.set_size_request ( 300, -1 );
    box->add ( progressBar );
    // Finally
    show_all_children();
}

DownloadWindow::~DownloadWindow() {
}

void DownloadWindow::doInicialize ( int sectors ) {
    sectorCount = sectors;
    sectorDone = 0;
    progressBar.set_fraction ( 0 );
    progressBar.set_text ( "Reading sector " + itos ( 1 ) + " of " + itos ( sectorCount ) );
    show();
    raise();
}

void DownloadWindow::doIncrease() {
    sectorDone++;
    if ( sectorDone < sectorCount ) {
        float percentage = ( float ) sectorDone / sectorCount;
        progressBar.set_fraction ( percentage );
        progressBar.set_text ( "Reading sector " + itos ( sectorDone + 1 ) + " of " + itos ( sectorCount ) );
    } else {
        progressBar.set_fraction ( 1 );
        progressBar.set_text ( "Reading done" );
    }
}
