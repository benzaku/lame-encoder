#pragma once

#include <functional>

namespace converter {

class Task {
public:
  Task(const std::function<void()> &);
  ~Task() = default;
  void execute();

private:
  const std::function<void()> _closure;
};

} // namespace converter
