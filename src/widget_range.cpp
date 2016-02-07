/*
 * libgphoto++ - modern c++ wrapper library for gphoto2
 * Copyright (C) 2016 Marco Gulino <marco AT gulinux.net>
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "widgets.h"
#include "widget_p.h"
using namespace GPhoto;
using namespace std;

Widget::RangeValue::RangeValue(Widget* widget): Value< float, float, float* >(widget, [](float &f) {return &f; })
{
  widget->d->gphoto << GP2_RUN(this, widget) { return gp_widget_get_range(widget->d->widget, &_range.min, &_range.max, &_range.increment); };
}


Widget::RangeValue::Range Widget::RangeValue::range() const
{
  return *this;
}


bool Widget::RangeValue::Range::operator==(const Widget::RangeValue::Range& o) const
{
  return o.min == min && o.max == max && o.increment == increment;
}

Widget::RangeValue::operator Range() const
{
  return _range;
}