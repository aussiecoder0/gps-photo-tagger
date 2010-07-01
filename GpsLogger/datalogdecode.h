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

#ifndef DATALOGDECODE_H_
#define DATALOGDECODE_H_

#include "datalogger.h"

using namespace std;

void ecef_to_geo ( double, double, double, double*, double*, double* );
unsigned long gsp_time_to_timestamp ( int, int );
void decode_long_entry ( const gbuint8*, long*, int*, int*, int*, int* );
void decode_short_entry ( const gbuint8*, long*, int*, int*, int*, int* );

#endif /*DATALOGDECODE_H_*/
