// --------------------------------------------------------------------------
//   SmartPeak -- Fast and Accurate CE-, GC- and LC-MS(/MS) Data Processing
// --------------------------------------------------------------------------
// Copyright The SmartPeak Team -- Novo Nordisk Foundation 
// Center for Biosustainability, Technical University of Denmark 2018-2021.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL ANY OF THE AUTHORS OR THE CONTRIBUTING
// INSTITUTIONS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
// EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
// PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
// OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
// WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
// OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
// ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
// --------------------------------------------------------------------------
// $Maintainer: Douglas McCloskey, Ahmed Khalil $
// $Authors: Douglas McCloskey $
// --------------------------------------------------------------------------

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <SmartPeak/core/CastValue.h>
#undef main
using namespace SmartPeak;
using namespace std;

TEST(CastValue, castValue_constructors)
{
  CastValue c_uninitialized;
  EXPECT_TRUE(c_uninitialized.getTag() == CastValue::Type::UNINITIALIZED);
  EXPECT_FALSE(c_uninitialized.b_);

  CastValue c_string(std::string("test"));
  EXPECT_TRUE(c_string.getTag() == CastValue::Type::STRING);
  EXPECT_STREQ(c_string.s_.c_str(), "test");

  CastValue c_charptr("test");
  EXPECT_TRUE(c_charptr.getTag() == CastValue::Type::STRING);
  EXPECT_STREQ(c_charptr.s_.c_str(), "test");

  CastValue c_float(1.1f);
  EXPECT_TRUE(c_float.getTag() == CastValue::Type::FLOAT);
  EXPECT_NEAR(c_float.f_, 1.1f, 1e-3f);

  CastValue c_int(42);
  EXPECT_TRUE(c_int.getTag() == CastValue::Type::INT);
  EXPECT_EQ(c_int.i_, 42);

  CastValue c_longint(42l);
  EXPECT_TRUE(c_longint.getTag() == CastValue::Type::LONG_INT);
  EXPECT_EQ(c_longint.li_, 42l);

  CastValue c_bool(true);
  EXPECT_TRUE(c_bool.getTag() == CastValue::Type::BOOL);
  EXPECT_TRUE(c_bool.b_);

  std::vector<bool> bool_list = { true, false, true };
  CastValue c_bool_list(bool_list);
  EXPECT_TRUE(c_bool_list.getTag() == CastValue::Type::BOOL_LIST);
  EXPECT_THAT((std::vector<bool>)c_bool_list.bl_, ::testing::ContainerEq(bool_list));

  std::vector<float> float_list = { 1.1f, 2.2f, 3.3f };
  CastValue c_float_list(float_list);
  EXPECT_TRUE(c_float_list.getTag() == CastValue::Type::FLOAT_LIST);
  EXPECT_THAT((std::vector<float>)c_float_list.fl_, ::testing::ContainerEq(float_list));

  std::vector<int> int_list = { 1, 2, 3 };
  CastValue c_int_list(int_list);
  EXPECT_TRUE(c_int_list.getTag() == CastValue::Type::INT_LIST);
  EXPECT_THAT((std::vector<int>)c_int_list.il_, ::testing::ContainerEq(int_list));

  std::vector<std::string> string_list = { "one", "two", "three" };
  CastValue c_string_list(string_list);
  EXPECT_TRUE(c_string_list.getTag() == CastValue::Type::STRING_LIST);
  EXPECT_THAT((std::vector<std::string>)c_string_list.sl_, ::testing::ContainerEq(string_list));
}

TEST(CastValue, castValue_constructor_copyConstructor)
{
  CastValue c;
  EXPECT_TRUE(c.getTag() == CastValue::Type::UNINITIALIZED);
  EXPECT_FALSE(c.b_);

  c = 7;

  EXPECT_TRUE(c.getTag() == CastValue::Type::INT);
  EXPECT_EQ(c.i_, 7);

  CastValue c2 = c;
  EXPECT_TRUE(c2.getTag() == CastValue::Type::INT);
  EXPECT_EQ(c2.i_, 7);

  CastValue c3(c);
  EXPECT_TRUE(c3.getTag() == CastValue::Type::INT);
  EXPECT_EQ(c3.i_, 7);
}

