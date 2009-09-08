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

#include "picasaexport.h"

PicasaExport::PicasaExport() {
    // Variables & Objects
    ready = true;
    // Dispatchers
    progress.connect ( sigc::mem_fun ( *this, &PicasaExport::onProgress ) );
    done.connect ( sigc::mem_fun ( *this, &PicasaExport::onDone ) );
}

PicasaExport::~PicasaExport() {
}

void PicasaExport::doExport ( list<PhotoItem> photoList ) {
    if ( ready ) {
        ready = false;
        this->photoList = photoList;
        if ( auth.empty() ) {
            LoginWindow *loginWindow = new LoginWindow();
            loginWindow->signalDone().connect ( sigc::mem_fun ( *this, &PicasaExport::authorize ) );
        } else {
            httpClient.headersClear();
            httpClient.headersAdd ( "GData-Version: 2" );
            httpClient.headersAdd ( "Authorization: GoogleLogin auth=" + auth );
            listAlbums();
        }
    }
}

PicasaExport::PicasaParser::PicasaParser() {
}

PicasaExport::PicasaParser::~PicasaParser() {
}

list<AlbumItem> PicasaExport::PicasaParser::getAlbumList() {
    return albums;
}

string PicasaExport::PicasaParser::getLastLink() {
    return link;
}

void PicasaExport::PicasaParser::on_start_document() {
    inEntry = false;
    inTitle = false;
    albums.clear();
}

void PicasaExport::PicasaParser::on_start_element ( const Glib::ustring& name, const AttributeList& attributes ) {
    if ( name == "entry" ) {
        inEntry = true;
        link = "";
        title = "";
    }
    if ( inEntry ) {
        if ( name == "link" ) {
            string rel = "";
            string href = "";
            for ( xmlpp::SaxParser::AttributeList::const_iterator iter = attributes.begin(); iter != attributes.end(); ++iter ) {
                if ( iter->name == "rel" ) {
                    rel = iter->value;
                }
                if ( iter->name == "href" ) {
                    href = iter->value;
                }
            }
            if ( rel == "http://schemas.google.com/g/2005#feed" ) {
                link = href;
            }
        }
        if ( name == "title" ) {
            inTitle = true;
            title = "";
        }
    }
}

void PicasaExport::PicasaParser::on_end_element ( const Glib::ustring& name ) {
    if ( name == "entry" ) {
        inEntry = false;
        AlbumItem item;
        item.title = title;
        item.link = link;
        albums.push_back ( item );
    }
    if ( name == "title" ) {
        inTitle = false;
    }
}

void PicasaExport::PicasaParser::on_characters ( const Glib::ustring& text ) {
    if ( inTitle ) {
        title += text;
    }
}

void* PicasaExport::entryPoint ( void *pointer ) {
    PicasaExport *picasaExport = ( PicasaExport* ) pointer;
    picasaExport->start();
    return NULL;
}

void PicasaExport::authorize ( bool success, string name, string password, LoginWindow *loginWindow ) {
    if ( success ) {
        httpClient.headersClear();
        httpClient.formClear();
        httpClient.formAddString ( "accountType", "HOSTED_OR_GOOGLE" );
        httpClient.formAddString ( "Email", name );
        httpClient.formAddString ( "Passwd", password );
        httpClient.formAddString ( "service", "lh2" );
        httpClient.formAddString ( "source", "jakubVanik-gpsPhotoTagger-beta" );
        string response = httpClient.httpPostForm ( "https://www.google.com/accounts/ClientLogin" );
        auth = extractAuth ( response );
        if ( auth.empty() ) {
            Gtk::MessageDialog dialog ( *loginWindow, "Can't login", false, Gtk::MESSAGE_ERROR, Gtk::BUTTONS_OK );
            dialog.set_secondary_text ( "Please check your username and password and try again." );
            dialog.run();
        } else {
            delete ( loginWindow );
            httpClient.headersAdd ( "GData-Version: 2" );
            httpClient.headersAdd ( "Authorization: GoogleLogin auth=" + auth );
            listAlbums();
        }
    } else {
        delete ( loginWindow );
        ready = true;
    }
}

string PicasaExport::extractAuth ( string response ) {
    string rest = response;
    while ( rest.empty() == false ) {
        int pos = rest.find ( "\n" );
        if ( pos == -1 ) {
            pos = rest.length() - 1;
        }
        const char *line = rest.c_str();
        char comp[6];
        memcpy ( comp, line, 5 );
        comp[5] = 0;
        if ( strcmp ( comp, "Auth=" ) == 0 ) {
            line += 5;
            int length = pos - 5;
            char *buffer = ( char* ) malloc ( length + 1 );
            memcpy ( buffer, line, length );
            buffer[length] = 0;
            string auth = buffer;
            free ( buffer );
            return auth;
        } else {
            line += pos + 1;
            int length = strlen ( line );
            char *buffer = ( char* ) malloc ( length + 1 );
            memcpy ( buffer, line, length );
            buffer[length] = 0;
            rest = buffer;
            free ( buffer );
        }
    }
    return "";
}

