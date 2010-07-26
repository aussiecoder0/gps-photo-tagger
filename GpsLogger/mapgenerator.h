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

#ifndef MAPGENERATOR_H_
#define MAPGENERATOR_H_

#define HAVE_CAIRO

#include <cstring>
#include <gtkmm-2.4/gtkmm.h>
#include <mapnik/cairo_renderer.hpp>
#include <mapnik/config_error.hpp>
#include <mapnik/coord.hpp>
#include <mapnik/datasource_cache.hpp>
#include <mapnik/font_engine_freetype.hpp>
#include <mapnik/image_util.hpp>
#include <mapnik/load_map.hpp>
#include <mapnik/map.hpp>
#include <cairomm/surface.h>
#include "functions.h"

using namespace std;

class Frame {
public:
    double minLat;
    double minLon;
    double maxLat;
    double maxLon;
};

class MapGenerator {
public:
    // Constructor & Destructor
    MapGenerator();
    virtual ~MapGenerator();
    // Getters & Setters
    void setDone ( Glib::Dispatcher* );
    void setSize ( int, int );
    void setCenter ( double, double );
    void setZoom ( int );
    // Action calls
    Frame doGenerate ( Cairo::RefPtr<Cairo::ImageSurface>& );
    Frame doTake ( Cairo::RefPtr<Cairo::ImageSurface>& );
private:
    // Static entrypoints
    static void* entryPoint ( void* );
    // Variables & Objects
    Frame frame;
    Cairo::RefPtr<Cairo::ImageSurface> image;
    bool valid;
    int width, height;
    double latitude, longitude;
    int zoom;
    mapnik::Map map;
    mapnik::projection projection;
    bool run, correct;
    int counter;
    Frame candidate;
    Glib::Dispatcher *done;
    pthread_t thread;
    pthread_mutex_t mutex;
    pthread_cond_t cond;
    // Functions
    void addLayer ( mapnik::Map&, string );
    void adjustMap();
    void startThread();
};

#endif /* MAPGENERATOR_H_ */
