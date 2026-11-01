#pragma once
#include <sigc++/sigc++.h>
#include <wp/wp.h>

namespace bar::services {

struct VolumeState {
    double volume = 0.0;
    bool muted = false;
};

class VolumeService {
    public:
    VolumeService();
    ~VolumeService();

    VolumeService(const VolumeService &) = delete;
    VolumeService &operator=(const VolumeService &) = delete;

    void set_volume(double pct);
    void set_muted(bool muted);

    const VolumeState &state() const {
        return this->current;
    }

    sigc::signal<void(VolumeState)> changed;

    private:
    WpCore *core = nullptr;
    WpPlugin *mixer = nullptr;
    WpPlugin *default_nodes = nullptr;
    VolumeState current;
    int pending_plugins = 2;
    guint mixer_handler = 0;
    guint nodes_handler = 0;
    bool ready = false;

    static void on_component_loaded(GObject *, GAsyncResult *, gpointer);
    static void on_mixer_changed(WpPlugin *, guint, gpointer);
    static void on_nodes_changed(WpPlugin *, gpointer);

    guint default_sink_id();
    void refresh();
};

} // namespace bar::services
