#include <algorithm>
#include <atomic>
#include <cmath>
#include <execution>
#include <fstream>
#include <chrono>
#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <iterator>
#include <list>
#include <map>
#include <optional>
#include <type_traits>
#include <vector>

#include "logic.h"
#include "resultmanager.h"
#include "statemanager.h"

using counterInt_type = std :: uint64_t;
using operatingInt_type = std :: uint32_t;

using results = resultManager <counterInt_type, operatingInt_type>;
using state = stateManager <counterInt_type, operatingInt_type>;

/*constexpr const*/ auto nextRest = steps :: template getRests <arrayerForType <std :: size_t> :: template type> :: array();
/*constexpr const*/ auto counterStep = steps :: template getStepps <arrayerForType <std :: size_t> :: template type> :: array();
/*constexpr const*/ auto processors = steps :: getProcessors :: getProcessors<operatingInt_type, operatingInt_type, std :: atomic<bool> >();

template <class Type, std :: size_t N> constexpr std :: size_t elements(Type(&)[N]) {return N;}


enum class param
{
 blockSize,
 help,
 input,
 output
};

constexpr const static unsigned long int maxSize = 1'000'000'000ul;
constexpr const static unsigned long int step = maxSize / 10'000ul;

std :: ostream* output = &std :: cout;
std :: optional<std :: ifstream> input = {};

void parseArgs(int argc, char* argv[])
{
 const auto paramMap = []()
 {
  std :: map<std :: string, param> ans;
  ans.emplace("-o", param :: output);
  ans.emplace("--output", param :: output);
  ans.emplace("-i", param :: input);
  ans.emplace("--input", param :: input);
  ans.emplace("-s", param :: blockSize);
  ans.emplace("--block-size", param :: blockSize);
  ans.emplace("-h", param :: help);
  ans.emplace("/?", param :: help);
  ans.emplace("--help", param :: help);
  return ans;
 }();

 for(const auto end = argv + argc;argv != end;)
 {
  std :: cout << "processing: " << *argv << std :: endl;
  if (const auto found = paramMap.find(*argv++); found != paramMap.cend())
  {
   switch(found->second)
   {
    case param :: output:
    std :: cout << "opening file" << std :: endl;
    output = new std :: ofstream(*argv++);
    std :: cout << "File opened: " << std :: boolalpha << output->good() << std :: endl;
    break;
    case param :: input:
    input.emplace(*argv++);
    std :: cout << "Input file opened: " << std :: boolalpha << input->good() << std :: endl;
    break;
   };
  }
  else
  {
   std :: cerr << "Failed to parse arguments" << std :: endl;
   exit(EXIT_FAILURE);
  }
 }
}

int main(int argc, char* argv[])
{
 if (!(results :: init(maxSize)))
 {
  std :: cerr << "Failed to initialise." << std :: endl;
  return EXIT_FAILURE;
 }
 //countingBlock.reserve(sqrtl(maxSize));
 unsigned long int nextStep = step;
 parseArgs(--argc, ++argv);
 std :: cout << "Done parsing arguments" << std :: endl;
 std :: cout << "Build at: " << __DATE__ << std :: endl;
 if (input && (!results :: load(std :: istream_iterator<counterInt_type>(*input), std :: istream_iterator<counterInt_type>())))
 {
  std :: cerr << "Failed to load input data." << std :: endl;
  return EXIT_FAILURE;
 }

 auto  lastConstant = *(results :: getRadicesEnd());
 std :: cout << "lastConstant is " << lastConstant << std :: endl;
 std :: atomic<bool> blockFound;
 std :: atomic<bool> lastFound;
 const std :: chrono :: time_point<std :: chrono :: high_resolution_clock> start = std :: chrono :: high_resolution_clock :: now();
 std :: chrono :: time_point<std :: chrono :: high_resolution_clock> prevExtend = start;
 std :: chrono :: time_point<std :: chrono :: high_resolution_clock> prevStep = start;
 auto prevCounter = state :: counter;
 auto currentRest = state :: counter % steps :: tableSize;
 
 while(results :: foundSize() < maxSize)
 {
  state :: counter += counterStep[currentRest];
  lastFound = false;
  blockFound = false;
  const auto processor = processors[currentRest];
  std :: transform(std :: execution :: par, state :: countingBlock.cbegin(), state :: countingBlock.cend(), results :: getRadicesBegin(), state :: countingBlock.begin(), [&blockFound, processor](decltype(state :: countingBlock) :: value_type counter, const auto& constant)
  {
   processor(constant, counter, blockFound);
   return counter;
  });
  processor(lastConstant, state :: lastCounting, lastFound);
  currentRest = nextRest[currentRest];
  if (!blockFound)
  {
   if (lastFound)
   {
    state :: countingBlock.push_back(state :: lastCounting);
    lastConstant = results :: extendBlock();
    state :: lastCounting = state :: counter % lastConstant;
    const std :: chrono :: time_point<std :: chrono :: high_resolution_clock> newPrevExtend = std :: chrono :: high_resolution_clock :: now();
    const auto duration = std :: chrono :: duration_cast<std :: chrono :: milliseconds>(newPrevExtend - prevExtend);
    std :: cout << "Extending conunting block to size " << state :: countingBlock.size() << " after " << duration.count() << "ms" << std :: endl;
    prevExtend = newPrevExtend;
   }
   else
   {
    results :: addFound(state :: counter);
    if (results :: foundSize() == nextStep)
    {
     const std :: chrono :: time_point<std :: chrono :: high_resolution_clock> newPrevStep = std :: chrono :: high_resolution_clock :: now();
     const auto duration = std :: chrono :: duration_cast<std :: chrono :: seconds>(newPrevStep - prevStep);
     const auto primesSpeed = static_cast<const long double>( step) / duration.count();
     const auto totalSpeed = static_cast<const long double>(state :: counter - prevCounter) / duration.count();
     const auto globalSpeed = static_cast<const long double>(state :: counter) / std :: chrono :: duration_cast<std :: chrono :: seconds>(newPrevStep - start).count();
     std :: cout << "Found " << nextStep << " of " << maxSize << "; " << primesSpeed << " primes per s; " << totalSpeed << " checks per s; global " << globalSpeed << " checks per s" << std :: endl;
     prevCounter = state :: counter;
     prevStep = newPrevStep;
     nextStep += step;
    }
   }
  }
 }
 const std :: chrono :: time_point<std :: chrono :: high_resolution_clock> stop = std :: chrono :: high_resolution_clock :: now();
 std :: cout << "Found primes: ";
 results :: print(*output);
 std :: cout << "flushing result: " << std :: boolalpha << static_cast<bool>((*output) << std :: flush) << std :: endl;
 std :: cout << std :: endl;
 auto duration = std :: chrono :: duration_cast<std :: chrono :: milliseconds>(stop - start);
 std :: cout << "In: " << duration.count() << "ms" << std :: endl;
 std :: cout << "Counting on set of " << state :: countingBlock.size() << " primes" << std :: endl;
 std :: cout << "Largest in operation is: " << lastConstant << " (0x" << std :: hex << lastConstant << ")" << std :: endl;
 if (output != &std :: cout)
 {
  (reinterpret_cast<std :: ofstream*>(output))->close();
  delete output;
 }
 return EXIT_SUCCESS;
}
