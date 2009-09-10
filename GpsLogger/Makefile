makefile: gpslogger

format:
	astyle --style=kr *.h -p -P -n
	astyle --style=kr *.cpp -p -P -n

all: gpslogger

gpslogger: connector.o datalogdecode.o datalogger.o dayview.o downloadwindow.o functions.o httpclient.o lowlevel.o main.o mainwindow.o mapwidget.o photowidget.o photowindow.o picasaexport.o picasawindows.o settingswindow.o timewindow.o
	g++ -o"gpslogger" `pkg-config --libs gtkmm-2.4 exiv2 libxml++-2.6` `curl-config --libs` `Magick++-config --libs` connector.o datalogdecode.o datalogger.o dayview.o downloadwindow.o functions.o httpclient.o lowlevel.o main.o mainwindow.o mapwidget.o photowidget.o photowindow.o picasaexport.o picasawindows.o settingswindow.o timewindow.o

connector.o: connector.cpp
	g++ -I"/usr/include/exiv2" -I"/usr/include/curl" -O3 -c `pkg-config --cflags gtkmm-2.4 libxml++-2.6` connector.cpp

datalogdecode.o: datalogdecode.cpp
	g++ -I"/usr/include/exiv2" -I"/usr/include/curl" -O3 -c `pkg-config --cflags gtkmm-2.4 libxml++-2.6` datalogdecode.cpp

datalogger.o: datalogger.cpp
	g++ -I"/usr/include/exiv2" -I"/usr/include/curl" -O3 -c `pkg-config --cflags gtkmm-2.4 libxml++-2.6` datalogger.cpp

dayview.o: dayview.cpp
	g++ -I"/usr/include/exiv2" -I"/usr/include/curl" -O3 -c `pkg-config --cflags gtkmm-2.4 libxml++-2.6` dayview.cpp
	
downloadwindow.o: downloadwindow.cpp
	g++ -I"/usr/include/exiv2" -I"/usr/include/curl" -O3 -c `pkg-config --cflags gtkmm-2.4 libxml++-2.6` downloadwindow.cpp

functions.o: functions.cpp
	g++ -I"/usr/include/exiv2" -I"/usr/include/curl" -O3 -c `pkg-config --cflags gtkmm-2.4 libxml++-2.6` functions.cpp

httpclient.o: httpclient.cpp
	g++ -I"/usr/include/exiv2" -I"/usr/include/curl" -O3 -c `pkg-config --cflags gtkmm-2.4 libxml++-2.6` httpclient.cpp

lowlevel.o: lowlevel.cpp
	g++ -I"/usr/include/exiv2" -I"/usr/include/curl" -O3 -c `pkg-config --cflags gtkmm-2.4 libxml++-2.6` lowlevel.cpp

main.o: main.cpp
	g++ -I"/usr/include/exiv2" -I"/usr/include/curl" -O3 -c `pkg-config --cflags gtkmm-2.4 libxml++-2.6` main.cpp

mainwindow.o: mainwindow.cpp
	g++ -I"/usr/include/exiv2" -I"/usr/include/curl" -O3 -c `pkg-config --cflags gtkmm-2.4 libxml++-2.6` mainwindow.cpp

mapwidget.o: mapwidget.cpp
	g++ -I"/usr/include/exiv2" -I"/usr/include/curl" -O3 -c `pkg-config --cflags gtkmm-2.4 libxml++-2.6` mapwidget.cpp

photowidget.o: photowidget.cpp
	g++ -I"/usr/include/exiv2" -I"/usr/include/curl" -O3 -c `pkg-config --cflags gtkmm-2.4 libxml++-2.6` photowidget.cpp

photowindow.o: photowindow.cpp
	g++ -I"/usr/include/exiv2" -I"/usr/include/curl" -O3 -c `pkg-config --cflags gtkmm-2.4 libxml++-2.6` photowindow.cpp

picasaexport.o: picasaexport.cpp
	g++ -I"/usr/include/exiv2" -I"/usr/include/curl" -O3 -c `pkg-config --cflags gtkmm-2.4 libxml++-2.6` picasaexport.cpp

picasawindows.o: picasawindows.cpp
	g++ -I"/usr/include/exiv2" -I"/usr/include/curl" -O3 -c `pkg-config --cflags gtkmm-2.4 libxml++-2.6` picasawindows.cpp

settingswindow.o: settingswindow.cpp
	g++ -I"/usr/include/exiv2" -I"/usr/include/curl" -O3 -c `pkg-config --cflags gtkmm-2.4 libxml++-2.6` settingswindow.cpp

timewindow.o: timewindow.cpp
	g++ -I"/usr/include/exiv2" -I"/usr/include/curl" -O3 -c `pkg-config --cflags gtkmm-2.4 libxml++-2.6` timewindow.cpp

clean:
	rm *.o 
	rm gpslogger
