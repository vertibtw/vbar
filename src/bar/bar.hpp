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
#include "../util/is_int.hpp"
#include "battery.hpp"
#include "clock.hpp"
#include "color_picker.hpp"
#include "screen_recording.hpp"
#include "volume/volume_button.hpp"
#include "volume/volume_window.hpp"
#include "window_title.hpp"
#include "workspaces.hpp"

namespace bar {

inline Gtk::Orientation orientation = Gtk::Orientation::HORIZONTAL; // here so that I don't have to deal with pointers

class Bar : public Gtk::Window {
    private:
    std::shared_ptr<hyprland::Ipc> ipc;

    bar::modules::Workspaces *mod_workspaces = nullptr;
    bar::modules::WindowTitle *mod_window_title = nullptr;
    bar::modules::Clock *mod_clock = nullptr;
    bar::modules::Battery *mod_battery = nullptr;
    bar::modules::VolumeButton *mod_vol_btn = nullptr;
    bar::modules::RecordButton *mod_sr_btn = nullptr;
    bar::modules::PickerButton *mod_cp_btn = nullptr;

    public:
    Bar(std::shared_ptr<ini>);
    ~Bar() = default;
};

} // namespace bar