TEST(CastValue, castValue_assignment_operator)
{
  CastValue c;

  // From other cast values
  CastValue c_uninitialized;
  c = c_uninitialized;
  EXPECT_TRUE(c.getTag() == CastValue::Type::UNINITIALIZED);
  EXPECT_FALSE(c.b_);

  CastValue c_string(std::string("test"));
  c = c_string;
  EXPECT_TRUE(c.getTag() == CastValue::Type::STRING);
  EXPECT_STREQ(c.s_.c_str(), "test");

  CastValue c_charptr("test");
  c = c_charptr;
  EXPECT_TRUE(c.getTag() == CastValue::Type::STRING);
  EXPECT_STREQ(c.s_.c_str(), "test");

  CastValue c_float(1.1f);
  c = c_float;
  EXPECT_TRUE(c.getTag() == CastValue::Type::FLOAT);
  EXPECT_NEAR(c.f_, 1.1f, 1e-3f);

  CastValue c_int(42);
  c = c_int;
  EXPECT_TRUE(c.getTag() == CastValue::Type::INT);
  EXPECT_EQ(c.i_, 42);

  CastValue c_longint(42l);
  c = c_longint;
  EXPECT_TRUE(c.getTag() == CastValue::Type::LONG_INT);
  EXPECT_EQ(c.li_, 42l);

  CastValue c_bool(true);
  c = c_bool;
  EXPECT_TRUE(c.getTag() == CastValue::Type::BOOL);
  EXPECT_TRUE(c.b_);

  std::vector<bool> bool_list = { true, false, true };
  CastValue c_bool_list(bool_list);
  c = c_bool_list;
  EXPECT_TRUE(c.getTag() == CastValue::Type::BOOL_LIST);
  EXPECT_THAT((std::vector<bool>)c.bl_, ::testing::ContainerEq(bool_list));

  std::vector<float> float_list = { 1.1f, 2.2f, 3.3f };
  CastValue c_float_list(float_list);
  c = c_float_list;
  EXPECT_TRUE(c.getTag() == CastValue::Type::FLOAT_LIST);
  EXPECT_THAT((std::vector<float>)c.fl_, ::testing::ContainerEq(float_list));

  std::vector<int> int_list = { 1, 2, 3 };
  CastValue c_int_list(int_list);
  c = c_int_list;
  EXPECT_TRUE(c.getTag() == CastValue::Type::INT_LIST);
  EXPECT_THAT((std::vector<int>)c.il_, ::testing::ContainerEq(int_list));

  std::vector<std::string> string_list = { "one", "two", "three" };
  CastValue c_string_list(string_list);
  c = c_string_list;
  EXPECT_TRUE(c.getTag() == CastValue::Type::STRING_LIST);
  EXPECT_THAT((std::vector<std::string>)c.sl_, ::testing::ContainerEq(string_list));

  // self assignment;
  c = c;
  EXPECT_TRUE(c.getTag() == CastValue::Type::STRING_LIST);
  EXPECT_THAT((std::vector<std::string>)c.sl_, ::testing::ContainerEq(string_list));

  // From flat values
  c = std::string("test");
  EXPECT_TRUE(c.getTag() == CastValue::Type::STRING);
  EXPECT_STREQ(c.s_.c_str(), "test");

  c = "test";
  EXPECT_TRUE(c.getTag() == CastValue::Type::STRING);
  EXPECT_STREQ(c.s_.c_str(), "test");

  c = 1.1f;
  EXPECT_TRUE(c.getTag() == CastValue::Type::FLOAT);
  EXPECT_NEAR(c.f_, 1.1f, 1e-3f);

  c = 42;
  EXPECT_TRUE(c.getTag() == CastValue::Type::INT);
  EXPECT_EQ(c.i_, 42);

  c = 42l;
  EXPECT_TRUE(c.getTag() == CastValue::Type::LONG_INT);
  EXPECT_EQ(c.li_, 42l);

  c = true;
  EXPECT_TRUE(c.getTag() == CastValue::Type::BOOL);
  EXPECT_TRUE(c.b_);

  c = bool_list;
  EXPECT_TRUE(c.getTag() == CastValue::Type::BOOL_LIST);
  EXPECT_THAT((std::vector<bool>)c.bl_, ::testing::ContainerEq(bool_list));

  c = float_list;
  EXPECT_TRUE(c.getTag() == CastValue::Type::FLOAT_LIST);
  EXPECT_THAT((std::vector<float>)c.fl_, ::testing::ContainerEq(float_list));

  c = int_list;
  EXPECT_TRUE(c.getTag() == CastValue::Type::INT_LIST);
  EXPECT_THAT((std::vector<int>)c.il_, ::testing::ContainerEq(int_list));

  c = string_list;
  EXPECT_TRUE(c.getTag() == CastValue::Type::STRING_LIST);
  EXPECT_THAT((std::vector<std::string>)c.sl_, ::testing::ContainerEq(string_list));
}

