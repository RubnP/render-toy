/**
 * @file
 * @author Ruben Pena <rubn.pena@gmail.com>
 * @brief This file contains an imgui handling wrapper
 *
 */

#pragma once

#include <imgui.h>
#include <vk_loader.hh>
#include <window.hh>

namespace imgui_hnd {

void imgui_init(vk_loader *vk_ld_ptr, platform::window *wnd);
void imgui_main_loop_start();
void imgui_main_loop_end();
void imgui_shutdown(vk_loader *vk_ld_ptr);

} // namespace imgui_hnd
