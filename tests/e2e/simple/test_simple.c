#include "Mocksimple_dep.h"
#include "unity.h"

void
setUp(void)
{
  Mocksimple_dep_Init();
}

void
tearDown(void)
{
  Mocksimple_dep_Verify();
  Mocksimple_dep_Destroy();
}

void
test_mock_returns_canned_value(void)
{
  simple_dep_get_ExpectAndReturn(42);
  TEST_ASSERT_EQUAL_INT(42, simple_dep_get());
}

int
main(void)
{
  UNITY_BEGIN();
  RUN_TEST(test_mock_returns_canned_value);
  return UNITY_END();
}
