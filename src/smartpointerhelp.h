#pragma once
#include <memory>

#define mkU std::make_unique
#define mkS std::make_shared
template <typename ...Args> using uPtr = std::unique_ptr<Args...>;
template <typename ...Args> using sPtr = std::shared_ptr<Args...>;
