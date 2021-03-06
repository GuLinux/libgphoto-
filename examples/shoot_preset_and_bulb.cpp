/*
 * Copyright (C) 2016  Marco Gulino <marco@gulinux.net>
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
 *
 */
#include "commons.h"
#include <exposure.h>

using namespace std;
using namespace GPhotoCPP;


int main(int argc, char **argv) {
  init_options(argc, argv);
  if(has_option("-h") || has_option("--help")) {
    cout << args[0] << " usage: " << endl
	 << "\t-m: enable mirror lock (default: disabled)" << endl
	 << "\t-s: download and save files to current directory (default: disabled)" << endl
	 << "\t-r: set raw format before shooting" << endl
	 << "\t-j: set jpeg format before shooting" << endl
	 << endl;
    return 0;
  }

  auto logger = make_logger();
  
  
  auto camera = init_camera(logger);
  if(!camera)
    return 1;
  auto settings = camera->widgets_settings();
  
  static multimap<string, string> widget_names{
    {"shutter", "eosremoterelease"}, 
    {"shutter", "bulb"}, {"customfunc", "customfuncex"}, 
    {"exposure", "shutterspeed"}, 
    {"exposure", "shutterspeed2"}, 
    {"exposure", "eos-shutterspeed"},
    {"format", "imageformat"}
  };
  multimap<string, WidgetPtr> widgets_init;
  transform(begin(widget_names), end(widget_names), inserter(widgets_init, end(widgets_init)), [&](const pair<string,string> &p){ return make_pair(p.first,  settings->child_by_name(p.second));});
  map<string, WidgetPtr> widgets;
  copy_if(begin(widgets_init), end(widgets_init), inserter(widgets, end(widgets)), [](const pair<string,WidgetPtr> &p){ return static_cast<bool>(p.second);});
  cout << "Widgets found:\n";
  for(auto w: widgets_init) { if(w.second) cout << w.first << ": " << w.second->name() << endl; }
  cout << "Selected widgets:\n";
  for(auto w: widgets) { cout << w.first << ": " << w.second->name() << endl; }

  if(!widgets.count("format"))
    cerr << "WARNING: Unable to find image format config widget\n";
  if(has_option("-j") && widgets.count("format"))
    widgets["format"]->get<Widget::MenuValue>()->set("Large Fine JPEG"); // TODO: dynamic from menu entries
  if(has_option("-r") && widgets.count("format"))
    widgets["format"]->get<Widget::MenuValue>()->set("RAW");
    

  ShooterPtr shooter = make_shared<SerialShooter>("/dev/ttyUSB0", logger);
  if(widgets.count("shutter")) {
    shooter = widgets["shutter"]->name() == "eosremoterelease" ? ShooterPtr{new EOSRemoteReleaseShutter{camera, logger}} : ShooterPtr{new BulbSettingShutter{camera, logger}};
  }
  
  auto mirror_lock = has_option("-m") ? GPhotoCPP::Camera::MirrorLock{seconds(2), shooter} : GPhotoCPP::Camera::MirrorLock{};
  
  cout << "Using mirror lock (option -m): " << boolalpha << static_cast<bool>(mirror_lock) << endl;
  cout << "Widgets: " << endl;
  for(auto setting: camera->widgets_settings()->all_children()) {
    cout << "** " << setting << endl;
  }
  
  
  if(!widgets.count("exposure")) {
    cout << "Error! unable to find <shutterspeed> settings widget\n";
    return 1;
  }
  Exposure exposure{widgets["exposure"]};
  
  auto shoot = [&](function<GPhotoCPP::Camera::ShotPtr()> shoot_f){
    camera->save_settings();
    GPhotoCPP::Camera::ShotPtr result = shoot_f();
    (**result).wait();
    cerr << "Shot finished: duration=" << result->duration().count() << ", elapsed: " << result->elapsed().count() << endl;
    CameraFilePtr cf = (**result).get();
    cerr << *cf << endl;
    if(has_option("-s"))
      cf->save(cf->file());
  };
  
  exposure.set(seconds{0.1});
  shoot([&]{ return camera->shoot_preset(mirror_lock); });
  this_thread::sleep_for(seconds(2));
  exposure.set_bulb();
  shoot([&]{ return camera->shoot_bulb(seconds(2), shooter, mirror_lock); });
  this_thread::sleep_for(seconds(2));
  shoot([&]{ return camera->shoot_bulb(seconds(5), shooter, mirror_lock); });
  

  return 0;
};