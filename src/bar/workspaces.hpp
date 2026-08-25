#pragma once

#include "../ipc/hyprland/ipc.hpp"
#include "workspace.hpp"
#include <gtkmm/box.h>
#include <gtkmm/enums.h>
#include <gtkmm/label.h>
#include <gtkmm/object.h>
#include <memory>
#include <string>
#include <vector>

namespace bar::modules {
class Workspaces : public Gtk::Box {
    private:
    std::vector<hyprland::Workspace *> workspaces;
    std::shared_ptr<hyprland::Ipc> ipc;

    public:
    void change_active_ws(int);
    void destroy_ws(int);
    void create_ws(int);

    std::string indicator_type; // this could probably be an enum but I don't wanna do that for now.

    Workspaces(std::shared_ptr<hyprland::Ipc>, std::string);
    ~Workspaces() = default;
};
} // namespace bar::modules
