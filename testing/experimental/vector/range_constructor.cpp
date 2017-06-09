#include <iostream>
#include <cassert>
#include <algorithm>
#include <vector>
#include <list>
#include <agency/experimental/vector.hpp>

template<class Container>
void test_range_constructor()
{
  using namespace agency::experimental;

  using value_type = typename Container::value_type;

  size_t num_elements_to_insert = 5;
  Container items(num_elements_to_insert);
  std::iota(items.begin(), items.end(), 0);

  vector<value_type> v(items.begin(), items.end());

  assert(v.size() == num_elements_to_insert);
  assert(std::equal(v.begin(), v.end(), items.begin()));
}


int main()
{
  test_range_constructor<std::vector<int>>();

  test_range_constructor<std::list<int>>();

  std::cout << "OK" << std::endl;

  return 0;
}

