
#pragma once
#include <glm/glm.hpp>

class ubo {};

class trans_mat : ubo {
public:
  glm::mat4 mod_world;
  glm::mat4 world_cam;
  glm::mat4 proj;
};
