#pragma once

#include <crap/algorithm/copyvalue.h>
#include <crap/algorithm/transform2value.h>
#include <crap/functional/logicaloperatorsfortype.h>
#include <crap/functional/multipliesvalue.h>
#include <crap/numeric/iotavalue.h>
#include <crap/utility/reproducevalue.h>
#include <crap/utility/valuedemultiplexer.h>

#include <atomic>
#include <type_traits>
#include <utility>

template <class Type1, class Type2, class ResType, template <Type1, Type2> class Operator, Type1 ... Values1> struct transform2ValueTyped
{
 template <Type2 ... Values2> using with = crap :: copyValue<ResType, static_cast<ResType>(Operator <Values1, Values2> :: value)...>;
};

template <class Type1, class Type2, class ResType, template <Type1, Type2> class Operator> struct transform2ValueTypedForType
{
 template <Type1 ... Values> using type = transform2ValueTyped<Type1, Type2, ResType, Operator, Values...>;
};

template <class Type, Type ... Values> struct arrayer
{
 private:
 constexpr const static Type values[sizeof...(Values)] = {Values...};
 public:
 constexpr const static Type (& array() noexcept)[sizeof...(Values)];
};

template <class Type, Type ... Values> inline constexpr const Type (& arrayer <Type, Values...> :: array() noexcept)[sizeof...(Values)]
{
 return arrayer <Type, Values...> :: values;
}

template <class Type> struct arrayerForType
{
 template <Type ... Values> using type = arrayer<Type, Values...>;
};

template <std :: size_t ... Primes>  struct logic
{
 public:
 constexpr const static auto tableSize = crap :: multipliesValue <std :: size_t, Primes...> :: value;
 private:
 template <std :: size_t ... Stepps> struct stepps;
 template <std :: size_t Index, std :: size_t Stepp> using nextRest = std :: integral_constant<std :: size_t, (Index + Stepp) % tableSize>;
 template <std :: size_t ... Stepps> struct processors;
 public:
 template <template <std :: size_t...> class Container>
 using getStepps = typename crap :: reproduceValue <tableSize, std :: size_t, 2u> :: template type <stepps> :: template type<Container>;
 template <template <std :: size_t...> class Container>
 using getRests = typename getStepps <crap :: iotaValue <tableSize, std :: size_t> :: template type <transform2ValueTypedForType <std :: size_t, std :: size_t, std :: size_t, nextRest> :: template type> :: template with> :: template type <Container>;
 using getProcessors = getStepps<processors>;
 constexpr static std :: size_t getPrimesToSkip() noexcept;
};

template <std :: size_t ... Primes> template <std :: size_t ... Stepps> struct logic <Primes...> :: stepps
{
 private:
 constexpr const static std :: size_t tableSize = logic <Primes...> :: tableSize;
 template <std :: size_t Step, std :: size_t Index> using maybePrime = std :: integral_constant<bool, (crap :: multipliesValue <std :: size_t, (((Index + Step) % tableSize) % Primes)...> :: value != 0)>;
 template <std :: size_t Jump, std :: size_t Index> using jumpStep = std :: integral_constant<std :: size_t,((maybePrime <Jump, Index> :: value) ? Jump : (Jump + 2u))>;
 constexpr const static auto ready = crap :: iotaValue <sizeof...(Stepps), std :: size_t> :: template type <transform2ValueTyped <std :: size_t, std :: size_t, bool, maybePrime, Stepps...> :: template with> :: template type <crap :: logicalOperatorsForType <bool> :: template LogicalAnd> :: value;
 template <std :: size_t ... PassedStepps> using ifReady = crap :: copyValue<std :: size_t, PassedStepps...>;
 template <std :: size_t ... PassedStepps> using ifNotReady = typename crap :: iotaValue <sizeof...(PassedStepps), std :: size_t> :: template type<transform2ValueTyped <std :: size_t, std :: size_t, std :: size_t, jumpStep, PassedStepps...> :: template with> :: template type<stepps>;
 public:
 template <template <std :: size_t...> class Container>
 using type = typename crap :: valueDemultiplexer <std :: size_t, ready, crap :: copyValue<std :: size_t, Stepps...> :: template type, ifReady, ifNotReady> :: type :: template type<Container>;
};

template <std :: size_t ... Primes> template <std :: size_t ... Stepps> struct logic <Primes...> :: processors
{
 private:
 static_assert(sizeof...(Stepps) == (logic <Primes...> :: tableSize), "Elements must match.");
 template <class RadixType, class DigitType, class FlagType, std :: size_t N> static void process(const RadixType& constant, DigitType& counter, FlagType& flag);
 public:
 template <class RadixType, class DigitType, class FlagType> /*constexpr*/ static void (* const (& getProcessors() noexcept) [logic <Primes...> :: tableSize])(const RadixType&, DigitType&, FlagType&);
};

template <std :: size_t ... Primes> constexpr inline std :: size_t logic <Primes...> :: getPrimesToSkip() noexcept
{
 return (sizeof...(Primes) + 1u);
}

template <std :: size_t ... Primes> template <std :: size_t ... Stepps> template <class RadixType, class DigitType, class FlagType, std :: size_t N> inline void logic <Primes...> :: template processors <Stepps...> :: process(const RadixType& constant, DigitType& counter, FlagType& flag)
{
 for(counter += N;constant <= counter;counter -= constant);
 if (counter == 0u) flag = true;
}

template <std :: size_t ... Primes>
template <std :: size_t ... Stepps>
template <class RadixType, class DigitType, class FlagType>
inline /*constexpr*/ void (* const (& logic <Primes...> :: template processors <Stepps...> :: getProcessors() noexcept) [logic <Primes...> :: tableSize])(const RadixType&, DigitType&, FlagType&)
{
 using This = typename logic <Primes...> :: template processors<Stepps...>;
 /*constexpr*/ static void (* const addresses [logic <Primes...> :: tableSize])(const RadixType&, DigitType&, FlagType&) = {&(This :: process<RadixType, DigitType, FlagType, Stepps>)...};
 return addresses;
}

using steps = logic<3u, 5u, 7u>;
