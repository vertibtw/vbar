#pragma once
#include <glibmm/main.h>
#include <gtk/gtk.h>
#include <gtk4-layer-shell.h>
#include <gtkmm.h>
#include <gtkmm/box.h>
#include <gtkmm/centerbox.h>
#include <gtkmm/object.h>
#include <memory>
#include <string>

#include "../ini/ini.hpp"
#include "../ipc/hyprland/ipc.hpp"
#include "../services/services.hpp"
#include "../util/is_int.hpp"
#include "../util/log.hpp"
#include "modules/battery.hpp"
#include "modules/clock.hpp"
#include "modules/color_picker.hpp"
#include "modules/layout.hpp"
#include "modules/media.hpp"
#include "modules/screen_recording.hpp"
#include "modules/systray.hpp"
#include "modules/volume/volume_button.hpp"
#include "modules/volume/volume_window.hpp"
#include "modules/window_title.hpp"
#include "modules/workspaces.hpp"

namespace bar {

inline Gtk::Orientation orientation = Gtk::Orientation::HORIZONTAL; // here so that I don't have to deal with pointers

class Bar : public Gtk::Window {
    private:
    std::shared_ptr<hyprland::Ipc> ipc;
    std::shared_ptr<Services> services;

    bar::modules::Workspaces *mod_workspaces = nullptr;
    bar::modules::WindowTitle *mod_window_title = nullptr;
    bar::modules::Clock *mod_clock = nullptr;
    bar::modules::Battery *mod_battery = nullptr;
    bar::modules::VolumeButton *mod_vol_btn = nullptr;
    bar::modules::VolumeWindow *mod_vol_win = nullptr;
    bar::modules::RecordButton *mod_sr_btn = nullptr;
    bar::modules::PickerButton *mod_cp_btn = nullptr;
    bar::modules::LayoutBtn *mod_layout_btn = nullptr;
    bar::modules::Systray *mod_systray = nullptr;
    bar::modules::Media *mod_media = nullptr;

    void apply_modules(std::string &, Gtk::Box *, std::shared_ptr<ini>, std::vector<hyprland::Workspace *>);

    public:
    GdkMonitor *monitor;

    void on_event(std::string event, std::string arg);

    Bar(std::shared_ptr<ini>, std::shared_ptr<hyprland::Ipc>, std::shared_ptr<Services>, GdkMonitor *);
    ~Bar() = default;
};

} // namespace bar
