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
 * 
 */

#include "camera_filesystem.h"
#include "backend/list.h"
#include <algorithm>
#include "camerafile.h"
#include "backend/gphoto_wrapper.h"

using namespace GPhotoCPP;
using namespace std;

DPTR_CLASS(CameraFolder) {
public:
  Private(const string& path, const GPhotoCameraPtr& gphoto_camera, const CameraFolderPtr& parent, const LoggerPtr &logger, GPhotoCPP::CameraFolder *q);
  const string path;
  const GPhotoCameraPtr camera;
  const CameraFolderPtr parent;
  CameraFolderPtr shared_from_this() const;
  const LoggerPtr logger;
private:
  GPhotoCPP::CameraFolder* q;
};

CameraFolderPtr CameraFolder::Private::shared_from_this() const
{
  return q->shared_from_this();
}


CameraFolder::Private::Private(const string& path, const GPhotoCameraPtr& gphoto_camera, const CameraFolderPtr& parent, const LoggerPtr& logger, CameraFolder* q) 
  : path{path}, camera{gphoto_camera}, parent{parent}, logger{logger}, q{q}
{
}

CameraFolder::CameraFolder(const string& path, const GPhotoCameraPtr& gphoto_camera, const LoggerPtr& logger, const CameraFolderPtr& parent)
 : dptr(path, gphoto_camera, parent, logger, this)
{
}

CameraFolder::~CameraFolder()
{
}

list< CameraFolderPtr > CameraFolder::folders() const {
  List files_list{*d->camera};
  d->camera << CAM_RUN(this, &files_list) { GPRET(gp_camera_folder_list_folders(gp_cam, d->path.c_str(), files_list, gp_ctx)) };
  multimap<string,string> files_map = files_list;
  list<CameraFolderPtr> folders(files_map.size());;
  transform(begin(files_map), end(files_map), begin(folders), [this](const pair<string,string> &p){
    return make_shared<CameraFolder>( path() + p.first, d->camera, d->logger, d->shared_from_this());
  });
  return folders;
}

list< CameraFileInfoPtr > CameraFolder::files() const
{
  List files_list{*d->camera};
  d->camera << CAM_RUN(this, &files_list) { GPRET(gp_camera_folder_list_files(gp_cam, d->path.c_str(), files_list, gp_ctx)) };
  multimap<string,string> files_map = files_list;
  list<CameraFileInfoPtr> files(files_map.size());;
  transform(begin(files_map), end(files_map), begin(files), [this](const pair<string,string> &p){
    return make_shared<GPhotoCPP::CameraFileInfo>(p.first, d->shared_from_this(), d->camera, d->logger);
  });
  return files;
}

CameraFolderPtr CameraFolder::parent() const
{
  return d->parent;
}


DPTR_CLASS(GPhotoCPP::CameraFileInfo) {
public:
  Private(const string& name, const CameraFolderPtr& folder, const GPhotoCameraPtr& camera, const LoggerPtr& logger);
  const string name;
  const CameraFolderPtr folder;
  const GPhotoCameraPtr camera;
  const LoggerPtr logger;
private:
};

GPhotoCPP::CameraFileInfo::Private::Private(const string& name, const CameraFolderPtr& folder, const GPhotoCameraPtr& camera, const LoggerPtr &logger)
  : name{name}, folder{folder}, camera{camera}, logger{logger}
{
}


GPhotoCPP::CameraFileInfo::CameraFileInfo(const string& name, const CameraFolderPtr& folder, const GPhotoCameraPtr& gphoto_camera, const LoggerPtr& logger)
  : dptr(name, folder, gphoto_camera, logger)
{
}

CameraFolderPtr GPhotoCPP::CameraFileInfo::parent() const
{
  return d->folder;
}

string CameraFolder::path() const
{
  return d->path[d->path.size()-1] == '/' ? d->path : d->path + "/";
}

CameraFilePtr GPhotoCPP::CameraFileInfo::camera_file() const
{
  return make_shared<GPhotoCPP::CameraFile>(d->folder->path(), name(), d->camera, d->logger);
}

string GPhotoCPP::CameraFileInfo::name() const
{
  return d->name;
}

string GPhotoCPP::CameraFileInfo::path() const
{
  return d->folder->path() + name();
}

void GPhotoCPP::CameraFileInfo::remove()
{
  d->camera << CAM_RUN(this) { GPRET(gp_camera_file_delete(gp_cam, d->folder->path().c_str(), d->name.c_str(), gp_ctx)) };
}


