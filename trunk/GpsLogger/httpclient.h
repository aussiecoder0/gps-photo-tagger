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

#ifndef HTTPCLIENT_H_
#define HTTPCLIENT_H_

#include <cstdlib>
#include <cstring>
#include <sstream>
#include <curl.h>

using namespace std;

class HttpClient {
public:
    // Constructor & Destructor
    HttpClient();
    virtual ~HttpClient();
    // Getters & Setters
    void headersClear();
    void headersAdd ( string );
    void formClear();
    void formAddString ( string, string );
    // Action calls
    string httpGet ( string );
    string httpPost ( string, string );
    string httpPost ( string, char*, int );
    string httpPostForm ( string );
private:
    // Structs & Classes
    struct DownloadStruct {
        char *memory;
        size_t size;
    };
    struct UploadStruct {
        char *memory;
        int length;
        int position;
    };
    // Static entrypoints
    static size_t writer ( void*, size_t, size_t, void* );
    static size_t reader ( void*, size_t, size_t, void* );
    // Variables & Objects
    struct curl_slist *headerlist;
    struct curl_httppost *formpost;
    struct curl_httppost *lastptr;
};

#endif /*HTTPCLIENT_H_*/
