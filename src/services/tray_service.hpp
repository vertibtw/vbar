#pragma once
#include <gdkmm.h>
#include <giomm.h>
#include <memory>
#include <sigc++/sigc++.h>
#include <vector>

namespace bar::services {

class TrayItem {
    public:
    explicit TrayItem(const Glib::ustring &identifier);

    const Glib::ustring &id() const {
        return this->identifier;
    }

    bool valid() const {
        return static_cast<bool>(this->proxy);
    }

    Glib::ustring title() const;
    Glib::ustring icon_name() const;
    Glib::RefPtr<Gdk::Texture> icon() const;

    void activate();
    void context_menu();

    sigc::signal<void()> changed;

    private:
    Glib::ustring identifier;
    Glib::RefPtr<Gio::DBus::Proxy> proxy;

    Glib::ustring get_string(const char *property) const;
    bool item_is_menu() const;
    void call(const char *method);
};

class TrayService {
    public:
    TrayService();

    const std::vector<std::unique_ptr<TrayItem>> &items() const {
        return this->tray_items;
    }

    sigc::signal<void()> changed;

    private:
    Glib::RefPtr<Gio::DBus::Proxy> watcher;
    std::vector<std::unique_ptr<TrayItem>> tray_items;

    void add(const Glib::ustring &identifier);
    void remove(const Glib::ustring &identifier);
};

} // namespace bar::services
