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

#include "settings.h"

SettingsSection::SettingsSection ( string name ) {
    this->name = name;
}

SettingsSection::~SettingsSection() {
}

string SettingsSection::getName() {
    return name;
}

bool SettingsSection::getBool ( string key ) {
    vector<MultiType>::iterator iterator = data.begin();
    while ( iterator != data.end() ) {
        if ( iterator->key.compare ( key ) == 0 ) {
            return iterator->valBool;
        }
        iterator++;
    }
    return false;
}

int SettingsSection::getInt ( string key ) {
    vector<MultiType>::iterator iterator = data.begin();
    while ( iterator != data.end() ) {
        if ( iterator->key.compare ( key ) == 0 ) {
            return iterator->valInt;
        }
        iterator++;
    }
    return 0;
}

double SettingsSection::getDouble ( string key ) {
    vector<MultiType>::iterator iterator = data.begin();
    while ( iterator != data.end() ) {
        if ( iterator->key.compare ( key ) == 0 ) {
            return iterator->valDouble;
        }
        iterator++;
    }
    return 0;
}

string SettingsSection::getString ( string key ) {
    vector<MultiType>::iterator iterator = data.begin();
    while ( iterator != data.end() ) {
        if ( iterator->key.compare ( key ) == 0 ) {
            return iterator->valString;
        }
        iterator++;
    }
    return "";
}

void SettingsSection::setBool ( string key, bool valBool ) {
    MultiType value;
    value.key = key;
    value.valBool = valBool;
    data.push_back ( value );
}

void SettingsSection::setInt ( string key, int valInt ) {
    MultiType value;
    value.key = key;
    value.valInt = valInt;
    data.push_back ( value );
}

void SettingsSection::setDouble ( string key, double valDouble ) {
    MultiType value;
    value.key = key;
    value.valDouble = valDouble;
    data.push_back ( value );
}

void SettingsSection::setString ( string key, string valString ) {
    MultiType value;
    value.key = key;
    value.valString = valString;
    data.push_back ( value );
}

SettingsManager::SettingsManager() :
        settingsParser ( this ) {
    settingsParser.parse_file ( "settings.xml" );
}

SettingsManager::~SettingsManager() {
    vector<SettingsSection*>::iterator iterator = sections.begin();
    while ( iterator != sections.end() ) {
        SettingsSection *section = *iterator;
        delete ( section );
        iterator++;
    }
}

SettingsSection *SettingsManager::getSection ( string name ) {
    vector<SettingsSection*>::iterator iterator = sections.begin();
    while ( iterator != sections.end() ) {
        SettingsSection *section = *iterator;
        if ( section->getName().compare ( name ) == 0 ) {
            return section;
        }
        iterator++;
    }
    return NULL;
}

SettingsManager::SettingsParser::SettingsParser ( SettingsManager* settings ) {
    this->settings = settings;
}

SettingsManager::SettingsParser::~SettingsParser() {
}

void SettingsManager::SettingsParser::on_start_document() {
}

void SettingsManager::SettingsParser::on_start_element ( const Glib::ustring& name, const AttributeList& attributes ) {
    if ( name == "section" ) {
        for ( xmlpp::SaxParser::AttributeList::const_iterator iter = attributes.begin(); iter != attributes.end(); ++iter ) {
            if ( iter->name == "name" ) {
                section = iter->value;
            }
        }
        settings->openSection = new SettingsSection ( section );
        settings->sections.push_back ( settings->openSection );
    }
    if ( name == "value" ) {
        for ( xmlpp::SaxParser::AttributeList::const_iterator iter = attributes.begin(); iter != attributes.end(); ++iter ) {
            if ( iter->name == "key" ) {
                key = iter->value;

            }
            if ( iter->name == "type" ) {
                type = iter->value;
            }
        }
        value.clear();
    }
}

void SettingsManager::SettingsParser::on_end_element ( const Glib::ustring& name ) {
    if ( name == "section" ) {
        settings->openSection = NULL;
    }
    if ( name == "value" ) {
        if ( type.compare ( "bool" ) == 0 ) {
            bool valBool = ( value.compare ( "true" ) == 0 );
            settings->openSection->setBool ( key, valBool );
        }
        if ( type.compare ( "int" ) == 0 ) {
            int valInt = atoi ( value.c_str() );
            settings->openSection->setInt ( key, valInt );
        }
        if ( type.compare ( "double" ) == 0 ) {
            int valDouble = atof ( value.c_str() );
            settings->openSection->setDouble ( key, valDouble );
        }
        if ( type.compare ( "string" ) == 0 ) {
            settings->openSection->setString ( key, value );
        }
    }
}

void SettingsManager::SettingsParser::on_characters ( const Glib::ustring& text ) {
    value += text;
}

int Settings::counter = 0;
SettingsManager *Settings::manager = NULL;

Settings::Settings() {
    if ( counter == 0 ) {
        manager = new SettingsManager();
    }
    counter++;
}

Settings::~Settings() {
    counter--;
    if ( counter == 0 ) {
        delete ( manager );
        manager = NULL;
    }
}

SettingsSection *Settings::getSection ( string name ) {
    if ( counter != 0 ) {
        return manager->getSection ( name );
    } else {
        return NULL;
    }
}
