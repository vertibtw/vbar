#include "layout.hpp"
#include "../util/log.hpp"
#include <gtkmm/object.h>

namespace bar::modules {

  void LayoutBtn::on_workspace_change(hyprland::Workspace* ws) {
    for (auto [layout, icon] : this->icons) {
      if (ws->layout == layout) {
          this->l->set_text(icon);
          return;
      }
    }
    this->l->set_text("unknown");
  }

  LayoutBtn::LayoutBtn(std::vector<hyprland::Workspace*> ws, std::shared_ptr<hyprland::Ipc> ipc) {
    int active_ws_id = -1;
    for (auto [id, layout] : this->workspaces) {
      layout = "dwindle"; // dwindle by default
    }

    for (auto w : ws) {
      this->workspaces[w->id] = w->layout;
      if (w->active) active_ws_id = w->id;
    }
    
    if (active_ws_id == -1) {
      lg::err("no active workspace id provided, layout button will not be created.");
      return;        
    }

    Gtk::make_managed<Gtk::Label>("unknown");
    for (auto [layout, icon] : this->icons) {
        if (ws[active_ws_id]->layout == layout)
          this->l->set_text(icon);
    }
    this->active_ws = active_ws_id;
    this->append(*this->l);


    auto click = Gtk::GestureClick::create();

    click->signal_pressed().connect([this, ipc](int, double, double){
        auto it = icons.find(workspaces[this->active_ws]);
        if (it != icons.end()) {
          it++;
          if (it == icons.end()) it = icons.begin();
          workspaces[this->active_ws] = it->first;
          std::ignore = ipc->socket1(std::format("/dispatch workspacerule {}, layout:{}", this->active_ws, icons[workspaces[this->active_ws]]));
          this->l->set_text(it->second);
        }
    });
  }
  
}
