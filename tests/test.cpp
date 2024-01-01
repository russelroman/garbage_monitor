#include "CppUTest/TestHarness.h"

extern "C" {
  
}

TEST_GROUP(TestMySum) {
  void setup() {
  }

  void teardown() {
  }
};

TEST(TestMySum, Test_MySumBasic) {
  LONGS_EQUAL(7, 3+3);
}