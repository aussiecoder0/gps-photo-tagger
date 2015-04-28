### Operating system ###

> GPS Photo Tagger is primary written for Linux operating systems. I haven't tried compilation under Windows yet but I am going to make this application cross platform. But some time will be needed to develop mechanism for searching proper serial port.

### Download sources ###

> You can download latest version of source code using SVN.

> `svn checkout http://gps-photo-tagger.googlecode.com/svn/trunk/GpsLogger GpsLogger`

### Requirements ###

> Application is compiled by g++ compiler using included makefile. Application also uses few libraries which are needed for run and their development packages are required for compilation. Used libraries are gtkmm-2.4, exiv2, curl4, xml++-2.6, magick++9 and mapnik.

### Recommended packages for Ubuntu ###

  * subversion
  * make
  * astyle
  * g++
  * libgtkmm-2.4-dev
  * libexiv2-dev
  * libcurl4-openssl-dev
  * libxml++2.6-dev
  * libmagick++9-dev
  * libmapnik-dev

> `sudo apt-get install subversion make astyle g++ libgtkmm-2.4-dev libexiv2-dev libcurl4-openssl-dev libxml++2.6-dev libmagick++9-dev libmapnik-dev`

### Make options ###

  * **all** - compile whole project
  * **clean** - delete binary files
  * **format** - format source files