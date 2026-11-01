#pragma once
#include <ctime>
#include <format>
#include <glibmm.h>
#include <gtkmm.h>
#include <sstream>

namespace bar {
namespace modules {
class Clock : public Gtk::Box {
    private:
    Gtk::Label *l = Gtk::make_managed<Gtk::Label>();
    bool update();

    public:
    Clock();
    ~Clock() = default;
};
} // namespace modules
} // namespace bar