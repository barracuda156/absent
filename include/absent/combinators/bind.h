#ifndef RVARAGO_ABSENT_BIND_H
#define RVARAGO_ABSENT_BIND_H

#include "absent/nullable/syntax.h"
#include "absent/support/member.h"

#include <functional>

namespace rvarago::absent {

/***
 * Given a nullable type N<A> (i.e. optional like object), and an unary function f: A -> N<B>:
 * - When empty: it should return a new empty nullable N<B>.
 * - When *not* empty: it should return a value generated by applying the unary mapping function to the input
 * nullable's which is already wrapped a nullable N<B>.
 *
 * @param input any optional like object that can be checked against empty and provide access to its wrapped value.
 * @param fn an unary function A -> N<B>.
 * @return a new nullable generated by fn, possibly empty if input is also empty.
 */
template <template <typename...> typename Nullable, typename UnaryFunction, typename A, typename... Rest>
constexpr decltype(auto) bind(Nullable<A, Rest...> const &input, UnaryFunction mapper) noexcept {
    using namespace nullable::syntax;
    using ResultT = decltype(mapper(std::declval<A>()));
    if (empty<Nullable, A, Rest...>::_(input)) {
        return make_empty<ResultT>::_(input);
    }
    auto const input_value = value<Nullable, A, Rest...>::_(input);
    return mapper(input_value);
}

/***
 * The same as bind but for a member function that has to be const and parameterless.
 */
template <template <typename...> typename Nullable, typename A, typename B, typename... Rest>
constexpr decltype(auto) bind(Nullable<A, Rest...> const &input,
                              support::member_mapper<const A, Nullable<B, Rest...>> mapper) noexcept {
    return bind(input, [&mapper](auto const &value) { return std::invoke(mapper, value); });
}

/***
 * Infix version of bind.
 */
template <template <typename...> typename Nullable, typename UnaryFunction, typename A, typename... Rest>
constexpr decltype(auto) operator>>(Nullable<A, Rest...> const &input, UnaryFunction mapper) noexcept {
    return bind(input, mapper);
}

/***
 * Infix version of bind for a member function.
 */
template <template <typename...> typename Nullable, typename A, typename B, typename... Rest>
constexpr decltype(auto) operator>>(Nullable<A, Rest...> const &input,
                                    support::member_mapper<const A, Nullable<B, Rest...>> mapper) noexcept {
    return bind(input, mapper);
}

}

#endif // RVARAGO_ABSENT_BIND_H
