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

#include "colorrenderrer.h"

ColorCellRenderer::ColorCellRenderer() :
        Glib::ObjectBase ( typeid ( ColorCellRenderer ) ),
        Gtk::CellRenderer(),
        color ( *this, "color" ) {
    property_mode() = Gtk::CELL_RENDERER_MODE_INERT;
    property_xpad() = 0;
    property_ypad() = 0;
}

Glib::PropertyProxy<int> ColorCellRenderer::property_color()  {
    return color.get_proxy();
}

void ColorCellRenderer::get_size_vfunc ( Gtk::Widget& widget, Gdk::Rectangle const * cell_area, int* x_offset, int* y_offset, int* width, int* height ) const {
    *width = 32;
    *height = 16;
}

void ColorCellRenderer::render_vfunc ( Glib::RefPtr<Gdk::Drawable> const & window, Gtk::Widget& widget, Gdk::Rectangle const & background_area, Gdk::Rectangle const & cell_area, Gdk::Rectangle const & expose_area, Gtk::CellRendererState flags ) {
    Cairo::RefPtr<Cairo::Context> context = window->create_cairo_context();
    context->rectangle ( cell_area.get_x(), cell_area.get_y(), cell_area.get_width(), cell_area.get_height() );
    context->clip();
    context->save();
    int centerX = cell_area.get_x() + cell_area.get_width() / 2;
    int centerY = cell_area.get_y() + cell_area.get_height() / 2;
    context->translate ( centerX + 0.5, centerY + 0.5 );
    int palete[] = {0xED, 0xD4, 0x00, 0xF5, 0x79, 0x00, 0xC1, 0x7D, 0x11, 0x73, 0xd2, 0x16, 0x34, 0x65, 0xA4, 0x75, 0x50, 0x7B, 0xCC, 0x00, 0x00};
    int index = ( color % 7 ) * 3;
    double rChannel = ( double ) palete[index + 0] / 255;
    double gChannel = ( double ) palete[index + 1] / 255;
    double bChannel = ( double ) palete[index + 2] / 255;
    context->rectangle ( -12, -8, 23, 15 );
    context->set_source_rgb ( rChannel, gChannel, bChannel );
    context->fill_preserve();
    context->set_source_rgb ( 0.0, 0.0, 0.0 );
    context->set_line_width ( 1.0 );
    context->stroke();
    context->restore();
}
