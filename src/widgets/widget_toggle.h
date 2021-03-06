#ifndef LIBGPHOTO_CPP_WIDGETS_WIDGET_TOGGLE_H
#define LIBGPHOTO_CPP_WIDGETS_WIDGET_TOGGLE_H
/*
 * libgphoto++ - modern c++ wrapper library for gphoto2
 * Copyright (C) 2016 Marco Gulino <marco AT gulinux.net>
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "widget.h"
namespace GPhotoCPP {
class Widget::ToggleValue : public Widget::Value<bool, int, std::shared_ptr<int>>{
private:
  friend class Widget;
  ToggleValue(const WidgetPtr &widget);
};
}

std::ostream &operator<<(std::ostream &o, const GPhotoCPP::Widget::ToggleValue &w);
inline std::ostream &operator<<(std::ostream &o, const std::shared_ptr<GPhotoCPP::Widget::ToggleValue> &w) { return o << *w; }
#endif // LIBGPHOTO_CPP_WIDGETS_WIDGET_TOGGLE_H
