#pragma once
#include <glibmm.h>
#include <gtkmm.h>

namespace hyprland {
class Workspace : public Gtk::Box {
    public:
    int id = -1;
    // TODO: make this an enum
    std::string layout = "dwindle"; // assuming dwindle is default
    bool active = false;
    bool exists = false;

    Workspace() {
        auto click = Gtk::GestureClick::create();
        auto motion = Gtk::EventControllerMotion::create();
        click->signal_pressed().connect([&](int, double, double) {
            Glib::spawn_command_line_sync(("hyprctl dispatch 'hl.dsp.focus({ workspace = " + std::to_string(this->id) +
                                           " })'")); // TODO: use the socket1 function thingy
        });

        motion->signal_enter().connect([this](double x, double y) {
            if (!this->active)
                this->add_css_class("active");
        });
        motion->signal_leave().connect([this]() {
            if (!this->active)
                this->remove_css_class("active");
        });

        this->add_controller(motion);
        this->add_controller(click);
    }
};
} // namespace hyprland