TEST(CastValue, castValue_comparison_operator)
{
  // From other cast values
  CastValue c_uninitialized1;
  CastValue c_uninitialized2;
  EXPECT_TRUE(c_uninitialized1 == c_uninitialized2);
  EXPECT_TRUE(c_uninitialized1 >= c_uninitialized2);
  EXPECT_FALSE(c_uninitialized1 >  c_uninitialized2);
  EXPECT_TRUE(c_uninitialized1 <= c_uninitialized2);
  EXPECT_FALSE(c_uninitialized1 <  c_uninitialized2);

  CastValue c_string1(std::string("test1"));
  CastValue c_string2(std::string("test2"));
  EXPECT_FALSE(c_string1 == c_string2);
  EXPECT_FALSE(c_string1 >= c_string2);
  EXPECT_FALSE(c_string1 > c_string2);
  EXPECT_TRUE(c_string1 <= c_string2);
  EXPECT_TRUE(c_string1 < c_string2);
  CastValue c_string3(std::string("test1"));
  EXPECT_TRUE(c_string1 == c_string3);
  CastValue c_string4(std::string("TEST1"));
  EXPECT_FALSE(c_string1 == c_string4);

  CastValue c_float1(1.1f);
  CastValue c_float2(2.2f);
  EXPECT_FALSE(c_float1 == c_float2);
  EXPECT_FALSE(c_float1 >= c_float2);
  EXPECT_FALSE(c_float1 > c_float2);
  EXPECT_TRUE(c_float1 <= c_float2);
  EXPECT_TRUE(c_float1 < c_float2);
  CastValue c_float3(1.1f);
  EXPECT_TRUE(c_float1 == c_float3);
  EXPECT_TRUE(c_float1 >= c_float3);
  EXPECT_FALSE(c_float1 > c_float3);
  EXPECT_TRUE(c_float1 <= c_float3);
  EXPECT_FALSE(c_float1 < c_float3);

  CastValue c_int1(42);
  CastValue c_int2(43);
  EXPECT_FALSE(c_int1 == c_int2);
  EXPECT_FALSE(c_int1 >= c_int2);
  EXPECT_FALSE(c_int1 > c_int2);
  EXPECT_TRUE(c_int1 <= c_int2);
  EXPECT_TRUE(c_int1 < c_int2);
  CastValue c_int3(42);
  EXPECT_TRUE(c_int1 == c_int3);
  EXPECT_TRUE(c_int1 >= c_int3);
  EXPECT_FALSE(c_int1 > c_int3);
  EXPECT_TRUE(c_int1 <= c_int3);
  EXPECT_FALSE(c_int1 < c_int3);

  CastValue c_longint1(42l);
  CastValue c_longint2(43l);
  EXPECT_FALSE(c_longint1 == c_longint2);
  EXPECT_FALSE(c_longint1 >= c_longint2);
  EXPECT_FALSE(c_longint1 > c_longint2);
  EXPECT_TRUE(c_longint1 <= c_longint2);
  EXPECT_TRUE(c_longint1 < c_longint2);
  CastValue c_longint3(42l);
  EXPECT_TRUE(c_longint1 == c_longint3);
  EXPECT_TRUE(c_longint1 >= c_longint3);
  EXPECT_FALSE(c_longint1 > c_longint3);
  EXPECT_TRUE(c_longint1 <= c_longint3);
  EXPECT_FALSE(c_longint1 < c_longint3);

  CastValue c_bool1(true);
  CastValue c_bool2(false);
  EXPECT_FALSE(c_bool1 == c_bool2);
  EXPECT_TRUE(c_bool1 >= c_bool2);
  EXPECT_TRUE(c_bool1 > c_bool2);
  EXPECT_FALSE(c_bool1 <= c_bool2);
  EXPECT_FALSE(c_bool1 < c_bool2);
  CastValue c_bool3(true);
  EXPECT_TRUE(c_bool1 == c_bool3);
  EXPECT_TRUE(c_bool1 >= c_bool3);
  EXPECT_FALSE(c_bool1 > c_bool3);
  EXPECT_TRUE(c_bool1 <= c_bool3);
  EXPECT_FALSE(c_bool1 < c_bool3);

  std::vector<bool> bool_list1 = { true, false, true };
  CastValue c_bool_list1(bool_list1);
  std::vector<bool> bool_list2 = { false, true, false };
  CastValue c_bool_list2(bool_list2);
  EXPECT_FALSE(c_bool_list1 == c_bool_list2);
  std::vector<bool> bool_list3 = { true, false, true };
  CastValue c_bool_list3(bool_list3);
  EXPECT_TRUE(c_bool_list1 == c_bool_list3);
  // not supported always return true
  EXPECT_TRUE(c_bool_list1 >= c_bool_list2);
  EXPECT_TRUE(c_bool_list1 > c_bool_list2);
  EXPECT_TRUE(c_bool_list1 <= c_bool_list2);
  EXPECT_TRUE(c_bool_list1 < c_bool_list2);

  std::vector<float> float_list1 = { 1.1f, 2.2f, 3.3f };
  CastValue c_float_list1(float_list1);
  std::vector<float> float_list2 = { 4.4f, 5.5f, 6.6f };
  CastValue c_float_list2(float_list2);
  EXPECT_FALSE(c_float_list1 == c_float_list2);
  std::vector<float> float_list3 = { 1.1f, 2.2f, 3.3f };
  CastValue c_float_list3(float_list3);
  EXPECT_TRUE(c_float_list1 == c_float_list3);
  // not supported always return true
  EXPECT_TRUE(c_float_list1 >= c_float_list2);
  EXPECT_TRUE(c_float_list1 > c_float_list2);
  EXPECT_TRUE(c_float_list1 <= c_float_list2);
  EXPECT_TRUE(c_float_list1 < c_float_list2);

  std::vector<int> int_list1 = { 1, 2, 3 };
  CastValue c_int_list1(int_list1);
  std::vector<int> int_list2 = { 4, 5, 6 };
  CastValue c_int_list2(int_list2);
  EXPECT_FALSE(c_int_list1 == c_int_list2);
  std::vector<int> int_list3 = { 1, 2, 3 };
  CastValue c_int_list3(int_list3);
  EXPECT_TRUE(c_int_list1 == c_int_list3);
  // not supported always return true
  EXPECT_TRUE(c_int_list1 >= c_int_list2);
  EXPECT_TRUE(c_int_list1 > c_int_list2);
  EXPECT_TRUE(c_int_list1 <= c_int_list2);
  EXPECT_TRUE(c_int_list1 < c_int_list2);

  std::vector<std::string> string_list1 = { "one", "two", "three" };
  CastValue c_string_list1(string_list1);
  std::vector<std::string> string_list2 = { "four", "five", "six" };
  CastValue c_string_list2(string_list2);
  EXPECT_FALSE(c_string_list1 == c_string_list2);
  std::vector<std::string> string_list3 = { "one", "two", "three" };
  CastValue c_string_list3(string_list3);
  EXPECT_TRUE(c_string_list1 == c_string_list3);
  // not supported always return true
  EXPECT_TRUE(c_string_list1 >= c_string_list2);
  EXPECT_TRUE(c_string_list1 > c_string_list2);
  EXPECT_TRUE(c_string_list1 <= c_string_list2);
  EXPECT_TRUE(c_string_list1 < c_string_list2);

  // Comparing with flat values
  EXPECT_FALSE(c_string1 == std::string("test2"));
  EXPECT_FALSE(c_string1 >= std::string("test2"));
  EXPECT_FALSE(c_string1 > std::string("test2"));
  EXPECT_TRUE(c_string1 <= std::string("test2"));
  EXPECT_TRUE(c_string1 < std::string("test2"));
  EXPECT_TRUE(c_string1 == std::string("test1"));
  EXPECT_FALSE(c_string1 == std::string("test4"));

  EXPECT_FALSE(c_float1 == 2.2f);
  EXPECT_FALSE(c_float1 >= 2.2f);
  EXPECT_FALSE(c_float1 > 2.2f);
  EXPECT_TRUE(c_float1 <= 2.2f);
  EXPECT_TRUE(c_float1 < 2.2f);

  EXPECT_FALSE(c_int1 == 43);
  EXPECT_FALSE(c_int1 >= 43);
  EXPECT_FALSE(c_int1 > 43);
  EXPECT_TRUE(c_int1 <= 43);
  EXPECT_TRUE(c_int1 < 43);
  EXPECT_TRUE(c_int1 == 42);
  EXPECT_TRUE(c_int1 >= 42);
  EXPECT_FALSE(c_int1 > 42);
  EXPECT_TRUE(c_int1 <= 42);
  EXPECT_FALSE(c_int1 < 42);

  EXPECT_FALSE(c_longint1 == 43l);
  EXPECT_FALSE(c_longint1 >= 43l);
  EXPECT_FALSE(c_longint1 > 43l);
  EXPECT_TRUE(c_longint1 <= 43l);
  EXPECT_TRUE(c_longint1 < 43l);
  EXPECT_TRUE(c_longint1 == 42l);
  EXPECT_TRUE(c_longint1 >= 42l);
  EXPECT_FALSE(c_longint1 > 42l);
  EXPECT_TRUE(c_longint1 <= 42l);
  EXPECT_FALSE(c_longint1 < 42l);

  EXPECT_FALSE(c_bool1 == false);
  EXPECT_TRUE(c_bool1 >= false);
  EXPECT_TRUE(c_bool1 > false);
  EXPECT_FALSE(c_bool1 <= false);
  EXPECT_FALSE(c_bool1 < false);
  EXPECT_TRUE(c_bool1 == true);
  EXPECT_TRUE(c_bool1 >= true);
  EXPECT_FALSE(c_bool1 > true);
  EXPECT_TRUE(c_bool1 <= true);
  EXPECT_FALSE(c_bool1 < true);

  EXPECT_FALSE(c_bool_list1 == bool_list2);
  // not supported always return true
  EXPECT_TRUE(c_bool_list1 >= bool_list2);
  EXPECT_TRUE(c_bool_list1 > bool_list2);
  EXPECT_TRUE(c_bool_list1 <= bool_list2);
  EXPECT_TRUE(c_bool_list1 < bool_list2);

  EXPECT_FALSE(c_float_list1 == float_list2);
  // not supported always return true
  EXPECT_TRUE(c_float_list1 >= float_list2);
  EXPECT_TRUE(c_float_list1 > float_list2);
  EXPECT_TRUE(c_float_list1 <= float_list2);
  EXPECT_TRUE(c_float_list1 < float_list2);

  EXPECT_FALSE(c_int_list1 == int_list2);
  // not supported always return true
  EXPECT_TRUE(c_int_list1 >= int_list2);
  EXPECT_TRUE(c_int_list1 > int_list2);
  EXPECT_TRUE(c_int_list1 <= int_list2);
  EXPECT_TRUE(c_int_list1 < int_list2);

  EXPECT_FALSE(c_string_list1 == string_list2);
  // not supported always return true
  EXPECT_TRUE(c_string_list1 >= string_list2);
  EXPECT_TRUE(c_string_list1 > string_list2);
  EXPECT_TRUE(c_string_list1 <= string_list2);
  EXPECT_TRUE(c_string_list1 < string_list2);

  // Comparing different types is not supported
  EXPECT_FALSE(c_string1 == c_float1);
  EXPECT_FALSE(c_float1 == c_int1);
  EXPECT_FALSE(c_int1 == c_longint1);
  EXPECT_FALSE(c_longint1 == c_bool1);
  EXPECT_FALSE(c_bool1 == c_bool_list1);
  EXPECT_FALSE(c_bool_list1 == c_float_list1);
  EXPECT_FALSE(c_float_list1 == c_int_list1);
  EXPECT_FALSE(c_int_list1 == c_string_list1);

}

