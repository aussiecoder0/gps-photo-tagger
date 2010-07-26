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

#ifndef PICASAEXPORT_H_
#define PICASAEXPORT_H_

#include <cstdlib>
#include <cstring>
#include <queue>
#include <exiv2/exif.hpp>
#include <exiv2/image.hpp>
#include <gtkmm-2.4/gtkmm.h>
#include <ImageMagick/Magick++.h>
#include <libxml++-2.6/libxml++/libxml++.h>
#include "functions.h"
#include "httpclient.h"
#include "photowidget.h"
#include "picasawindows.h"

using namespace std;

class PicasaExport {
public:
    // Constructor & Destructor
    PicasaExport();
    virtual ~PicasaExport();
    // Action calls
    void doExport ( list<PhotoItem> );
private:
    // Structs & Classes
    class PicasaParser : public xmlpp::SaxParser {
    public:
        PicasaParser();
        virtual ~PicasaParser();
        list<AlbumItem> getAlbumList();
        string getLastLink();
    private:
        bool inEntry, inTitle;
        string link, title;
        list<AlbumItem> albums;
    protected:
        virtual void on_start_document();
        virtual void on_start_element ( const Glib::ustring& name, const AttributeList& properties );
        virtual void on_end_element ( const Glib::ustring& name );
        virtual void on_characters ( const Glib::ustring& characters );
    };
    struct UploadData {
        string link;
        char *data;
        int size;
    };
    // Static entrypoints
    static void* entryPoint1 ( void* );
    static void* entryPoint2 ( void* );
    // Variables & Objects
    bool ready;
    list<PhotoItem> photoList;
    HttpClient httpClient;
    PicasaParser picasaParser;
    string auth;
    string uploadLink;
    bool titles;
    int dimension;
    SetupWindow *setupWindow;
    pthread_t thread1;
    pthread_t thread2;
    pthread_mutex_t mutex;
    bool queueDone;
    queue<UploadData> uploadQueue;
    // Functions
    void authorize ( bool, string, string, LoginWindow* );
    string extractAuth ( string );
    void listAlbums();
    void upload ( bool, bool, string, bool, int, SetupWindow* );
    void start1();
    void start2();
    // Dispatchers
    Glib::Dispatcher progress;
    Glib::Dispatcher done;
    // Dispatcher signals
    void onProgress();
    void onDone();
};

#endif /*PICASAEXPORT_H_*/
