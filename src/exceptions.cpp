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


#include "exceptions.h"
#include <gphoto2/gphoto2.h>

using namespace std;
using namespace GPhoto;
Exception::Exception(int error_code): runtime_error(gp_result_as_string(error_code))
{
}

Exception::Exception(const string& __arg): runtime_error(__arg)
{
}

TimeoutError::TimeoutError(const string& __arg): Exception(__arg)
{

}