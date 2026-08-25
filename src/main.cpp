#include <cstdlib>
#include <format>
#include <fstream>
#include <gtkmm.h>
#include <iostream>
#include <memory>
#include <print>
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

    // this is a terrible excuse to use std::format lol
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
        else
            css_buf = std::format("{}\n{}", themes::id_ws, css_buf);
    } else {
        lg::warn("no workspace indicator type provided, using default (id)");
        css_buf = std::format("{}\n{}", themes::id_ws, css_buf);
    }

    lg::info(std::format("css_buf = \n {}", css_buf));

    auto app = Gtk::Application::create("v.bar");

    auto bar_widget = Gtk::make_managed<bar::Bar>(conf);

    app->signal_startup().connect([&]() -> void {
        css->load_from_data(css_buf);
        Gtk::StyleContext::add_provider_for_display(Gdk::Display::get_default(), css,
                                                    GTK_STYLE_PROVIDER_PRIORITY_USER + 1);
        app->add_window(*bar_widget);
        app->signal_activate().connect([&]() -> void { bar_widget->present(); });
    });

    app->signal_shutdown().connect([]() -> void {});

    return app->run(argc, argv);
}
