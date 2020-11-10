#include "Task.h"
namespace converter {

Task::Task(const std::function<void()> &closure)
    : _closure(std::move(closure)) {}

void Task::execute() {
  if (_closure) {
    _closure();
  }
}

} // namespace converter
