#pragma once

#include "Task.h"

#include <atomic>
#include <list>
#include <memory>
#include <thread>
#include <vector>

namespace converter {

class Executor {
public:
  Executor(const size_t concurrency = 1u);
  virtual ~Executor();
  void post(const std::shared_ptr<Task> &task);
  void start();
  void finish();

private:
  void runner();
  std::vector<std::thread> _threads;
  const size_t _concurrency;
  std::list<std::shared_ptr<Task>> _tasks;
  std::mutex _mutex;
  std::atomic<bool> _running;
  std::condition_variable _hasNewTask;
  std::atomic<bool> _acceptingTask;
  std::condition_variable _allTasksFinished;
};

} // namespace converter
