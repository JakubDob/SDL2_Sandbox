#pragma once
#include "Types.h"
#include <iostream>
#include <utility>

namespace utils {
namespace detail {
template <typename T, typename... Ts>
struct UniqueTypes : std::conjunction<std::negation<std::is_same<T, Ts>>...,
                                      UniqueTypes<Ts...>> {};

template <typename T>
struct UniqueTypes<T> : std::true_type {};

template <typename T, typename... Ts, auto... Ints>
constexpr auto getTypeIndexImpl(std::index_sequence<Ints...>&&) {
    return ((Ints * std::is_same_v<T, Ts>)+... + 0);
}
}  // namespace detail

template <typename It, typename Cmp>
void insertionSort(It begin, It end, Cmp cmp) {
    using Type = typename It::value_type;
    for (It i = begin + 1; i < end; ++i) {
        Type tmp = *i;
        It j = i - 1;
        while (j >= begin && cmp(tmp, *j)) {
            *(j + 1) = *j;
            j--;
        }
        *(j + 1) = tmp;
    }
}

/*
Sort just indices without modifying the original collection
*/
template <typename SrcVec, typename IdxVec, typename Cmp>
void insertionSort(SrcVec const& src, IdxVec& idx, Cmp cmp) {
    for (auto i = 1; i < idx.size(); ++i) {
        auto tmpI = idx[i];
        auto j = i - 1;
        while (j >= 0 && cmp(src[tmpI], src[idx[j]])) {
            idx[j + 1] = idx[j];
            j--;
        }
        idx[j + 1] = tmpI;
    }
}

// Data must be sorted indirectly using array of indices
// Returns iterators of the index array
template <typename SrcVec, typename IdxVec, typename S, typename Lt,
          typename Eq>
constexpr auto binarySearch(SrcVec const& src, IdxVec const& idx,
                            S const& value, Lt lt, Eq eq) {
    auto firstI = idx.begin();
    auto lastI = idx.end() - 1;

    while (firstI <= lastI) {
        // mid value of the index set
        auto midI = firstI + (lastI - firstI) / 2;
        if (eq(value, src[*midI])) {
            // return leftmost value if there are duplicates
            while (--midI >= firstI && eq(value, src[*midI])) {
            }
            ++midI;
            return midI;
        }
        if (lt(value, src[*midI])) {
            lastI = midI - 1;
        } else {
            firstI = midI + 1;
        }
    }
    return idx.end();
}

// Data must be sorted directly from begin to end
template <typename It, typename S, typename Lt, typename Eq>
constexpr It binarySearch(It begin, It end, S const& value, Lt lt, Eq eq) {
    It first = begin;
    It last = end - 1;
    while (first <= last) {
        It mid = first + (last - first) / 2;
        if (eq(value, *mid)) {
            // return leftmost value if there are duplicates
            while (--mid >= first && eq(value, *mid)) {
            }
            return mid + 1;
        }
        if (lt(value, *mid)) {
            last = mid - 1;
        } else {
            first = mid + 1;
        }
    }
    return end;
}

template <typename T, typename... Ts>
constexpr auto getTypeIndex() {
    return detail::getTypeIndexImpl<T, Ts...>(
        std::make_index_sequence<sizeof...(Ts)>{});
}

template <typename T, typename... Ts>
constexpr bool containsType() {
    return std::disjunction_v<std::is_same<T, Ts>...>;
}

template <typename T, typename R, typename... Args>
constexpr bool verifyFunctionReturnType(R (*)(Args...)) {
    return std::is_same_v<R, T>;
}

template <typename T, typename... Ts>
constexpr bool containsTypeInSequence(TypeSequence<Ts...>) {
    return std::disjunction_v<std::is_same<T, Ts>...>;
}

template <typename T, typename... Ts>
constexpr auto getTypeIndexFromSequence(TypeSequence<Ts...>) {
    return utils::getTypeIndex<T, Ts...>();
}
template <typename... Ts>
constexpr auto getSizeOfSequence(TypeSequence<Ts...>) {
    return sizeof...(Ts);
}

template <typename T>
inline constexpr addr_t getAlignedAddress(addr_t addr) {
    bool isAligned = (addr % alignof(T)) == 0;
    if (isAligned) {
        return addr;
    } else {
        return addr + alignof(T) - (addr % alignof(T));
    }
}
}  // namespace utils