TEST(CastValue, castValue_comparison_methods)
{
  // From other cast values
  CastValue c_uninitialized1;
  CastValue c_uninitialized2;
  EXPECT_TRUE(c_uninitialized1.is_equal_to(c_uninitialized2));
  EXPECT_FALSE(c_uninitialized1.is_greater_than(c_uninitialized2));
  EXPECT_FALSE(c_uninitialized1.is_less_than(c_uninitialized2));

  CastValue c_string1(std::string("test1"));
  CastValue c_string2(std::string("test2"));
  EXPECT_FALSE(c_string1.is_equal_to(c_string2));
  EXPECT_FALSE(c_string1.is_greater_than(c_string2));
  EXPECT_TRUE(c_string1.is_less_than(c_string2));
  CastValue c_string3(std::string("test1"));
  EXPECT_TRUE(c_string1.is_equal_to(c_string3));
  CastValue c_string4(std::string("TEST1"));
  EXPECT_FALSE(c_string1.is_equal_to(c_string4));
  
  // non case sensitive
  EXPECT_TRUE(c_string1.is_equal_to(c_string4, false));

  CastValue c_float1(1.1f);
  CastValue c_float2(2.2f);
  EXPECT_FALSE(c_float1.is_equal_to(c_float2));
  EXPECT_FALSE(c_float1.is_greater_than(c_float2));
  EXPECT_TRUE(c_float1.is_less_than(c_float2));
  CastValue c_float3(1.1f);
  EXPECT_TRUE(c_float1.is_equal_to(c_float3));
  EXPECT_FALSE(c_float1.is_greater_than(c_float3));
  EXPECT_FALSE(c_float1.is_less_than(c_float3));

  CastValue c_int1(42);
  CastValue c_int2(43);
  EXPECT_FALSE(c_int1.is_equal_to(c_int2));
  EXPECT_FALSE(c_int1.is_greater_than(c_int2));
  EXPECT_TRUE(c_int1.is_less_than(c_int2));
  CastValue c_int3(42);
  EXPECT_TRUE(c_int1.is_equal_to(c_int3));
  EXPECT_FALSE(c_int1.is_greater_than(c_int3));
  EXPECT_FALSE(c_int1.is_less_than(c_int3));

  CastValue c_longint1(42l);
  CastValue c_longint2(43l);
  EXPECT_FALSE(c_longint1.is_equal_to(c_longint2));
  EXPECT_FALSE(c_longint1.is_greater_than(c_longint2));
  EXPECT_TRUE(c_longint1.is_less_than(c_longint2));
  CastValue c_longint3(42l);
  EXPECT_TRUE(c_longint1.is_equal_to(c_longint3));
  EXPECT_FALSE(c_longint1.is_greater_than(c_longint3));
  EXPECT_FALSE(c_longint1.is_less_than(c_longint3));

  CastValue c_bool1(true);
  CastValue c_bool2(false);
  EXPECT_FALSE(c_bool1.is_equal_to(c_bool2));
  EXPECT_TRUE(c_bool1.is_greater_than(c_bool2));
  EXPECT_FALSE(c_bool1.is_less_than(c_bool2));
  CastValue c_bool3(true);
  EXPECT_TRUE(c_bool1.is_equal_to(c_bool3));
  EXPECT_FALSE(c_bool1.is_greater_than(c_bool3));
  EXPECT_FALSE(c_bool1.is_less_than(c_bool3));

  std::vector<bool> bool_list1 = { true, false, true };
  CastValue c_bool_list1(bool_list1);
  std::vector<bool> bool_list2 = { false, true, false };
  CastValue c_bool_list2(bool_list2);
  EXPECT_FALSE(c_bool_list1.is_equal_to(c_bool_list2));
  std::vector<bool> bool_list3 = { true, false, true };
  CastValue c_bool_list3(bool_list3);
  EXPECT_TRUE(c_bool_list1.is_equal_to(c_bool_list3));
  // not supported always return true
  EXPECT_TRUE(c_bool_list1.is_greater_than(c_bool_list2));
  EXPECT_TRUE(c_bool_list1.is_less_than(c_bool_list2));

  std::vector<float> float_list1 = { 1.1f, 2.2f, 3.3f };
  CastValue c_float_list1(float_list1);
  std::vector<float> float_list2 = { 4.4f, 5.5f, 6.6f };
  CastValue c_float_list2(float_list2);
  EXPECT_FALSE(c_float_list1.is_equal_to(c_float_list2));
  std::vector<float> float_list3 = { 1.1f, 2.2f, 3.3f };
  CastValue c_float_list3(float_list3);
  EXPECT_TRUE(c_float_list1.is_equal_to(c_float_list3));
  // not supported always return true
  EXPECT_TRUE(c_float_list1.is_greater_than(c_float_list2));
  EXPECT_TRUE(c_float_list1.is_less_than(c_float_list2));

  std::vector<int> int_list1 = { 1, 2, 3 };
  CastValue c_int_list1(int_list1);
  std::vector<int> int_list2 = { 4, 5, 6 };
  CastValue c_int_list2(int_list2);
  EXPECT_FALSE(c_int_list1.is_equal_to(c_int_list2));
  std::vector<int> int_list3 = { 1, 2, 3 };
  CastValue c_int_list3(int_list3);
  EXPECT_TRUE(c_int_list1.is_equal_to(c_int_list3));
  // not supported always return true
  EXPECT_TRUE(c_int_list1.is_greater_than(c_int_list2));
  EXPECT_TRUE(c_int_list1.is_less_than(c_int_list2));

  std::vector<std::string> string_list1 = { "one", "two", "three" };
  CastValue c_string_list1(string_list1);
  std::vector<std::string> string_list2 = { "four", "five", "six" };
  CastValue c_string_list2(string_list2);
  EXPECT_FALSE(c_string_list1.is_equal_to(c_string_list2));
  std::vector<std::string> string_list3 = { "one", "two", "three" };
  CastValue c_string_list3(string_list3);
  EXPECT_TRUE(c_string_list1.is_equal_to(c_string_list3));
  std::vector<std::string> string_list4 = { "One", "tWo", "THREE" };
  CastValue c_string_list4(string_list4);
  EXPECT_FALSE(c_string_list1.is_equal_to(c_string_list4, true));
  EXPECT_TRUE(c_string_list1.is_equal_to(c_string_list4, false));
  // not supported always return true
  EXPECT_TRUE(c_string_list1.is_greater_than(c_string_list2));
  EXPECT_TRUE(c_string_list1.is_less_than(c_string_list2));

  // Comparing different types is not supported
  EXPECT_FALSE(c_string1.is_equal_to(c_float1));
  EXPECT_FALSE(c_float1.is_equal_to(c_int1));
  EXPECT_FALSE(c_int1.is_equal_to(c_longint1));
  EXPECT_FALSE(c_longint1.is_equal_to(c_bool1));
  EXPECT_FALSE(c_bool1.is_equal_to(c_bool_list1));
  EXPECT_FALSE(c_bool_list1.is_equal_to(c_float_list1));
  EXPECT_FALSE(c_float_list1.is_equal_to(c_int_list1));
  EXPECT_FALSE(c_int_list1.is_equal_to(c_string_list1));
}

