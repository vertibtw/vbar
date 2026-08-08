#include <cstdlib>
#include <format>
#include <fstream>
#include <gtkmm.h>
#include <iostream>
#include <memory>
#include <sstream>

#include "bar/bar.hpp"
#include "ini/ini.hpp"
#include "theme/core.hpp"

int main(int argc, char **argv) {
    std::stringstream ss;

    std::string home_dir;
    char *home_dir_ = std::getenv("HOME");

    if (home_dir_ == nullptr || std::string(home_dir_).empty()) {
        std::cerr << "ERROR: $HOME environment variable not set.\n";
        std::exit(1);
    } else {
        home_dir = home_dir_;
    }

    std::string config_path;

    config_path = home_dir + "/.vbar.ini"; // I don't wanna do configs in .config

    std::ifstream f(config_path);

    if (!f.is_open()) {
        std::cout << "ERROR: couldn't open config file '" << config_path << "'\n";
        return EXIT_FAILURE;
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
            std::cerr << "ERROR: invalid theme name: '" << (*conf)[""]["theme"] << "'; using catppuccin mocha.\n";
            css_buf = std::format("{}\n{}", themes::catppuccin::mocha, themes::core);
        }
    } else {
        std::cerr << "WARN: no theme provided, using catppuccin mocha.\n";
        css_buf = std::format("{}\n{}", themes::catppuccin::mocha, themes::core);
    }

    auto app = Gtk::Application::create("v.bar");

    auto bar_widget = Gtk::make_managed<bar::Bar>(conf);

    app->signal_startup().connect([&]() -> void {
        css->load_from_data(css_buf);
        Gtk::StyleContext::add_provider_for_display(Gdk::Display::get_default(), css,
                                                    GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
        app->add_window(*bar_widget);
        app->signal_activate().connect([&]() -> void { bar_widget->present(); });
    });

    return app->run(argc, argv);
}
