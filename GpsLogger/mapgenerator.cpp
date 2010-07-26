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

#include "mapgenerator.h"

MapGenerator::MapGenerator() :
        projection ( "+proj=merc +a=6378137 +b=6378137 +lat_ts=0.0 +lon_0=0.0 +x_0=0.0 +y_0=0 +k=1.0 +units=m +nadgrids=@null +no_defs +over" ) {
    // Globals
    mapnik::datasource_cache::instance()->register_datasources ( "/usr/lib/mapnik/0.7/input/" );
    mapnik::freetype_engine::register_font ( "/usr/share/fonts/truetype/ttf-dejavu/DejaVuSans.ttf" );
    // Variables & Objects
    valid = false;
    width = 0;
    height = 0;
    latitude = 0;
    longitude = 0;
    zoom = 1;
    load_map ( map, "styles.xml" );
    addLayer ( map, "polygon" );
    addLayer ( map, "line" );
    addLayer ( map, "roads" );
    addLayer ( map, "point" );
    run = true;
    correct = true;
    counter = 0;
    pthread_cond_init ( &cond, NULL );
    pthread_mutex_init ( &mutex, NULL );
    pthread_create ( &thread, NULL, entryPoint, ( void* ) this );
}

MapGenerator::~MapGenerator() {
    pthread_mutex_lock ( &mutex );
    run = false;
    counter = 0;
    pthread_cond_signal ( &cond );
    pthread_mutex_unlock ( &mutex );
    pthread_join ( thread, NULL );
    pthread_mutex_destroy ( &mutex );
    pthread_cond_destroy ( &cond );
}

void MapGenerator::setDone ( Glib::Dispatcher *done ) {
    pthread_mutex_lock ( &mutex );
    this->done = done;
    pthread_mutex_unlock ( &mutex );
}

void MapGenerator::setSize ( int width, int height ) {
    this->width = width;
    this->height = height;
    map.resize ( width, height );
    adjustMap();
}

void MapGenerator::setCenter ( double latitude, double longitude ) {
    this->latitude = latitude;
    this->longitude = longitude;
    adjustMap();
}

void MapGenerator::setZoom ( int zoom ) {
    this->zoom = zoom;
    adjustMap();
}

Frame MapGenerator::doGenerate ( Cairo::RefPtr<Cairo::ImageSurface> &image ) {
    pthread_mutex_lock ( &mutex );
    Frame validFrame = this->frame;
    Cairo::RefPtr<Cairo::ImageSurface> validImage = this->image;
    pthread_mutex_unlock ( &mutex );
    Frame frame;
    frame.minLat = 0;
    frame.minLon = 0;
    frame.maxLat = height;
    frame.maxLon = width;
    mapnik::CoordTransform transform = map.view_transform();
    transform.backward ( &frame.minLon, &frame.minLat );
    transform.backward ( &frame.maxLon, &frame.maxLat );
    projection.inverse ( frame.minLon, frame.minLat );
    projection.inverse ( frame.maxLon, frame.maxLat );
    pthread_mutex_lock ( &mutex );
    correct = false;
    counter = 1000;
    candidate = frame;
    pthread_cond_signal ( &cond );
    pthread_mutex_unlock ( &mutex );
    image = Cairo::ImageSurface::create ( Cairo::FORMAT_ARGB32, width, height );
    Cairo::RefPtr<Cairo::Context> context = Cairo::Context::create ( image );
    context->rectangle ( 0, 0, width, height );
    context->set_source_rgb ( 0.75, 0.75, 0.75 );
    context->fill();
    if ( valid ) {
        double xKoef = width / ( frame.maxLon - frame.minLon );
        double xSize = ( validFrame.maxLon - validFrame.minLon ) * xKoef;
        double xScale = xSize / validImage->get_width();
        double xCorner = ( validFrame.minLon - frame.minLon ) * xKoef / xScale;
        double yKoef = height / ( frame.maxLat - frame.minLat );
        double ySize = ( validFrame.maxLat - validFrame.minLat ) * yKoef;
        double yScale = ySize / validImage->get_height();
        double yCorner = ( validFrame.minLat - frame.minLat ) * yKoef / yScale;
        context->scale ( xScale, yScale );
        context->set_source ( validImage, xCorner, yCorner );
        context->paint();
    }
    return frame;
}

Frame MapGenerator::doTake ( Cairo::RefPtr<Cairo::ImageSurface> &image ) {
    pthread_mutex_lock ( &mutex );
    Frame validFrame = this->frame;
    Cairo::RefPtr<Cairo::ImageSurface> validImage = this->image;
    pthread_mutex_unlock ( &mutex );
    image = Cairo::ImageSurface::create ( Cairo::FORMAT_ARGB32, width, height );
    if ( valid ) {
        Cairo::RefPtr<Cairo::Context> context = Cairo::Context::create ( image );
        context->set_source ( validImage, 0, 0 );
        context->paint();
    }
    return validFrame;
}

void* MapGenerator::entryPoint ( void *pointer ) {
    MapGenerator *mapGenerator = ( MapGenerator* ) pointer;
    mapGenerator->startThread();
    return NULL;
}

void MapGenerator::addLayer ( mapnik::Map &map, string name ) {
    mapnik::parameters parameters;
    parameters["type"] = "postgis";
    parameters["host"] = "localhost";
    parameters["port"] = 5432;
    parameters["dbname"] = "gis";
    parameters["table"] = "planet_osm_" + name;
    parameters["user"] = "gis";
    parameters["password"] = "gis";
    mapnik::Layer layer ( name );
    layer.set_datasource ( mapnik::datasource_cache::instance()->create ( parameters ) );
    layer.add_style ( name + "-border" );
    layer.add_style ( name + "-fill" );
    map.addLayer ( layer );
}

void MapGenerator::adjustMap() {
    double minLat = latitude;
    double minLon = longitude - 10.0 / zoom;
    double maxLat = latitude;
    double maxLon = longitude + 10.0 / zoom;
    projection.forward ( minLon, minLat );
    projection.forward ( maxLon, maxLat );
    mapnik::Envelope<double> envelope ( minLon, minLat, maxLon, maxLat );
    map.zoomToBox ( envelope );
}

void MapGenerator::startThread() {
    pthread_mutex_lock ( &mutex );
    while ( run ) {
        if ( correct ) {
            pthread_cond_wait ( &cond, &mutex );
        }
        while ( counter > 0 ) {
            pthread_mutex_unlock ( &mutex );
            usleep ( 1000 );
            pthread_mutex_lock ( &mutex );
            counter--;
        }
        if ( run ) {
            correct = true;
            Frame frame = candidate;
            pthread_mutex_unlock ( &mutex );
            Cairo::RefPtr<Cairo::ImageSurface> image;
            image = Cairo::ImageSurface::create ( Cairo::FORMAT_ARGB32, map.getWidth(), map.getHeight() );
            mapnik::cairo_renderer<Cairo::Surface> render ( map, image );
            render.apply();
            pthread_mutex_lock ( &mutex );
            if ( correct ) {
                this->frame = frame;
                this->image = image;
                valid = true;
                ( *done ) ();
            }
        }
    }
    pthread_mutex_unlock ( &mutex );
    pthread_exit ( NULL );
}
