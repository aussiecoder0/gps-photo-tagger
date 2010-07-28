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

#ifndef SETTINGS_H_
#define SETTINGS_H_

#include <cstdlib>
#include <libxml++-2.6/libxml++/libxml++.h>

using namespace std;

class SettingsSection {
public:
    // Constructor & Destructor
    SettingsSection ( string );
    virtual ~SettingsSection();
    // Getters & Setters
    string getName();
    bool getBool ( string );
    int getInt ( string );
    double getDouble ( string );
    string getString ( string );
    void setBool ( string, bool );
    void setInt ( string, int );
    void setDouble ( string, double );
    void setString ( string, string );
private:
    // Structs & Classes
    class MultiType {
    public:
        string key;
        bool valBool;
        int valInt;
        double valDouble;
        string valString;
    };
    // Variables & Objects
    string name;
    vector<MultiType> data;
};

class SettingsManager {
public:
    // Constructor & Destructor
    SettingsManager();
    virtual ~SettingsManager();
    // Getters & Setters
    SettingsSection *getSection ( string );
private:
    // Structs & Classes
    class SettingsParser: public xmlpp::SaxParser {
    public:
        SettingsParser ( SettingsManager* );
        virtual ~SettingsParser();
    private:
        SettingsManager *settings;
        string section;
        string key;
        string type;
        string value;
    protected:
        virtual void on_start_document();
        virtual void on_start_element ( const Glib::ustring& name, const AttributeList& properties );
        virtual void on_end_element ( const Glib::ustring& name );
        virtual void on_characters ( const Glib::ustring& characters );
    };
    // Variables & Objects
    SettingsParser settingsParser;
    SettingsSection *openSection;
    vector<SettingsSection*> sections;
};

class Settings {
public:
    // Constructor & Destructor
    Settings();
    virtual ~Settings();
    // Getters & Setters
    static SettingsSection *getSection ( string );
private:
    // Variables & Objects
    static int counter;
    static SettingsManager *manager;
};

#endif /* SETTINGS_H_ */
