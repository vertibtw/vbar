#include "color_picker.hpp"
#include <format>
#include <iostream>

namespace bar::modules {
PickerButton::PickerButton() {
    this->label->set_text("");
    this->add_css_class(
        "cp_button"); // yikes (also I should probably have a generic button class but that's a problem for future me)
    this->set_child(*this->label);
    this->signal_clicked().connect([]() {
        std::string cmd = "hyprpicker", out, err;
        Glib::spawn_command_line_sync(cmd, &out, &err, nullptr);

        while (!out.empty() && (out.back() == '\n' || out.back() == '\r' || out.back() == ' '))
            out.pop_back();

        if (!out.empty()) {
            Glib::spawn_command_line_async(std::format("wl-copy --trim-newline '{}'", out));
            Glib::spawn_command_line_async(std::format("notify-send 'hyprpicker: {}'", out));
        }
        if (!err.empty())
            Glib::spawn_command_line_async(std::format("notify-send hyprpicker error(s): {}", err));
    });
}
} // namespace bar::modules
