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

#include "httpclient.h"

HttpClient::HttpClient() {
    // Variables & Objects
    headerlist = NULL;
    formpost = NULL;
    lastptr = NULL;
    // Finally
    curl_global_init ( CURL_GLOBAL_ALL );
}

HttpClient::~HttpClient() {
    curl_slist_free_all ( headerlist );
    curl_formfree ( formpost );
}

void HttpClient::headersClear() {
    curl_slist_free_all ( headerlist );
    headerlist = NULL;
}

void HttpClient::headersAdd ( string headers ) {
    headerlist = curl_slist_append ( headerlist, headers.c_str() );
}

void HttpClient::formClear() {
    curl_formfree ( formpost );
    formpost = NULL;
    lastptr = NULL;
}

void HttpClient::formAddString ( string name, string content ) {
    curl_formadd ( &formpost, &lastptr, CURLFORM_COPYNAME, name.c_str(), CURLFORM_COPYCONTENTS, content.c_str(), CURLFORM_END );
}

string HttpClient::httpGet ( string url ) {
    struct DownloadStruct download;
    download.memory = NULL;
    download.size = 0;
    CURL *curl = curl_easy_init();
    curl_easy_setopt ( curl, CURLOPT_URL, url.c_str() );
    curl_easy_setopt ( curl, CURLOPT_HTTPHEADER, headerlist );
    curl_easy_setopt ( curl, CURLOPT_WRITEFUNCTION, writer );
    curl_easy_setopt ( curl, CURLOPT_WRITEDATA, ( void* ) &download );
    curl_easy_perform ( curl );
    curl_easy_cleanup ( curl );
    stringstream out;
    out << download.memory;
    return out.str();
}

string HttpClient::httpPost ( string url, string data ) {
    struct DownloadStruct download;
    download.memory = NULL;
    download.size = 0;
    CURL *curl = curl_easy_init();
    curl_easy_setopt ( curl, CURLOPT_URL, url.c_str() );
    curl_easy_setopt ( curl, CURLOPT_HTTPHEADER, headerlist );
    curl_easy_setopt ( curl, CURLOPT_POST, 1 );
    curl_easy_setopt ( curl, CURLOPT_POSTFIELDS, data.c_str() );
    curl_easy_setopt ( curl, CURLOPT_WRITEFUNCTION, writer );
    curl_easy_setopt ( curl, CURLOPT_WRITEDATA, ( void* ) &download );
    curl_easy_perform ( curl );
    curl_easy_cleanup ( curl );
    stringstream out;
    out << download.memory;
    return out.str();
}

string HttpClient::httpPost ( string url, char *data, int length ) {
    struct DownloadStruct download;
    download.memory = NULL;
    download.size = 0;
    struct UploadStruct upload;
    upload.memory = data;
    upload.position = 0;
    upload.length = length;
    CURL *curl = curl_easy_init();
    curl_easy_setopt ( curl, CURLOPT_URL, url.c_str() );
    curl_easy_setopt ( curl, CURLOPT_HTTPHEADER, headerlist );
    curl_easy_setopt ( curl, CURLOPT_POST, 1 );
    curl_easy_setopt ( curl, CURLOPT_READFUNCTION, reader );
    curl_easy_setopt ( curl, CURLOPT_READDATA, ( void* ) &upload );
    curl_easy_setopt ( curl, CURLOPT_WRITEFUNCTION, writer );
    curl_easy_setopt ( curl, CURLOPT_WRITEDATA, ( void* ) &download );
    curl_easy_perform ( curl );
    curl_easy_cleanup ( curl );
    stringstream out;
    out << download.memory;
    return out.str();
}

string HttpClient::httpPostForm ( string url ) {
    struct DownloadStruct download;
    download.memory = NULL;
    download.size = 0;
    CURL *curl = curl_easy_init();
    curl_easy_setopt ( curl, CURLOPT_URL, url.c_str() );
    curl_easy_setopt ( curl, CURLOPT_HTTPHEADER, headerlist );
    curl_easy_setopt ( curl, CURLOPT_HTTPPOST, formpost );
    curl_easy_setopt ( curl, CURLOPT_WRITEFUNCTION, writer );
    curl_easy_setopt ( curl, CURLOPT_WRITEDATA, ( void* ) &download );
    curl_easy_perform ( curl );
    curl_easy_cleanup ( curl );
    stringstream out;
    out << download.memory;
    return out.str();
}

size_t HttpClient::writer ( void *pointer, size_t size, size_t blocks, void *data ) {
    size_t realsize = size * blocks;
    struct DownloadStruct *download = ( struct DownloadStruct* ) data;
    if ( download->memory == NULL ) {
        download->memory = ( char* ) malloc ( realsize + 1 );
    } else {
        download->memory = ( char* ) realloc ( download->memory, download->size + realsize + 1 );
    }
    if ( download->memory ) {
        memcpy ( & ( download->memory[download->size] ), pointer, realsize );
        download->size += realsize;
        download->memory[download->size] = 0;
    }
    return realsize;
}

size_t HttpClient::reader ( void *pointer, size_t size, size_t blocks, void *data ) {
    size_t realsize = size * blocks;
    struct UploadStruct *upload = ( struct UploadStruct* ) data;
    size_t remains = upload->length - upload->position;
    if ( remains < realsize ) {
        realsize = remains;
    }
    char *temp = upload->memory + upload->position;
    memcpy ( pointer, temp, realsize );
    upload->position += realsize;
    return realsize;
}
