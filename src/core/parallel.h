#pragma once

#include <functional>

#include "geometry.h"

namespace raytracer {

using ThreadFunc1d = std::function<void(int)>;
using ThreadFunc2d = std::function<void(Point2i)>;

extern thread_local int threadIndex;

void parallelInit();
void parallelClean();
void ParallelFor(ThreadFunc1d func, int count, int chunkSize);
void ParallelFor2d(ThreadFunc2d func, const Point2i &count);

}  // namespace raytracer
