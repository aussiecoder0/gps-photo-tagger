makefile: gpslogger

format:
	astyle --style=kr -pPn colorrenderrer.h connector.h datalogger.h downloadwindow.h httpclient.h mainwindow.h mapgenerator.h photowidget.h picasaexport.h settings.h settingswindow.h trackwidget.h datalogdecode.h dayview.h functions.h lowlevel.h mapwidget.h photowindow.h picasawindows.h timewindow.h trackwindow.h
	astyle --style=kr -pPn colorrenderrer.cpp connector.cpp datalogdecode.cpp datalogger.cpp dayview.cpp downloadwindow.cpp functions.cpp httpclient.cpp lowlevel.cpp main.cpp mainwindow.cpp mapgenerator.cpp mapwidget.cpp photowidget.cpp photowindow.cpp picasaexport.cpp picasawindows.cpp settings.cpp settingswindow.cpp timewindow.cpp trackwidget.cpp trackwindow.cpp

all: gpslogger

gpslogger: colorrenderrer.o connector.o datalogdecode.o datalogger.o dayview.o downloadwindow.o functions.o httpclient.o lowlevel.o main.o mainwindow.o mapgenerator.o mapwidget.o photowidget.o photowindow.o picasaexport.o picasawindows.o settings.o settingswindow.o timewindow.o trackwidget.o trackwindow.o
	g++ -o"gpslogger" `pkg-config --libs gtkmm-2.4 exiv2 libxml++-2.6` `curl-config --libs` `Magick++-config --libs` -lboost_thread -lmapnik colorrenderrer.o connector.o datalogdecode.o datalogger.o dayview.o downloadwindow.o functions.o httpclient.o lowlevel.o main.o mainwindow.o mapgenerator.o mapwidget.o photowidget.o photowindow.o picasaexport.o picasawindows.o settings.o settingswindow.o timewindow.o trackwidget.o trackwindow.o

colorrenderrer.o: colorrenderrer.cpp
	g++ -O3 -c -ansi -I"/usr/include/ImageMagick" `pkg-config --cflags gtkmm-2.4 libxml++-2.6` colorrenderrer.cpp

connector.o: connector.cpp
	g++ -O3 -c -ansi -I"/usr/include/ImageMagick" `pkg-config --cflags gtkmm-2.4 libxml++-2.6` connector.cpp

datalogdecode.o: datalogdecode.cpp
	g++ -O3 -c -ansi -I"/usr/include/ImageMagick" `pkg-config --cflags gtkmm-2.4 libxml++-2.6` datalogdecode.cpp

datalogger.o: datalogger.cpp
	g++ -O3 -c -ansi -I"/usr/include/ImageMagick" `pkg-config --cflags gtkmm-2.4 libxml++-2.6` datalogger.cpp

dayview.o: dayview.cpp
	g++ -O3 -c -ansi -I"/usr/include/ImageMagick" `pkg-config --cflags gtkmm-2.4 libxml++-2.6` dayview.cpp
	
downloadwindow.o: downloadwindow.cpp
	g++ -O3 -c -ansi -I"/usr/include/ImageMagick" `pkg-config --cflags gtkmm-2.4 libxml++-2.6` downloadwindow.cpp

functions.o: functions.cpp
	g++ -O3 -c -ansi -I"/usr/include/ImageMagick" `pkg-config --cflags gtkmm-2.4 libxml++-2.6` functions.cpp

httpclient.o: httpclient.cpp
	g++ -O3 -c -ansi -I"/usr/include/ImageMagick" `pkg-config --cflags gtkmm-2.4 libxml++-2.6` httpclient.cpp

lowlevel.o: lowlevel.cpp
	g++ -O3 -c -ansi -I"/usr/include/ImageMagick" `pkg-config --cflags gtkmm-2.4 libxml++-2.6` lowlevel.cpp

main.o: main.cpp
	g++ -O3 -c -ansi -I"/usr/include/ImageMagick" `pkg-config --cflags gtkmm-2.4 libxml++-2.6` main.cpp

mainwindow.o: mainwindow.cpp
	g++ -O3 -c -ansi -I"/usr/include/ImageMagick" `pkg-config --cflags gtkmm-2.4 libxml++-2.6` mainwindow.cpp

mapgenerator.o: mapgenerator.cpp
	g++ -O3 -c -ansi -I"/usr/include/ImageMagick" `pkg-config --cflags gtkmm-2.4 libxml++-2.6` mapgenerator.cpp

mapwidget.o: mapwidget.cpp
	g++ -O3 -c -ansi -I"/usr/include/ImageMagick" `pkg-config --cflags gtkmm-2.4 libxml++-2.6` mapwidget.cpp

photowidget.o: photowidget.cpp
	g++ -O3 -c -ansi -I"/usr/include/ImageMagick" `pkg-config --cflags gtkmm-2.4 libxml++-2.6` photowidget.cpp

photowindow.o: photowindow.cpp
	g++ -O3 -c -ansi -I"/usr/include/ImageMagick" `pkg-config --cflags gtkmm-2.4 libxml++-2.6` photowindow.cpp

picasaexport.o: picasaexport.cpp
	g++ -O3 -c -ansi -I"/usr/include/ImageMagick" `pkg-config --cflags gtkmm-2.4 libxml++-2.6` picasaexport.cpp

picasawindows.o: picasawindows.cpp
	g++ -O3 -c -ansi -I"/usr/include/ImageMagick" `pkg-config --cflags gtkmm-2.4 libxml++-2.6` picasawindows.cpp

settings.o: settings.cpp
	g++ -O3 -c -ansi -I"/usr/include/ImageMagick" `pkg-config --cflags gtkmm-2.4 libxml++-2.6` settings.cpp

settingswindow.o: settingswindow.cpp
	g++ -O3 -c -ansi -I"/usr/include/ImageMagick" `pkg-config --cflags gtkmm-2.4 libxml++-2.6` settingswindow.cpp

timewindow.o: timewindow.cpp
	g++ -O3 -c -ansi -I"/usr/include/ImageMagick" `pkg-config --cflags gtkmm-2.4 libxml++-2.6` timewindow.cpp

trackwidget.o: trackwidget.cpp
	g++ -O3 -c -ansi -I"/usr/include/ImageMagick" `pkg-config --cflags gtkmm-2.4 libxml++-2.6` trackwidget.cpp

trackwindow.o: trackwindow.cpp
	g++ -O3 -c -ansi -I"/usr/include/ImageMagick" `pkg-config --cflags gtkmm-2.4 libxml++-2.6` trackwindow.cpp

clean:
	rm colorrenderrer.o connector.o datalogdecode.o datalogger.o dayview.o downloadwindow.o functions.o httpclient.o lowlevel.o main.o mainwindow.o mapgenerator.o mapwidget.o photowidget.o photowindow.o picasaexport.o picasawindows.o settings.o settingswindow.o timewindow.o trackwidget.o trackwindow.o
	rm gpslogger
