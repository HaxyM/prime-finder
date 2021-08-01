#pragma once

#include <iostream>
#include <vector>

#include "logic.h"
#include "statemanager.h"

template <class counterInt_type, class operatingInt_type> class resultManager
{
 public:
 ~resultManager() = delete;
 static bool init(const std :: size_t& maxSize) noexcept;
 static void addFound(const counterInt_type& result);
 static operatingInt_type extendBlock() noexcept;
 static std :: size_t foundSize() noexcept;
 static void print(std :: ostream& output);
 static auto getRadicesBegin() noexcept;
 static auto getRadicesEnd() noexcept;
 private:
 constexpr const static std :: size_t smallPrimes = 203'280'221ul;
 static std :: vector<operatingInt_type> found;
 static std :: vector<counterInt_type> foundLarge;
 static typename decltype(found) :: const_iterator radicesBegin;
 static typename decltype(found) :: const_iterator radicesEnd;
 static std :: size_t expectedSize;
};

template <class counterInt_type, class operatingInt_type> std :: vector<operatingInt_type> resultManager <counterInt_type, operatingInt_type> :: found = {};
template <class counterInt_type, class operatingInt_type> std :: vector<counterInt_type> resultManager <counterInt_type, operatingInt_type> :: foundLarge = {};
template <class counterInt_type, class operatingInt_type> typename decltype(resultManager <counterInt_type, operatingInt_type> :: found) :: const_iterator resultManager <counterInt_type, operatingInt_type> :: radicesBegin = std :: next(resultManager <counterInt_type, operatingInt_type> :: found.cbegin(), steps :: getPrimesToSkip());
template <class counterInt_type, class operatingInt_type> typename decltype(resultManager <counterInt_type, operatingInt_type> :: found) :: const_iterator resultManager <counterInt_type, operatingInt_type> :: radicesEnd = resultManager <counterInt_type, operatingInt_type> :: radicesBegin;
template <class counterInt_type, class operatingInt_type> std :: size_t resultManager <counterInt_type, operatingInt_type> :: expectedSize = {};

template <class counterInt_type, class operatingInt_type> inline bool resultManager <counterInt_type, operatingInt_type> :: init(const std :: size_t& maxSize) noexcept
{
 const operatingInt_type initialFound[] = {2u, 3u, 5u, 7u, 11u, 13u, 17u, 19u, 23u, 29u, 31u, 37u, 41u, 43u, 47u, 53u, 59u, 61u, 67u, 71u, 73u, 79u, 83u, 89u, 97u, 101u, 103u, 107u, 109u, 113u, 127u};
 try
 {
  resultManager <counterInt_type, operatingInt_type> :: found.reserve(smallPrimes);
  if (!(stateManager <counterInt_type, operatingInt_type> :: reserveFor(maxSize))) return false;
  std :: copy(std :: begin(initialFound), std :: end(initialFound), std :: back_inserter(found));
  stateManager <counterInt_type, operatingInt_type> :: counter = 127u;
  stateManager <counterInt_type, operatingInt_type> :: countingBlock = {127u % 11u};
  stateManager <counterInt_type, operatingInt_type> :: lastCounting = 127u % 13u;
  resultManager <counterInt_type, operatingInt_type> :: radicesBegin = std :: next(resultManager <counterInt_type, operatingInt_type> :: found.cbegin(), steps :: getPrimesToSkip());
  resultManager <counterInt_type, operatingInt_type> :: radicesEnd = std :: next(resultManager <counterInt_type, operatingInt_type> :: radicesBegin, stateManager <counterInt_type, operatingInt_type> :: countingBlock.size());
  resultManager <counterInt_type, operatingInt_type> :: expectedSize = maxSize;
  return true;
 }
 catch (...) {}
 return false;
}

template <class counterInt_type, class operatingInt_type> inline void resultManager <counterInt_type, operatingInt_type> :: addFound(const counterInt_type& result)
{
 static bool inLarge = false;
 if (inLarge)
 {
  foundLarge.push_back(result);
 }
 else
 {
  if (result > std :: numeric_limits<typename decltype(resultManager <counterInt_type, operatingInt_type> :: found) :: value_type> :: max())
  {
   const auto countingBlockSize = std :: distance(resultManager <counterInt_type, operatingInt_type> :: radicesBegin, resultManager <counterInt_type, operatingInt_type> :: radicesEnd);
   resultManager <counterInt_type, operatingInt_type> :: found.shrink_to_fit();
   resultManager <counterInt_type, operatingInt_type> :: radicesBegin = std :: next(resultManager <counterInt_type, operatingInt_type> :: found.cbegin(), steps :: getPrimesToSkip());
   resultManager <counterInt_type, operatingInt_type> :: radicesEnd = std :: next(resultManager <counterInt_type, operatingInt_type> :: radicesBegin, countingBlockSize);
   resultManager <counterInt_type, operatingInt_type> :: foundLarge.reserve(resultManager <counterInt_type, operatingInt_type> :: expectedSize - found.size());
   resultManager <counterInt_type, operatingInt_type> :: foundLarge.push_back(result);
   inLarge = true;
  }
  else
  {
   found.push_back(static_cast<typename decltype(resultManager <counterInt_type, operatingInt_type> :: found) :: value_type>(result));
  }
 }
}

template <class counterInt_type, class operatingInt_type> inline operatingInt_type resultManager <counterInt_type, operatingInt_type> :: extendBlock() noexcept
{
 std :: advance(resultManager <counterInt_type, operatingInt_type> :: radicesEnd, 1u);
 return *(resultManager <counterInt_type, operatingInt_type> :: radicesEnd);
}

template <class counterInt_type, class operatingInt_type> inline std :: size_t resultManager <counterInt_type, operatingInt_type> :: foundSize() noexcept
{
 return resultManager <counterInt_type, operatingInt_type> :: found.size() + resultManager <counterInt_type, operatingInt_type> :: foundLarge.size();
}

template <class counterInt_type, class operatingInt_type> inline void resultManager <counterInt_type, operatingInt_type> :: print(std :: ostream& output)
{
 std :: copy(resultManager <counterInt_type, operatingInt_type> :: found.cbegin(), resultManager <counterInt_type, operatingInt_type> :: found.cend(), std :: ostream_iterator<typename decltype(resultManager <counterInt_type, operatingInt_type> :: found) :: value_type>(output, ", "));
 std :: cout << found.size() << " 32bit primes found. Largest " << found.back() << std :: endl;
 std :: copy(resultManager <counterInt_type, operatingInt_type> :: foundLarge.cbegin(), resultManager <counterInt_type, operatingInt_type> :: foundLarge.cend(), std :: ostream_iterator<typename decltype(resultManager <counterInt_type, operatingInt_type> :: foundLarge) :: value_type>(output, ", "));
}

template <class counterInt_type, class operatingInt_type> inline auto resultManager <counterInt_type, operatingInt_type> :: getRadicesBegin() noexcept
{
 return resultManager <counterInt_type, operatingInt_type> :: radicesBegin;
}

template <class counterInt_type, class operatingInt_type> inline auto resultManager <counterInt_type, operatingInt_type> :: getRadicesEnd() noexcept
{
 return resultManager <counterInt_type, operatingInt_type> :: radicesEnd;
}
