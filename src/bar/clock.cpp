#include "clock.hpp"
#include "bar.hpp"

namespace bar::modules {
bool Clock::update() {
    auto now = std::time(nullptr);
    auto *lt = std::localtime(&now);
    std::string s;
    if (bar::orientation == Gtk::Orientation::HORIZONTAL)
        s = std::format("{:02d}:{:02d}", lt->tm_hour, lt->tm_min);
    else
        s = std::format("{:02d}\n{:02d}", lt->tm_hour, lt->tm_min);

    this->l->set_text(s);
    return true;
}
Clock::Clock() {
    this->append(*this->l);
    this->update(); // so that it's not blank on startup
    this->add_css_class("clock");

    Glib::signal_timeout().connect(sigc::mem_fun(*this, &Clock::update), 1000);
}
} // namespace bar::modules
