#pragma once
#include <gtkmm/box.h>
#include <gtkmm/label.h>
#include <map>
#include <memory>
#include <vector>

#include "../ipc/hyprland/ipc.hpp"
#include "workspace.hpp"

namespace bar::modules {
class LayoutBtn : public Gtk::Box {
    private:
    std::map<std::string, std::string> icons = {{"dwindle", "[@]"}, {"scrolling", "=[]="}, {"master", "[]="}};
    int active_ws;
    std::map<int, std::string> workspaces; // layouts by workspace id
    Gtk::Label *l;

    public:
    void on_workspace_change(hyprland::Workspace *);
    LayoutBtn(std::vector<hyprland::Workspace *>, std::shared_ptr<hyprland::Ipc>);
    ~LayoutBtn() = default;
};
} // namespace bar::modules
