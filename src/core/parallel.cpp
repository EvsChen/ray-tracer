#include "parallel.h"

#include <mutex>
#include <thread>
#include <vector>

namespace raytracer {

static std::vector<std::thread> threads;

class ParallelLoop {
 public:
  ParallelLoop(ThreadFunc1d func, int maxIndex, int chunkSize)
      : func1d(std::move(func)), maxIndex(maxIndex), chunkSize(chunkSize) {}

  ParallelLoop(const ThreadFunc2d &func, const Point2i &count)
      : func2d(func), maxIndex(count.x * count.y), chunkSize(1), nx(count.x) {}

  ThreadFunc1d func1d;
  ThreadFunc2d func2d;
  int nextIndex = 0;
  int activeWorkers = 0;
  int nx = -1;
  ParallelLoop *next = nullptr;
  const int maxIndex, chunkSize;

  bool finished() { return activeWorkers == 0 && nextIndex >= maxIndex; }
};
static ParallelLoop *workList = nullptr;
static std::mutex workListMutex;
static bool shutDownThreads = false;

static std::condition_variable workListCondition;
thread_local int threadIndex;

void workerFunc(int tIndex) {
  threadIndex = tIndex;
  std::unique_lock<std::mutex> lock(workListMutex);
  while (!shutDownThreads) {
    if (!workList) {
      workListCondition.wait(lock);
    } else {
      ParallelLoop &loop = *workList;
      // Determine the range that will be executed in the current thread
      int indexStart = loop.nextIndex;
      int indexEnd = std::min(loop.nextIndex + loop.chunkSize, loop.maxIndex);
      loop.nextIndex = indexEnd;
      if (indexEnd == loop.maxIndex) workList = loop.next;
      loop.activeWorkers++;

      // Execute the loop
      lock.unlock();
      for (int i = indexStart; i < indexEnd; ++i) {
        if (loop.func1d) {
          loop.func1d(i);
        } else {
          loop.func2d(Point2i(i % loop.nx, i / loop.nx));
        }
      }
      lock.lock();
      loop.activeWorkers--;
      if (loop.finished()) workListCondition.notify_all();
    }
  }
}

void parallelInit() {
  int maxThreads = 12;
  // 1 less than the number of max cores
  // left for the main thread
  threadIndex = 0;
  for (int i = 0; i < maxThreads - 1; i++) {
    threads.push_back(std::thread(workerFunc, i + 1));
  }
}

void parallelClean() {
  if (threads.empty()) return;
  {
    std::lock_guard<std::mutex> lock(workListMutex);
    shutDownThreads = true;
    workListCondition.notify_all();
  }

  for (auto &thread : threads) thread.join();
  threads.erase(threads.begin(), threads.end());
  shutDownThreads = false;
}

void ParallelFor(ThreadFunc1d func, int count, int chunkSize) {
  if (threads.empty() || count < chunkSize) {
    for (int i = 0; i < count; ++i) func(i);
    return;
  }
  ParallelLoop loop(std::move(func), count, chunkSize);
  workListMutex.lock();
  loop.next = workList;
  workList = &loop;
  workListMutex.unlock();

  std::unique_lock<std::mutex> lock(workListMutex);
  workListCondition.notify_all();
  while (!loop.finished()) {
    int indexStart = loop.nextIndex;
    int indexEnd = std::min(loop.nextIndex + loop.chunkSize, loop.maxIndex);
    if (indexEnd == loop.maxIndex) workList = loop.next;
    loop.nextIndex = indexEnd;
    loop.activeWorkers++;

    // Execute the loop
    lock.unlock();
    for (int i = indexStart; i < indexEnd; ++i) {
      if (loop.func1d) {
        loop.func1d(i);
      } else {
        loop.func2d(Point2i(i % loop.nx, i / loop.nx));
      }
    }
    lock.lock();
    loop.activeWorkers--;
  }
}

void ParallelFor2d(ThreadFunc2d func, const Point2i &count) {
  if (threads.empty() || count.x * count.y <= 1) {
    for (int i = 0; i < count.x; ++i) {
      for (int j = 0; j < count.y; ++j) {
        func(Point2i(i, j));
      }
    }
    return;
  }

  ParallelLoop loop(std::move(func), count);
  workListMutex.lock();
  loop.next = workList;
  workList = &loop;
  workListMutex.unlock();

  std::unique_lock<std::mutex> lock(workListMutex);
  workListCondition.notify_all();
  while (!loop.finished()) {
    int indexStart = loop.nextIndex;
    int indexEnd = std::min(loop.nextIndex + loop.chunkSize, loop.maxIndex);
    loop.nextIndex = indexEnd;
    if (indexEnd == loop.maxIndex) workList = loop.next;
    loop.activeWorkers++;

    // Execute the loop
    lock.unlock();
    for (int i = indexStart; i < indexEnd; ++i) {
      if (loop.func1d) {
        loop.func1d(i);
      } else {
        loop.func2d(Point2i(i % loop.nx, i / loop.nx));
      }
    }
    lock.lock();
    loop.activeWorkers--;
  }
}
}  // namespace raytracer
