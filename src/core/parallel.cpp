#include "parallel.h"
#include <thread>
#include <vector>
#include <mutex>

static std::vector<std::thread> threads;
class ParallelLoop;
static ParallelLoop *workList = nullptr;
static std::mutex workListMutex;

void workerFunc(int tIndex) {
  threadIndex = tIndex;
  std::unique_lock<std::mutex> lock(workListMutex);
}

void initParallel() {
  int maxThreads = 12;
  // 1 less than the number of max cores
  // left for the main thread
  threadIndex = 0;
  for (int i = 0; i < maxThreads - 1; i++) {
    threads.push_back(std::thread(workerFunc, i + 1));
  }
}