#include "battery.hpp"

namespace bar::modules {
Battery::Battery() {
    this->add_css_class("battery");
    this->st_icon = Gtk::make_managed<Gtk::Label>();
    this->percentage = Gtk::make_managed<Gtk::Label>();

    this->append(*this->st_icon);
    this->append(*this->percentage);

    this->c = Glib::signal_timeout().connect(
        [this]() -> bool {
            int capacity = -1;

            std::ifstream fc("/sys/class/power_supply/BAT0/capacity");
            std::ifstream fs("/sys/class/power_supply/BAT0/status");
            if (!(fc >> capacity)) {
                return false;
            }
            this->percentage->set_text(std::to_string(capacity) + "%");

            if (capacity > 20) {
                this->percentage->remove_css_class("low");
                if (this->state != BatteryState::normal) {
                    this->state = BatteryState::normal;
                }
            } else if (capacity > 5) {
                this->percentage->add_css_class("low");
                if (this->state != BatteryState::low) {
                    Glib::spawn_command_line_async("notify-send --urgency critical battery low");
                    this->state = BatteryState::low;
                }
            } else {
                this->percentage->add_css_class("critical");
                if (this->state != BatteryState::critical) {
                    Glib::spawn_command_line_async("notify-send --urgency critical battery critical");
                    this->state = BatteryState::critical;
                }
            }

            std::string buffer = "";
            if (!std::getline(fs, buffer))
                return false;
            if (buffer == "Full") {
                this->st_icon->set_text("[f]");

                this->st_icon->remove_css_class("discharging");
                this->st_icon->remove_css_class("not_charging");
                this->st_icon->remove_css_class("charging");
                this->st_icon->remove_css_class("unknown");
                this->st_icon->add_css_class("full");
            } else if (buffer == "Discharging") {
                this->st_icon->set_text("[d]");
                this->st_icon->remove_css_class("full");
                this->st_icon->remove_css_class("not_charging");
                this->st_icon->remove_css_class("charging");
                this->st_icon->remove_css_class("unknown");
                this->st_icon->add_css_class("discharging");
            } else if (buffer == "Not charging") {
                this->st_icon->set_text("[n]");
                this->st_icon->remove_css_class("discharging");
                this->st_icon->remove_css_class("full");
                this->st_icon->remove_css_class("charging");
                this->st_icon->remove_css_class("unknown");
                this->st_icon->add_css_class("not_charging");
            } else if (buffer == "Charging") {
                this->st_icon->set_text("[c]");
                this->st_icon->remove_css_class("discharging");
                this->st_icon->remove_css_class("not_charging");
                this->st_icon->remove_css_class("full");
                this->st_icon->remove_css_class("unknown");
                this->st_icon->add_css_class("charging");
            } else {
                this->st_icon->set_text("[u]");
                this->st_icon->remove_css_class("discharging");
                this->st_icon->remove_css_class("not_charging");
                this->st_icon->remove_css_class("charging");
                this->st_icon->remove_css_class("full");
                this->st_icon->add_css_class("unknown");
            }

            return true;
        },
        200);
}
} // namespace bar::modules
