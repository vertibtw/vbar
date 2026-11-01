#include "screen_recording.hpp"

namespace bar::modules {
RecordButton::RecordButton() {
    this->label->set_text("");
    this->add_css_class("sr_button");
    this->set_child(*this->label);
    this->signal_clicked().connect([]() {
        std::string home = std::getenv("HOME");
        std::string cmd = "kitty wf-recorder --audio-backend=pipewire --audio -f" + home + "/r.mp4";
        Glib::spawn_command_line_async(cmd);
    });
}
} // namespace bar::modules
