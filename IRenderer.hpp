#pragma once
class IRenderer {
public:
  virtual ~IRenderer() {};
  virtual void render(const float& delta) = 0;
  virtual void handleWindowResize() = 0;
};

