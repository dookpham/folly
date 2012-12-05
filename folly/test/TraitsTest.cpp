/*
 * Copyright 2012 Facebook, Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "folly/Benchmark.h"
#include "folly/Traits.h"

#include <gflags/gflags.h>
#include <gtest/gtest.h>

using namespace folly;
using namespace std;

struct T1 {}; // old-style IsRelocatable, below
struct T2 {}; // old-style IsRelocatable, below
struct T3 { typedef std::true_type IsRelocatable; };
struct T4 { typedef std::true_type IsTriviallyCopyable; };
struct T5 : T3 {};

struct F1 {};
struct F2 { typedef int IsRelocatable; };
struct F3 : T3 { typedef std::false_type IsRelocatable; };
struct F4 : T1 {};

namespace folly {
  template <> struct IsRelocatable<T1> : std::true_type {};
  template <> FOLLY_ASSUME_RELOCATABLE(T2);
}

TEST(Traits, scalars) {
  EXPECT_TRUE(IsRelocatable<int>::value);
  EXPECT_TRUE(IsRelocatable<bool>::value);
  EXPECT_TRUE(IsRelocatable<double>::value);
  EXPECT_TRUE(IsRelocatable<void*>::value);
}

TEST(Traits, containers) {
  EXPECT_TRUE  (IsRelocatable<vector<F1>>::value);
  EXPECT_FALSE((IsRelocatable<pair<F1, F1>>::value));
  EXPECT_TRUE ((IsRelocatable<pair<T1, T2>>::value));
  EXPECT_TRUE  (IsRelocatable<set<F1>>::value);
}

TEST(Traits, original) {
  EXPECT_TRUE(IsRelocatable<T1>::value);
  EXPECT_TRUE(IsRelocatable<T2>::value);
}

TEST(Traits, typedefd) {
  EXPECT_TRUE (IsRelocatable<T3>::value);
  EXPECT_TRUE (IsRelocatable<T5>::value);
  EXPECT_FALSE(IsRelocatable<F2>::value);
  EXPECT_FALSE(IsRelocatable<F3>::value);
}

TEST(Traits, unset) {
  EXPECT_FALSE(IsRelocatable<F1>::value);
  EXPECT_FALSE(IsRelocatable<F4>::value);
}

TEST(Traits, bitprop) {
  EXPECT_TRUE(IsTriviallyCopyable<T4>::value);
  EXPECT_TRUE(IsRelocatable<T4>::value);
}

TEST(Traits, bitAndInit) {
  EXPECT_TRUE (IsTriviallyCopyable<int>::value);
  EXPECT_FALSE(IsTriviallyCopyable<vector<int>>::value);
  EXPECT_TRUE (IsZeroInitializable<int>::value);
  EXPECT_FALSE(IsZeroInitializable<vector<int>>::value);
}

int main(int argc, char ** argv) {
  testing::InitGoogleTest(&argc, argv);
  google::ParseCommandLineFlags(&argc, &argv, true);
  if (FLAGS_benchmark) {
    folly::runBenchmarks();
  }
  return RUN_ALL_TESTS();
}

