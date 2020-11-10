#include "Executor.h"

#include <iostream>

namespace converter {

Executor::Executor(const size_t concurrency)
    : _concurrency(concurrency), _running(false), _acceptingTask(false) {}

void Executor::start() {
  if (_running)
    return;
  _acceptingTask.store(true);
  _running.store(true);
  for (int i = 0; i < _concurrency; ++i) {
    _threads.emplace_back(std::thread([this]() { runner(); }));
  }
}

Executor::~Executor() {}

void Executor::finish() {
  {
    _acceptingTask.store(false);
    _running.store(false);
    _hasNewTask.notify_all();
  }
  std::for_each(_threads.begin(), _threads.end(), [](auto &t) { t.join(); });
}

void Executor::post(const std::shared_ptr<Task> &task) {
  if (!_acceptingTask)
    return;
  {
    std::lock_guard<std::mutex> lock(_mutex);
    _tasks.emplace_back(task);
    _hasNewTask.notify_one();
  }
}

void Executor::runner() {
  while (true) {
    std::shared_ptr<Task> task = nullptr;
    std::unique_lock<std::mutex> lock(_mutex);

    if (_tasks.empty()) {
      _hasNewTask.wait(lock);
    }
    lock.unlock();
    while (!_tasks.empty()) {
      std::lock_guard<std::mutex> lock(_mutex);
      task = std::move(_tasks.front());
      _tasks.pop_front();
      if (task)
        task->execute();
    }
    if (!_running) {
      break;
    }
  }
}

} // namespace converter
