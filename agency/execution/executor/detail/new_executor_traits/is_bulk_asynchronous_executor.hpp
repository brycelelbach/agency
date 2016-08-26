#pragma once

#include <agency/detail/config.hpp>
#include <agency/detail/type_traits.hpp>
#include <agency/detail/integer_sequence.hpp>
#include <agency/execution/executor/executor_traits.hpp>
#include <agency/execution/executor/detail/new_executor_traits/executor_execution_depth_or.hpp>
#include <agency/execution/executor/detail/new_executor_traits/member_shape_type_or.hpp>
#include <agency/execution/executor/detail/new_executor_traits/member_future_or.hpp>
#include <agency/execution/executor/detail/new_executor_traits/member_index_type_or.hpp>
#include <type_traits>
#include <utility>

namespace agency
{
namespace detail
{
namespace new_executor_traits_detail
{


template<class Executor, class Function, class Shape,
         class ResultFactory,
         class... SharedFactories
        >
struct has_bulk_async_execute_impl
{
  using result_type = result_of_t<ResultFactory(Shape)>;
  using expected_future_type = member_future_or_t<Executor,result_type,std::future>;

  template<class Executor1,
           class ReturnType = decltype(
             std::declval<Executor1>().bulk_async_execute(
               std::declval<Function>(),
               std::declval<Shape>(),
               std::declval<ResultFactory>(),
               std::declval<SharedFactories>()...
             )
           ),
           class = typename std::enable_if<
             std::is_same<ReturnType,expected_future_type>::value
           >::type>
  static std::true_type test(int);

  template<class>
  static std::false_type test(...);

  using type = decltype(test<Executor>(0));
};


template<class Executor, class Function, class Shape,
         class ResultFactory,
         class... SharedFactories
        >
using has_bulk_async_execute = typename has_bulk_async_execute_impl<Executor, Function, Shape, ResultFactory, SharedFactories...>::type;


template<class T, class IndexSequence>
struct is_bulk_asynchronous_executor_impl;

template<class T, size_t... Indices>
struct is_bulk_asynchronous_executor_impl<T, index_sequence<Indices...>>
{
  // executor properties
  using shape_type = member_shape_type_or_t<T, size_t>;
  using index_type = member_index_type_or_t<T, shape_type>;

  // types related to functions passed to .bulk_async_execute()
  using result_type = int;
  template<size_t>
  using shared_type = int;

  // the functions we'll pass to .bulk_async_execute() to test
  using test_function = std::function<void(index_type, result_type&, shared_type<Indices>&...)>;
  using test_result_factory = std::function<result_type(shape_type)>;

  // XXX we're passing the wrong shape_type -- we need to pick out the last (depth - Ith) dimensions of the shape_type
  template<size_t I>
  using test_shared_factory = std::function<shared_type<I>(shape_type)>;

  using type = has_bulk_async_execute<
    T,
    test_function,
    shape_type,
    test_result_factory,
    test_shared_factory<Indices>...
  >;
};

template<class T>
using is_bulk_asynchronous_executor = typename is_bulk_asynchronous_executor_impl<
  T,
  make_index_sequence<
    executor_execution_depth_or<T>::value
  >
>::type;


} // end new_executor_traits_detail
} // end detail
} // end agency