TEST(CastValue, castValue_to_string)
{
  CastValue c_uninitialized;
  EXPECT_STREQ(std::string(c_uninitialized).c_str(), "false");

  CastValue c_string(std::string("test"));
  EXPECT_STREQ(std::string(c_string).c_str(), "test");

  CastValue c_charptr("test");
  EXPECT_STREQ(std::string(c_charptr).c_str(), "test");

  CastValue c_float(1.1f);
  EXPECT_STREQ(std::string(c_float).c_str(), "1.1");

  CastValue c_int(42);
  EXPECT_STREQ(std::string(c_int).c_str(), "42");

  CastValue c_longint(42l);
  EXPECT_STREQ(std::string(c_longint).c_str(), "42");

  CastValue c_bool(true);
  EXPECT_STREQ(std::string(c_bool).c_str(), "true");

  std::vector<bool> bool_list = { true, false, true };
  CastValue c_bool_list(bool_list);
  EXPECT_STREQ(std::string(c_bool_list).c_str(), "[true,false,true]");

  std::vector<float> float_list = { 1.1f, 2.2f, 3.3f };
  CastValue c_float_list(float_list);
  EXPECT_STREQ(std::string(c_float_list).c_str(), "[1.1,2.2,3.3]");

  std::vector<int> int_list = { 1, 2, 3 };
  CastValue c_int_list(int_list);
  EXPECT_STREQ(std::string(c_int_list).c_str(), "[1,2,3]");

  std::vector<std::string> string_list = { "one", "two", "three" };
  CastValue c_string_list(string_list);
  EXPECT_STREQ(std::string(c_string_list).c_str(), "['one','two','three']");
}

