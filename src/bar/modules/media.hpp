#pragma once
#include <gtkmm.h>
#include <memory>

namespace bar::services {
class MprisService;
}

namespace bar::modules {
class Media : public Gtk::Box {
    public:
    explicit Media(std::shared_ptr<bar::services::MprisService>);
    ~Media() = default;

    private:
    std::shared_ptr<bar::services::MprisService> service;
    Gtk::Label *label = Gtk::make_managed<Gtk::Label>();
    Gtk::Button *previous_btn = nullptr;
    Gtk::Button *play_btn = nullptr;
    Gtk::Button *next_btn = nullptr;
    Gtk::Image *play_image = nullptr;

    void update();
};
} // namespace bar::modules
