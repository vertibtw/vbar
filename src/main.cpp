#include <cstdlib>
#include <format>
#include <fstream>
#include <gtkmm.h>
#include <iostream>
#include <map>
#include <memory>
#include <print>
#include <set>
#include <sstream>

#include "bar/bar.hpp"
#include "ini/ini.hpp"
#include "theme/core.hpp"
#include "util/log.hpp"

int main(int argc, char **argv) {
    std::stringstream ss;
    std::string home_dir;
    char *home_dir_ = std::getenv("HOME");

    if (home_dir_ == nullptr || std::string(home_dir_).empty()) {
        lg::err("HOME environment variable not set.");
        std::exit(EXIT_FAILURE);
    } else {
        home_dir = home_dir_;
    }

    std::string config_path;

    config_path = home_dir + "/.vbar.ini"; // I don't wanna do configs in .config

    std::ifstream f(config_path);

    if (!f.is_open()) {
        lg::err(std::format("couldn't open config file '{}'", config_path));
        std::exit(EXIT_FAILURE);
    }
    ss << f.rdbuf();

    std::string content = ss.str();
    std::shared_ptr<ini> conf = std::make_shared<ini>(ini::parse(content));

    auto css = Gtk::CssProvider::create();
    std::string css_buf;

    if (conf->contains("", "theme")) {
        if ((*conf)[""]["theme"] == "catppuccin mocha")
            css_buf = std::format("{}\n{}", themes::catppuccin::mocha, themes::core);
        else if ((*conf)[""]["theme"] == "catppuccin frappe")
            css_buf = std::format("{}\n{}", themes::catppuccin::frappe, themes::core);
        else if ((*conf)[""]["theme"] == "catppuccin latte")
            css_buf = std::format("{}\n{}", themes::catppuccin::latte, themes::core);
        else if ((*conf)[""]["theme"] == "catppuccin macchiato")
            css_buf = std::format("{}\n{}", themes::catppuccin::macchiato, themes::core);
        else {
            lg::err(std::format("invalid theme name: '{}'; using catppuccin mocha.", (*conf)[""]["theme"]));
            css_buf = std::format("{}\n{}", themes::catppuccin::mocha, themes::core);
        }
    } else {
        lg::warn("no theme provided, using catppuccin mocha.");
        css_buf = std::format("{}\n{}", themes::catppuccin::mocha, themes::core);
    }

    if (conf->contains("bar", "ws-indicator-type")) {
        if ((*conf)["bar"]["ws-indicator-type"] == "id")
            css_buf = std::format("{}\n{}", themes::id_ws, css_buf);
        else if ((*conf)["bar"]["ws-indicator-type"] == "pill")
            css_buf = std::format("{}\n{}", themes::pill_ws, css_buf);
        else {
            lg::err(std::format("invalid workspace indicator type: '{}'", (*conf)["bar"]["ws-indicator-type"]));
            std::exit(EXIT_FAILURE);
        }
    } else {
        lg::warn("no workspace indicator type provided, using default (id)");
        css_buf = std::format("{}\n{}", themes::id_ws, css_buf);
    }

    auto app = Gtk::Application::create("v.bar");

    auto ipc = std::make_shared<hyprland::Ipc>();

    // I don't think there's a way to do this with Gtk::make_managed, sorry not sorry :p
    auto bars = std::make_shared<std::map<GdkMonitor *, std::unique_ptr<bar::Bar>>>();
    auto names = std::make_shared<std::map<GdkMonitor *, std::string>>(); // literally just so I can have the names when removing monitors.

    ipc->on_event = [&](std::string event, std::string arg) -> void {
        for (auto &[_, bar] : *bars) {
            if (bar)
                bar->on_event(event, arg);
        }
    };

    app->signal_startup().connect([&]() -> void {
        css->load_from_data(css_buf);
        Gtk::StyleContext::add_provider_for_display(Gdk::Display::get_default(), css,
                                                    GTK_STYLE_PROVIDER_PRIORITY_USER + 1);

        auto display = Gdk::Display::get_default();
        if (!display) {
            lg::err("no Gdk display available.");
            return;
        }
        auto monitors = display->get_monitors();
        if (!monitors) {
            lg::err("no monitor list available.");
            return;
        }

        auto update_monitors = [app, conf, ipc, bars, names, monitors]() {
            std::map<GdkMonitor *, std::string> current;
            for (int i = 0; i < monitors->get_n_items(); ++i) {
                auto m = monitors->get_typed_object<Gdk::Monitor>(i);
                if (!m)
                    continue;    
                current[m->gobj()] = m->get_connector();
            }

            // remove old monitors
            for (auto it = bars->begin(); it != bars->end();) {
                if (!current.contains(it->first)) {
                    lg::info(std::format("removing bar for monitor '{}'", (*names)[it->first]));
                    app->remove_window(*it->second);
                    names->erase(it->first);
                    it = bars->erase(it);
                } else {
                    it++;
                }
            }

            // add new monitors
            for (auto& [mon, connector] : current) {
                if (bars->contains(mon))
                    continue;
                lg::info (std::format("adding bar for monitor '{}'", connector));
                auto bar = std::make_unique<bar::Bar>(conf, ipc, mon);
                app->add_window(*bar);
                bar->present();
                (*bars)[mon] = std::move(bar);
                (*names)[mon] = connector;
            }
        };

        monitors->signal_items_changed().connect([update_monitors](guint, guint, guint) { update_monitors(); });
        update_monitors();
        app->hold();

        app->signal_activate().connect([bars]() -> void {
            for (auto &[_, bar] : *bars)
                bar->present();
        });
    });

    app->signal_shutdown().connect([app, bars, names]() -> void {
        lg::info("bye bye");
        for (auto &[_, bar] : *bars)
            app->remove_window(*bar);
        bars->clear();
        names->clear();
    });

    return app->run(argc, argv);
}
