#pragma once
#include <cstdlib>
#include <gtkmm.h>

namespace bar::modules {
class RecordButton : public Gtk::Button {
    public:
    Gtk::Label *label = Gtk::make_managed<Gtk::Label>();
    RecordButton();
    ~RecordButton() = default;
};
} // namespace bar::modules