TEST(CastValue, castValue_set_tag_and_data)
{
  CastValue c;

  CastValue c_uninitialized;
  c.setTagAndData(CastValue::Type::UNINITIALIZED, false);
  EXPECT_TRUE(c.getTag() == CastValue::Type::UNINITIALIZED);
  EXPECT_FALSE(c.b_);

  c.setTagAndData(CastValue::Type::STRING, std::string("test"));
  EXPECT_TRUE(c.getTag() == CastValue::Type::STRING);
  EXPECT_STREQ(c.s_.c_str(), "test");

  c.setTagAndData(CastValue::Type::STRING, "test");
  EXPECT_TRUE(c.getTag() == CastValue::Type::STRING);
  EXPECT_STREQ(c.s_.c_str(), "test");

  c.setTagAndData(CastValue::Type::FLOAT, 1.1f);
  EXPECT_TRUE(c.getTag() == CastValue::Type::FLOAT);
  EXPECT_NEAR(c.f_, 1.1f, 1e-3f);

  c.setTagAndData(CastValue::Type::INT, 42);
  EXPECT_TRUE(c.getTag() == CastValue::Type::INT);
  EXPECT_EQ(c.i_, 42);

  c.setTagAndData(CastValue::Type::LONG_INT, 42l);
  EXPECT_TRUE(c.getTag() == CastValue::Type::LONG_INT);
  EXPECT_EQ(c.li_, 42l);

  c.setTagAndData(CastValue::Type::BOOL, true);
  EXPECT_TRUE(c.getTag() == CastValue::Type::BOOL);
  EXPECT_TRUE(c.b_);

  std::vector<bool> bool_list = { true, false, true };
  c.setTagAndData(CastValue::Type::BOOL_LIST, bool_list);
  EXPECT_TRUE(c.getTag() == CastValue::Type::BOOL_LIST);
  EXPECT_THAT((std::vector<bool>)c.bl_, ::testing::ContainerEq(bool_list));

  std::vector<float> float_list = { 1.1f, 2.2f, 3.3f };
  c.setTagAndData(CastValue::Type::FLOAT_LIST, float_list);
  EXPECT_TRUE(c.getTag() == CastValue::Type::FLOAT_LIST);
  EXPECT_THAT((std::vector<float>)c.fl_, ::testing::ContainerEq(float_list));

  std::vector<int> int_list = { 1, 2, 3 };
  c.setTagAndData(CastValue::Type::INT_LIST, int_list);
  EXPECT_TRUE(c.getTag() == CastValue::Type::INT_LIST);
  EXPECT_THAT((std::vector<int>)c.il_, ::testing::ContainerEq(int_list));

  std::vector<std::string> string_list = { "one", "two", "three" };
  c.setTagAndData(CastValue::Type::STRING_LIST, string_list);
  EXPECT_TRUE(c.getTag() == CastValue::Type::STRING_LIST);
  EXPECT_THAT((std::vector<std::string>)c.sl_, ::testing::ContainerEq(string_list));

  // unknown type
  c.setTagAndData(CastValue::Type::UNKNOWN, "test");
  EXPECT_TRUE(c.getTag() == CastValue::Type::UNKNOWN);
  EXPECT_STREQ(c.s_.c_str(), "test");

}

