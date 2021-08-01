#pragma once

#include <vector>

template <class counterInt_type, class operatingInt_type> class stateManager
{
 public:
 ~stateManager() = delete;
 static bool reserveFor(const std :: size_t& maxSize) noexcept;
 static counterInt_type counter;
 static std :: vector<operatingInt_type> countingBlock;
 static operatingInt_type lastCounting;
};

template <class counterInt_type, class operatingInt_type> counterInt_type stateManager <counterInt_type, operatingInt_type> :: counter = {};
template <class counterInt_type, class operatingInt_type> std :: vector<operatingInt_type> stateManager <counterInt_type, operatingInt_type> :: countingBlock = {};
template <class counterInt_type, class operatingInt_type> operatingInt_type stateManager <counterInt_type, operatingInt_type> :: lastCounting = {};

template <class counterInt_type, class operatingInt_type> inline bool stateManager <counterInt_type, operatingInt_type> :: reserveFor(const std :: size_t& maxSize) noexcept
{
 try
 {
  stateManager <counterInt_type, operatingInt_type> :: countingBlock.reserve(sqrtl(maxSize));
  return true;
 }
 catch (...) {}
 return false;
}

