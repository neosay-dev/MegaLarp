#include "config/Config.hpp"
#include "ui/Menu.hpp"

#include <Geode/Geode.hpp>
#include <imgui-cocos.hpp>

using namespace geode::prelude;

$on_mod(Loaded) {
    log::info("MegaLarp v1.0.0 initializing...");

    megalarp::Config::get().load();

    ImGuiCocos::get()
        .setup([] {
            megalarp::Menu::get().setup();
        })
        .draw([] {
            megalarp::Menu::get().render();
        });

    log::info("MegaLarp loaded successfully! Press Tab (or your bound key) to open the menu.");
}