TEST(CastValue, castValue_operator_extraction)
{
  std::ostringstream oss;

  CastValue c_uninitialized;
  oss << c_uninitialized;
  EXPECT_STREQ(oss.str().c_str(), "false");
  oss.str("");
  oss.clear();

  CastValue c_string(std::string("test"));
  oss << c_string;
  EXPECT_STREQ(oss.str().c_str(), "test");
  oss.str("");
  oss.clear();

  CastValue c_charptr("test");
  oss << c_charptr;
  EXPECT_STREQ(oss.str().c_str(), "test");
  oss.str("");
  oss.clear();

  CastValue c_float(1.1f);
  oss << c_float;
  EXPECT_STREQ(oss.str().c_str(), "1.1");
  oss.str("");
  oss.clear();

  CastValue c_int(42);
  oss << c_int;
  EXPECT_STREQ(oss.str().c_str(), "42");
  oss.str("");
  oss.clear();

  CastValue c_longint(42l);
  oss << c_longint;
  EXPECT_STREQ(oss.str().c_str(), "42");
  oss.str("");
  oss.clear();

  CastValue c_bool(true);
  oss << c_bool;
  EXPECT_STREQ(oss.str().c_str(), "true");
  oss.str("");
  oss.clear();

  std::vector<bool> bool_list = { true, false, true };
  CastValue c_bool_list(bool_list);
  oss << c_bool_list;
  EXPECT_STREQ(oss.str().c_str(), "[true,false,true]");
  oss.str("");
  oss.clear();

  std::vector<float> float_list = { 1.1f, 2.2f, 3.3f };
  CastValue c_float_list(float_list);
  oss << c_float_list;
  EXPECT_STREQ(oss.str().c_str(), "[1.1,2.2,3.3]");
  oss.str("");
  oss.clear();

  std::vector<int> int_list = { 1, 2, 3 };
  CastValue c_int_list(int_list);
  oss << c_int_list;
  EXPECT_STREQ(oss.str().c_str(), "[1,2,3]");
  oss.str("");
  oss.clear();

  std::vector<std::string> string_list = { "one", "two", "three" };
  CastValue c_string_list(string_list);
  oss << c_string_list;
  EXPECT_STREQ(oss.str().c_str(), "['one','two','three']");
  oss.str("");
  oss.clear();
}
