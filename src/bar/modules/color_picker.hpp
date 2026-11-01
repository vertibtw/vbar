#pragma once
#include <cstdlib>
#include <gtkmm.h>

namespace bar::modules {
class PickerButton : public Gtk::Button {
    public:
    Gtk::Label *label = Gtk::make_managed<Gtk::Label>();
    PickerButton();
    ~PickerButton() = default;
};
} // namespace bar::modules
