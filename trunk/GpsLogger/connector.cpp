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

#include "connector.h"

Connector::Connector() {
    // Variables & Objects
    run = true;
    clear = false;
    write = false;
    read = false;
    first = NULL;
    info = ( skytraq_config* ) malloc ( sizeof ( skytraq_config ) );
    pthread_mutex_init ( &mutex, NULL );
    pthread_create ( &thread, NULL, entryPoint, ( void* ) this );
}

Connector::~Connector() {
    run = false;
    sleep ( 1 );
    free ( info );
}

bool Connector::isConnected() {
    return run;
}

int Connector::getPointer() {
    pthread_mutex_lock ( &mutex );
    int pointer = info->log_wr_ptr;
    pthread_mutex_unlock ( &mutex );
    return pointer;
}

void Connector::getSectors ( int &total, int &left ) {
    pthread_mutex_lock ( &mutex );
    total = info->total_sectors;
    left = info->sectors_left;
    pthread_mutex_unlock ( &mutex );
}

int Connector::getTime() {
    pthread_mutex_lock ( &mutex );
    int time = info->min_time;
    pthread_mutex_unlock ( &mutex );
    return time;
}

void Connector::setTime ( int time ) {
    if ( time > -1 ) {
        pthread_mutex_lock ( &mutex );
        info->min_time = time;
        pthread_mutex_unlock ( &mutex );
    }
}

int Connector::getDistance() {
    pthread_mutex_lock ( &mutex );
    int distance = info->min_distance;
    pthread_mutex_unlock ( &mutex );
    return distance;
}

void Connector::setDistance ( int distance ) {
    if ( distance > -1 ) {
        pthread_mutex_lock ( &mutex );
        info->min_distance = distance;
        pthread_mutex_unlock ( &mutex );
    }
}

bool Connector::getDatalog() {
    pthread_mutex_lock ( &mutex );
    bool datalog = info->datalog_enable;
    pthread_mutex_unlock ( &mutex );
    return datalog;
}

void Connector::setDatalog ( bool datalog ) {
    pthread_mutex_lock ( &mutex );
    info->datalog_enable = datalog;
    pthread_mutex_unlock ( &mutex );
}

bool Connector::getFifo() {
    pthread_mutex_lock ( &mutex );
    bool fifo = info->log_fifo_mode;
    pthread_mutex_unlock ( &mutex );
    return fifo;
}

void Connector::setFifo ( bool fifo ) {
    pthread_mutex_lock ( &mutex );
    info->log_fifo_mode = fifo;
    pthread_mutex_unlock ( &mutex );
}

LogEntry* Connector::GetFirstEntry() {
    return first;
}

void Connector::doClear() {
    pthread_mutex_lock ( &mutex );
    clear = true;
    pthread_mutex_unlock ( &mutex );
}

void Connector::doWrite() {
    pthread_mutex_lock ( &mutex );
    write = true;
    pthread_mutex_unlock ( &mutex );
}

int Connector::doRead ( Glib::Dispatcher *progress, Glib::Dispatcher *done ) {
    if ( run == false ) {
        return -1;
    }
    pthread_mutex_lock ( &mutex );
    this->progress = progress;
    this->done = done;
    read = true;
    int sectorsCount = info->total_sectors - info->sectors_left + 1;
    pthread_mutex_unlock ( &mutex );
    return sectorsCount;
}

void* Connector::entryPoint ( void *pointer ) {
    Connector *connector = ( Connector* ) pointer;
    connector->start();
    return NULL;
}

void Connector::start() {
    char name[] = "/dev/ttyUSB0";
    info = ( skytraq_config* ) malloc ( sizeof ( skytraq_config ) );
    pthread_mutex_lock ( &mutex );
    device = open_port ( name );
    if ( device != -1 ) {
        speed = skytraq_determine_speed ( device );
        if ( speed != 0 ) {
            skytraq_set_serial_speed ( device, 5, false );
            speed = skytraq_determine_speed ( device );
            if ( speed != 0 ) {
                skytraq_read_datalogger_config ( device, info );
                skytraq_read_agps_status ( device, info );
            } else {
                run = false;
            }
        } else {
            run = false;
        }
    } else {
        run = false;
    }
    pthread_mutex_unlock ( &mutex );
    while ( run ) {
        pthread_mutex_lock ( &mutex );
        if ( clear ) {
            clear = false;
            skytraq_clear_datalog ( device );
            skytraq_read_datalogger_config ( device, info );
        }
        if ( write ) {
            write = false;
            skytraq_write_datalogger_config ( device, info );
        }
        if ( read ) {
            read = false;
            first = download();
            ( *done ) ();
        }
        pthread_mutex_unlock ( &mutex );
        sleep ( 1 );
    }
    free ( info );
    skytraq_set_serial_speed ( device, 1, false );
    speed = skytraq_determine_speed ( device );
    close ( device );
}

LogEntry* Connector::download() {
    LogEntry *first = NULL;
    LogEntry *last = NULL;
    int sectorsCount = info->total_sectors - info->sectors_left + 1;
    for ( int i = 0; i < sectorsCount; i++ ) {
        gbuint8* buffer = ( gbuint8* ) malloc ( 4100 );
        int length = skytraq_read_datalog_sector ( device, i, buffer );
        int offset = 0;
        long time;
        int x, y, z, speed;
        double latitude, longitude, height;
        bool valid;
        while ( offset < length ) {
            if ( buffer[offset] == 0x40 ) {
                decode_long_entry ( buffer + offset,  &time, &x, &y, &z, &speed );
                ecef_to_geo ( x, y, z, &longitude, &latitude, &height );
                valid = true;
                offset += 18;
            } else if (  buffer[offset] == 0x80 ) {
                decode_short_entry ( buffer + offset,  &time, &x, &y, &z, &speed );
                ecef_to_geo ( x, y, z, &longitude, &latitude, &height );
                valid = true;
                offset += 8;
            } else {
                valid = false;
                offset++;
            }
            if ( valid ) {
                LogEntry *next = new LogEntry();
                next->time = time;
                next->speed = speed;
                next->latitude = latitude;
                next->longitude = longitude;
                next->height = height;
                next->photo = -1;
                next->next = NULL;
                next->nextDay = NULL;
                next->nextVisible = NULL;
                if ( last == NULL ) {
                    first = next;
                    last = next;
                } else {
                    last->next = next;
                    last->nextVisible = next;
                    last = next;
                }
            }
        }
        free ( buffer );
        ( *progress ) ();
        sleep ( 1 );
    }
    return first;
}
