#pragma once
#include <cstdint>
#include <fstream>
#include <gtkmm.h>
#include <iostream>
#include <sys/types.h>

namespace bar::modules {
class Battery : public Gtk::Box {
    private:
    enum BatteryState : u_int8_t { normal, low, critical };
    BatteryState state = BatteryState::normal;
    Gtk::Label *st_icon;
    Gtk::Label *percentage;

    sigc::connection c;

    public:
    Battery();
    ~Battery() {
        this->c.disconnect();
    };
};
} // namespace bar::modules
