#pragma once
class IRenderer {
public:
  virtual ~IRenderer() {};
  virtual void render(float delta) = 0;
  virtual void handleWindowResize() = 0;
  virtual void updateState() = 0;
};

