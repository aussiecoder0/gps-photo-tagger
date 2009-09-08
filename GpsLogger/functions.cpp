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

#include "functions.h"

string itos ( int number ) {
    stringstream out;
    out << number;
    return out.str();
}

string dtos ( double number ) {
    stringstream out;
    out << number;
    return out.str();
}

string coordinate ( bool isLon, double coordinate ) {
    stringstream out;
    int deg = abs ( coordinate );
    if ( isLon ) {
        if ( coordinate >= 0 ) {
            out << "E ";
        } else {
            out << "W ";
        }
    } else {
        if ( coordinate >= 0 ) {
            out << "N ";
        } else {
            out << "E ";
        }
    }
    if ( isLon ) {
        out << setw ( 3 ) << setfill ( '0' ) << right << deg << "°";
    } else {
        out << setw ( 2 ) << setfill ( '0' ) << right << deg << "°";
    }
    int min = ( coordinate - deg ) * 60;
    out << setw ( 2 ) << setfill ( '0' ) << right << min << "'";
    int sec = ( ( coordinate - deg ) * 60 - min ) * 60;
    out << setw ( 2 ) << setfill ( '0' ) << right << sec << "''";

    return out.str();
}

string extractFileName ( string path ) {
    string rest = path;
    int pos = rest.find ( "/" );
    while ( pos != -1 ) {
        const char *temp = rest.c_str();
        temp += pos + 1;
        int size = rest.length() - pos + 1;
        char *buffer = ( char* ) malloc ( size );
        memcpy ( buffer, temp, size );
        rest = buffer;
        free ( buffer );
        pos = rest.find ( "/" );
    }
    return rest;
}
