#pragma once

struct LinearVelocity {
  vec2 velocity = { 0.0f, 0.0f };
  float friction = 0.1f;
  vec2 acceleration = { 0.0f, 0.0f };
};

struct AngularVelocity {
  float velocity = 0.0f;
  float acceleration = 0.0f;
  float friction = 0.1f;
};

struct Velocity {
  LinearVelocity linear_velocity;
  AngularVelocity angular_velocity;
};

