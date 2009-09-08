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

#ifndef _CONNECTOR_H
#define	_CONNECTOR_H

#include <gtkmm.h>
#include "datalogdecode.h"
#include "datalogger.h"
#include "lowlevel.h"

using namespace std;

class LogEntry {
public:
    long time;
    int speed;
    double latitude, longitude, height;
    int photo;
    LogEntry *next;
    LogEntry *nextDay;
    LogEntry *nextVisible;
};

class Connector {
public:
    // Constructor & Destructor
    Connector ();
    virtual ~Connector();
    // Getters & Setters
    bool isConnected();
    int getPointer();
    void getSectors ( int&, int& );
    int getTime();
    void setTime ( int );
    int getDistance();
    void setDistance ( int );
    bool getDatalog();
    void setDatalog ( bool );
    bool getFifo();
    void setFifo ( bool );
    LogEntry* GetFirstEntry();
    // Action calls
    void doClear();
    void doWrite();
    int doRead ( Glib::Dispatcher*, Glib::Dispatcher* );
private:
    // Static entrypoints
    static void* entryPoint ( void* );
    // Functions
    void start();
    LogEntry* download();
    // Variables & Objects
    bool run, clear, write, read;
    LogEntry* first;
    skytraq_config *info;
    Glib::Dispatcher *progress;
    Glib::Dispatcher *done;
    pthread_t thread;
    pthread_mutex_t mutex;
    int device, speed;
};

#endif	/* _CONNECTOR_H */