void PicasaExport::listAlbums() {
    string response = httpClient.httpGet ( "http://picasaweb.google.com/data/feed/api/user/default" );
    picasaParser.parse_chunk ( response );
    list<AlbumItem> albums = picasaParser.getAlbumList();
    SetupWindow *setupWindow = new SetupWindow ( albums, photoList.size() );
    setupWindow->signalDone().connect ( sigc::mem_fun ( *this, &PicasaExport::upload ) );
}

void PicasaExport::upload ( bool success, bool existing, string album, bool titles, SetupWindow *setupWindow ) {
    if ( success ) {
        if ( existing == false ) {
            time_t timeNow = time ( NULL );
            string request = "<entry xmlns='http://www.w3.org/2005/Atom' xmlns:media='http://search.yahoo.com/mrss/' xmlns:gphoto='http://schemas.google.com/photos/2007'>"
                             "<title type='text'>" + album + "</title>"
                             "<summary type='text'></summary>"
                             "<gphoto:location></gphoto:location>"
                             "<gphoto:access>private</gphoto:access>"
                             "<gphoto:timestamp>" + itos ( timeNow ) + "000</gphoto:timestamp>"
                             "<media:group>"
                             "<media:keywords></media:keywords>"
                             "</media:group>"
                             "<category scheme='http://schemas.google.com/g/2005#kind' term='http://schemas.google.com/photos/2007#album'></category>"
                             "</entry>";
            httpClient.headersClear();
            httpClient.headersAdd ( "GData-Version: 2" );
            httpClient.headersAdd ( "Authorization: GoogleLogin auth=" + auth );
            httpClient.headersAdd ( "Content-Type: application/atom+xml" );
            string response = httpClient.httpPost ( "http://picasaweb.google.com/data/feed/api/user/default", request );
            PicasaParser *parser = new PicasaParser();
            parser->parse_chunk ( response );
            uploadLink = parser->getLastLink();
            delete ( parser );
        } else {
            uploadLink = album;
        }
        this->titles = titles;
        this->setupWindow = setupWindow;
        pthread_create ( &thread, NULL, entryPoint, ( void* ) this );
    } else {
        delete ( setupWindow );
        ready = true;
    }
}

void PicasaExport::start() {
    list<PhotoItem>::iterator i;
    for ( i = photoList.begin(); i != photoList.end(); ++i ) {
        PhotoItem photoItem = *i;
        string fileName = extractFileName ( photoItem.path );
        string title;
        if ( titles ) {
            title = fileName;
        }
        int length = 0;
        char *request = NULL;
        int position = 0;
        char *temp = NULL;
        string meta;
        meta += "Media multipart posting\r\n"
                "--END_OF_PART\r\n"
                "Content-Type: application/atom+xml\r\n"
                "\r\n";
        meta += "<entry xmlns='http://www.w3.org/2005/Atom' xmlns:georss='http://www.georss.org/georss' xmlns:gml='http://www.opengis.net/gml'>"
                "<title>" + fileName + "</title>"
                "<summary>" + title + "</summary>"
                "<category scheme='http://schemas.google.com/g/2005#kind' term='http://schemas.google.com/photos/2007#photo'/>";
        if ( photoItem.hasPos ) {
            meta += "<georss:where>"
                    "<gml:Point>"
                    "<gml:pos>" + dtos ( photoItem.latitude ) + " " + dtos ( photoItem.longitude ) + "</gml:pos>"
                    "</gml:Point>"
                    "</georss:where>";
        }
        meta += "</entry>";
        meta += "\r\n"
                "--END_OF_PART\r\n"
                "Content-Type: image/jpeg\r\n"
                "\r\n";
        length = meta.length();
        request = ( char* ) malloc ( length );
        memcpy ( request, meta.c_str(), length );
        position += length;
        char buffer[1024];
        FILE *file = fopen ( photoItem.path.c_str(), "r" );
        while ( true ) {
            length = fread ( buffer, 1, 1024, file );
            if ( length == 0 ) {
                break;
            }
            request = ( char* ) realloc ( request, position + length );
            temp = request + position;
            memcpy ( temp, buffer, length );
            position += length;
        }
        fclose ( file );
        string foot;
        foot += "\r\n"
                "--END_OF_PART--";
        length = foot.length();
        request = ( char* ) realloc ( request, position + length );
        temp = request + position;
        memcpy ( temp, foot.c_str(), length );
        position += length;
        httpClient.headersClear();
        httpClient.headersAdd ( "GData-Version: 2" );
        httpClient.headersAdd ( "Authorization: GoogleLogin auth=" + auth );
        httpClient.headersAdd ( "Content-Type: multipart/related; boundary=\"END_OF_PART\"" );
        httpClient.headersAdd ( "Content-Length: " + itos ( position ) );
        string response = httpClient.httpPost ( uploadLink, request, position );
        free ( request );
        progress();
    }
    done();
}

void PicasaExport::onProgress() {
    setupWindow->doIncrease();
}

void PicasaExport::onDone() {
    Gtk::MessageDialog dialog ( *setupWindow, "Upload has been finished", false, Gtk::MESSAGE_INFO, Gtk::BUTTONS_OK );
    dialog.set_secondary_text ( "Your album has been successfully uploaded. You can login to Picasa Web now and publish your album." );
    dialog.run();
    delete ( setupWindow );
    ready = true;
}