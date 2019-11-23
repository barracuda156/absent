#ifndef RVARAGO_ABSENT_FMAP_H
#define RVARAGO_ABSENT_FMAP_H

#include "absent/nullable/syntax.h"
#include "absent/support/member.h"

#include <functional>

namespace rvarago::absent {

/***
 * Given a nullable type N<A> (i.e. optional like object), and an unary function f: A -> B:
 * - When empty: it should return a new empty nullable N<B>.
 * - When *not* empty: it should return a value generated by applying the unary mapping function to the input
 * nullable's wrapped in a brand new nullable N<B>.
 *
 * @param input any optional like object that can be checked against empty and provide access to its wrapped value.
 * @param fn an unary function A -> B.
 * @return a new nullable containing the mapped value of type B, possibly empty if input is also empty.
 */
template <template <typename...> typename Nullable, typename UnaryFunction, typename A, typename... Rest>
constexpr decltype(auto) fmap(Nullable<A, Rest...> const &input, UnaryFunction mapper) noexcept {
    using namespace nullable::syntax;
    using ValueT = decltype(mapper(std::declval<A>()));
    if (empty<Nullable, A, Rest...>::_(input)) {
        return make_empty<Nullable<ValueT, Rest...>>::_(input);
    }
    auto const input_value = value<Nullable, A, Rest...>::_(input);
    return make<Nullable, ValueT, Rest...>::_(mapper(input_value));
}

/***
 * The same as fmap but for a member function that has to be const and parameterless.
 */
template <template <typename...> typename Nullable, typename A, typename B, typename... Rest>
constexpr decltype(auto) fmap(Nullable<A, Rest...> const &input, support::member_mapper<const A, B> mapper) noexcept {
    return fmap(input, [&mapper](auto const &value) { return std::invoke(mapper, value); });
}

/***
 * Infix version of fmap.
 */
template <template <typename...> typename Nullable, typename Mapper, typename A, typename... Rest>
constexpr decltype(auto) operator|(Nullable<A, Rest...> const &input, Mapper mapper) noexcept {
    return fmap(input, mapper);
}

/**
 * Infix version of fmap for a member function.
 */
template <template <typename...> typename Nullable, typename A, typename B, typename... Rest>
constexpr decltype(auto) operator|(Nullable<A, Rest...> const &input,
                                   support::member_mapper<const A, B> mapper) noexcept {
    return fmap(input, mapper);
}

}

#endif // RVARAGO_ABSENT_FMAP_H
