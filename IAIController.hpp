#pragma once

class IAIController {
public:
  virtual ~IAIController() {};
  virtual void update() = 0;
};