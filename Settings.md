### Introduction ###

> All configuration is saved in file settings.xml. Most of values are system dependent constants. For example paths to fonts and Postgres login informations.

### Configuration sections ###

> Sections are:

  * **device** - communication with GPS hardware settings
  * **postgis** - informations needed for connection with PostGIS
  * **picasa** - web album export settings

### Values description ###

| **section** | **key** | **type** | **description** |
|:------------|:--------|:---------|:----------------|
| device | port | string | path to serial port with device |
| postgis | datasource | string | path to datasource directory, where file `postgis.input` is placed |
|  | font | string | path to font file, this font is used in map |
|  | usepostgis | bool | define if PostGIS serve is used and map is rendered |
|  | host | string | Postgres server hostname |
|  | port | int | Postgres server port number |
|  | database | string | PostGIS database name |
|  | user | string | some user permitted to access this database |
|  | password | string | user's password |
| picasa | user | string | username prefilled in login dialog |
|  | password | string | password prefilled in login dialog, not recommended |