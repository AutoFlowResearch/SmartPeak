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
// $Maintainer: Douglas McCloskey $
// $Authors: Douglas McCloskey $
// --------------------------------------------------------------------------

#define BOOST_TEST_MODULE CastValue test suite
#include <boost/test/included/unit_test.hpp>
#include <SmartPeak/core/CastValue.h>

using namespace SmartPeak;
using namespace std;

BOOST_AUTO_TEST_SUITE(castvalue)

BOOST_AUTO_TEST_CASE(castValue_constructors)
{
  CastValue c_uninitialized;
  BOOST_CHECK_EQUAL(c_uninitialized.getTag() == CastValue::Type::UNINITIALIZED, true);
  BOOST_CHECK_EQUAL(c_uninitialized.b_, false);

  CastValue c_string(std::string("test"));
  BOOST_CHECK_EQUAL(c_string.getTag() == CastValue::Type::STRING, true);
  BOOST_CHECK_EQUAL(c_string.s_, "test");

  CastValue c_charptr("test");
  BOOST_CHECK_EQUAL(c_charptr.getTag() == CastValue::Type::STRING, true);
  BOOST_CHECK_EQUAL(c_charptr.s_, "test");

  CastValue c_float(1.1f);
  BOOST_CHECK_EQUAL(c_float.getTag() == CastValue::Type::FLOAT, true);
  BOOST_CHECK_CLOSE(c_float.f_, 1.1f, 1e-3f);

  CastValue c_int(42);
  BOOST_CHECK_EQUAL(c_int.getTag() == CastValue::Type::INT, true);
  BOOST_CHECK_EQUAL(c_int.i_, 42);

  CastValue c_longint(42l);
  BOOST_CHECK_EQUAL(c_longint.getTag() == CastValue::Type::LONG_INT, true);
  BOOST_CHECK_EQUAL(c_longint.li_, 42l);

  CastValue c_bool(true);
  BOOST_CHECK_EQUAL(c_bool.getTag() == CastValue::Type::BOOL, true);
  BOOST_CHECK_EQUAL(c_bool.b_, true);

  std::vector<bool> bool_list = { true, false, true };
  CastValue c_bool_list(bool_list);
  BOOST_CHECK_EQUAL(c_bool_list.getTag() == CastValue::Type::BOOL_LIST, true);
  BOOST_CHECK_EQUAL_COLLECTIONS(c_bool_list.bl_.begin(), c_bool_list.bl_.end(), bool_list.begin(), bool_list.end());

  std::vector<float> float_list = { 1.1f, 2.2f, 3.3f };
  CastValue c_float_list(float_list);
  BOOST_CHECK_EQUAL(c_float_list.getTag() == CastValue::Type::FLOAT_LIST, true);
  BOOST_CHECK_EQUAL_COLLECTIONS(c_float_list.fl_.begin(), c_float_list.fl_.end(), float_list.begin(), float_list.end());

  std::vector<int> int_list = { 1, 2, 3 };
  CastValue c_int_list(int_list);
  BOOST_CHECK_EQUAL(c_int_list.getTag() == CastValue::Type::INT_LIST, true);
  BOOST_CHECK_EQUAL_COLLECTIONS(c_int_list.il_.begin(), c_int_list.il_.end(), int_list.begin(), int_list.end());

  std::vector<std::string> string_list = { "one", "two", "three" };
  CastValue c_string_list(string_list);
  BOOST_CHECK_EQUAL(c_string_list.getTag() == CastValue::Type::STRING_LIST, true);
  BOOST_CHECK_EQUAL_COLLECTIONS(c_string_list.sl_.begin(), c_string_list.sl_.end(), string_list.begin(), string_list.end());
}

BOOST_AUTO_TEST_CASE(castValue_constructor_copyConstructor)
{
  CastValue c;
  BOOST_CHECK_EQUAL(c.getTag() == CastValue::Type::UNINITIALIZED, true);
  BOOST_CHECK_EQUAL(c.b_, false);

  c = 7;

  BOOST_CHECK_EQUAL(c.getTag() == CastValue::Type::INT, true);
  BOOST_CHECK_EQUAL(c.i_, 7);

  CastValue c2 = c;
  BOOST_CHECK_EQUAL(c2.getTag() == CastValue::Type::INT, true);
  BOOST_CHECK_EQUAL(c2.i_, 7);

  CastValue c3(c);
  BOOST_CHECK_EQUAL(c3.getTag() == CastValue::Type::INT, true);
  BOOST_CHECK_EQUAL(c3.i_, 7);
}

BOOST_AUTO_TEST_CASE(castValue_assignment_operator)
{
  CastValue c;

  // From other cast values
  CastValue c_uninitialized;
  c = c_uninitialized;
  BOOST_CHECK_EQUAL(c.getTag() == CastValue::Type::UNINITIALIZED, true);
  BOOST_CHECK_EQUAL(c.b_, false);

  CastValue c_string(std::string("test"));
  c = c_string;
  BOOST_CHECK_EQUAL(c.getTag() == CastValue::Type::STRING, true);
  BOOST_CHECK_EQUAL(c.s_, "test");

  CastValue c_charptr("test");
  c = c_charptr;
  BOOST_CHECK_EQUAL(c.getTag() == CastValue::Type::STRING, true);
  BOOST_CHECK_EQUAL(c.s_, "test");

  CastValue c_float(1.1f);
  c = c_float;
  BOOST_CHECK_EQUAL(c.getTag() == CastValue::Type::FLOAT, true);
  BOOST_CHECK_CLOSE(c.f_, 1.1f, 1e-3f);

  CastValue c_int(42);
  c = c_int;
  BOOST_CHECK_EQUAL(c.getTag() == CastValue::Type::INT, true);
  BOOST_CHECK_EQUAL(c.i_, 42);

  CastValue c_longint(42l);
  c = c_longint;
  BOOST_CHECK_EQUAL(c.getTag() == CastValue::Type::LONG_INT, true);
  BOOST_CHECK_EQUAL(c.li_, 42l);

  CastValue c_bool(true);
  c = c_bool;
  BOOST_CHECK_EQUAL(c.getTag() == CastValue::Type::BOOL, true);
  BOOST_CHECK_EQUAL(c.b_, true);

  std::vector<bool> bool_list = { true, false, true };
  CastValue c_bool_list(bool_list);
  c = c_bool_list;
  BOOST_CHECK_EQUAL(c.getTag() == CastValue::Type::BOOL_LIST, true);
  BOOST_CHECK_EQUAL_COLLECTIONS(c.bl_.begin(), c.bl_.end(), bool_list.begin(), bool_list.end());

  std::vector<float> float_list = { 1.1f, 2.2f, 3.3f };
  CastValue c_float_list(float_list);
  c = c_float_list;
  BOOST_CHECK_EQUAL(c.getTag() == CastValue::Type::FLOAT_LIST, true);
  BOOST_CHECK_EQUAL_COLLECTIONS(c.fl_.begin(), c.fl_.end(), float_list.begin(), float_list.end());

  std::vector<int> int_list = { 1, 2, 3 };
  CastValue c_int_list(int_list);
  c = c_int_list;
  BOOST_CHECK_EQUAL(c.getTag() == CastValue::Type::INT_LIST, true);
  BOOST_CHECK_EQUAL_COLLECTIONS(c.il_.begin(), c.il_.end(), int_list.begin(), int_list.end());

  std::vector<std::string> string_list = { "one", "two", "three" };
  CastValue c_string_list(string_list);
  c = c_string_list;
  BOOST_CHECK_EQUAL(c.getTag() == CastValue::Type::STRING_LIST, true);
  BOOST_CHECK_EQUAL_COLLECTIONS(c.sl_.begin(), c.sl_.end(), string_list.begin(), string_list.end());

  // self assignment;
  c = c;
  BOOST_CHECK_EQUAL(c.getTag() == CastValue::Type::STRING_LIST, true);
  BOOST_CHECK_EQUAL_COLLECTIONS(c.sl_.begin(), c.sl_.end(), string_list.begin(), string_list.end());

  // From flat values
  c = std::string("test");
  BOOST_CHECK_EQUAL(c.getTag() == CastValue::Type::STRING, true);
  BOOST_CHECK_EQUAL(c.s_, "test");

  c = "test";
  BOOST_CHECK_EQUAL(c.getTag() == CastValue::Type::STRING, true);
  BOOST_CHECK_EQUAL(c.s_, "test");

  c = 1.1f;
  BOOST_CHECK_EQUAL(c.getTag() == CastValue::Type::FLOAT, true);
  BOOST_CHECK_CLOSE(c.f_, 1.1f, 1e-3f);

  c = 42;
  BOOST_CHECK_EQUAL(c.getTag() == CastValue::Type::INT, true);
  BOOST_CHECK_EQUAL(c.i_, 42);

  c = 42l;
  BOOST_CHECK_EQUAL(c.getTag() == CastValue::Type::LONG_INT, true);
  BOOST_CHECK_EQUAL(c.li_, 42l);

  c = true;
  BOOST_CHECK_EQUAL(c.getTag() == CastValue::Type::BOOL, true);
  BOOST_CHECK_EQUAL(c.b_, true);

  c = bool_list;
  BOOST_CHECK_EQUAL(c.getTag() == CastValue::Type::BOOL_LIST, true);
  BOOST_CHECK_EQUAL_COLLECTIONS(c.bl_.begin(), c.bl_.end(), bool_list.begin(), bool_list.end());

  c = float_list;
  BOOST_CHECK_EQUAL(c.getTag() == CastValue::Type::FLOAT_LIST, true);
  BOOST_CHECK_EQUAL_COLLECTIONS(c.fl_.begin(), c.fl_.end(), float_list.begin(), float_list.end());

  c = int_list;
  BOOST_CHECK_EQUAL(c.getTag() == CastValue::Type::INT_LIST, true);
  BOOST_CHECK_EQUAL_COLLECTIONS(c.il_.begin(), c.il_.end(), int_list.begin(), int_list.end());

  c = string_list;
  BOOST_CHECK_EQUAL(c.getTag() == CastValue::Type::STRING_LIST, true);
  BOOST_CHECK_EQUAL_COLLECTIONS(c.sl_.begin(), c.sl_.end(), string_list.begin(), string_list.end());
}

BOOST_AUTO_TEST_CASE(castValue_comparison_operator)
{
  // From other cast values
  CastValue c_uninitialized1;
  CastValue c_uninitialized2;
  BOOST_CHECK_EQUAL(c_uninitialized1 == c_uninitialized2, true);
  BOOST_CHECK_EQUAL(c_uninitialized1 >= c_uninitialized2, true);
  BOOST_CHECK_EQUAL(c_uninitialized1 >  c_uninitialized2, false);
  BOOST_CHECK_EQUAL(c_uninitialized1 <= c_uninitialized2, true);
  BOOST_CHECK_EQUAL(c_uninitialized1 <  c_uninitialized2, false);

  CastValue c_string1(std::string("test1"));
  CastValue c_string2(std::string("test2"));
  BOOST_CHECK_EQUAL(c_string1 == c_string2, false);
  BOOST_CHECK_EQUAL(c_string1 >= c_string2, false);
  BOOST_CHECK_EQUAL(c_string1 > c_string2, false);
  BOOST_CHECK_EQUAL(c_string1 <= c_string2, true);
  BOOST_CHECK_EQUAL(c_string1 < c_string2, true);
  CastValue c_string3(std::string("test1"));
  BOOST_CHECK_EQUAL(c_string1 == c_string3, true);
  CastValue c_string4(std::string("TEST1"));
  BOOST_CHECK_EQUAL(c_string1 == c_string4, false);

  CastValue c_float1(1.1f);
  CastValue c_float2(2.2f);
  BOOST_CHECK_EQUAL(c_float1 == c_float2, false);
  BOOST_CHECK_EQUAL(c_float1 >= c_float2, false);
  BOOST_CHECK_EQUAL(c_float1 > c_float2, false);
  BOOST_CHECK_EQUAL(c_float1 <= c_float2, true);
  BOOST_CHECK_EQUAL(c_float1 < c_float2, true);
  CastValue c_float3(1.1f);
  BOOST_CHECK_EQUAL(c_float1 == c_float3, true);
  BOOST_CHECK_EQUAL(c_float1 >= c_float3, true);
  BOOST_CHECK_EQUAL(c_float1 > c_float3, false);
  BOOST_CHECK_EQUAL(c_float1 <= c_float3, true);
  BOOST_CHECK_EQUAL(c_float1 < c_float3, false);

  CastValue c_int1(42);
  CastValue c_int2(43);
  BOOST_CHECK_EQUAL(c_int1 == c_int2, false);
  BOOST_CHECK_EQUAL(c_int1 >= c_int2, false);
  BOOST_CHECK_EQUAL(c_int1 > c_int2, false);
  BOOST_CHECK_EQUAL(c_int1 <= c_int2, true);
  BOOST_CHECK_EQUAL(c_int1 < c_int2, true);
  CastValue c_int3(42);
  BOOST_CHECK_EQUAL(c_int1 == c_int3, true);
  BOOST_CHECK_EQUAL(c_int1 >= c_int3, true);
  BOOST_CHECK_EQUAL(c_int1 > c_int3, false);
  BOOST_CHECK_EQUAL(c_int1 <= c_int3, true);
  BOOST_CHECK_EQUAL(c_int1 < c_int3, false);

  CastValue c_longint1(42l);
  CastValue c_longint2(43l);
  BOOST_CHECK_EQUAL(c_longint1 == c_longint2, false);
  BOOST_CHECK_EQUAL(c_longint1 >= c_longint2, false);
  BOOST_CHECK_EQUAL(c_longint1 > c_longint2, false);
  BOOST_CHECK_EQUAL(c_longint1 <= c_longint2, true);
  BOOST_CHECK_EQUAL(c_longint1 < c_longint2, true);
  CastValue c_longint3(42l);
  BOOST_CHECK_EQUAL(c_longint1 == c_longint3, true);
  BOOST_CHECK_EQUAL(c_longint1 >= c_longint3, true);
  BOOST_CHECK_EQUAL(c_longint1 > c_longint3, false);
  BOOST_CHECK_EQUAL(c_longint1 <= c_longint3, true);
  BOOST_CHECK_EQUAL(c_longint1 < c_longint3, false);

  CastValue c_bool1(true);
  CastValue c_bool2(false);
  BOOST_CHECK_EQUAL(c_bool1 == c_bool2, false);
  BOOST_CHECK_EQUAL(c_bool1 >= c_bool2, true);
  BOOST_CHECK_EQUAL(c_bool1 > c_bool2, true);
  BOOST_CHECK_EQUAL(c_bool1 <= c_bool2, false);
  BOOST_CHECK_EQUAL(c_bool1 < c_bool2, false);
  CastValue c_bool3(true);
  BOOST_CHECK_EQUAL(c_bool1 == c_bool3, true);
  BOOST_CHECK_EQUAL(c_bool1 >= c_bool3, true);
  BOOST_CHECK_EQUAL(c_bool1 > c_bool3, false);
  BOOST_CHECK_EQUAL(c_bool1 <= c_bool3, true);
  BOOST_CHECK_EQUAL(c_bool1 < c_bool3, false);

  std::vector<bool> bool_list1 = { true, false, true };
  CastValue c_bool_list1(bool_list1);
  std::vector<bool> bool_list2 = { false, true, false };
  CastValue c_bool_list2(bool_list2);
  // not supported always return true
  BOOST_CHECK_EQUAL(c_bool_list1 == c_bool_list2, true);
  BOOST_CHECK_EQUAL(c_bool_list1 >= c_bool_list2, true);
  BOOST_CHECK_EQUAL(c_bool_list1 > c_bool_list2, true);
  BOOST_CHECK_EQUAL(c_bool_list1 <= c_bool_list2, true);
  BOOST_CHECK_EQUAL(c_bool_list1 < c_bool_list2, true);

  std::vector<float> float_list1 = { 1.1f, 2.2f, 3.3f };
  CastValue c_float_list1(float_list1);
  std::vector<float> float_list2 = { 4.4f, 5.5f, 6.6f };
  CastValue c_float_list2(float_list2);
  // not supported always return true
  BOOST_CHECK_EQUAL(c_float_list1 == c_float_list2, true);
  BOOST_CHECK_EQUAL(c_float_list1 >= c_float_list2, true);
  BOOST_CHECK_EQUAL(c_float_list1 > c_float_list2, true);
  BOOST_CHECK_EQUAL(c_float_list1 <= c_float_list2, true);
  BOOST_CHECK_EQUAL(c_float_list1 < c_float_list2, true);

  std::vector<int> int_list1 = { 1, 2, 3 };
  CastValue c_int_list1(int_list1);
  std::vector<int> int_list2 = { 4, 5, 6 };
  CastValue c_int_list2(int_list2);
  // not supported always return true
  BOOST_CHECK_EQUAL(c_int_list1 == c_int_list2, true);
  BOOST_CHECK_EQUAL(c_int_list1 >= c_int_list2, true);
  BOOST_CHECK_EQUAL(c_int_list1 > c_int_list2, true);
  BOOST_CHECK_EQUAL(c_int_list1 <= c_int_list2, true);
  BOOST_CHECK_EQUAL(c_int_list1 < c_int_list2, true);

  std::vector<std::string> string_list1 = { "one", "two", "three" };
  CastValue c_string_list1(string_list1);
  std::vector<std::string> string_list2 = { "four", "five", "six" };
  CastValue c_string_list2(string_list2);
  // not supported always return true
  BOOST_CHECK_EQUAL(c_string_list1 == c_string_list2, true);
  BOOST_CHECK_EQUAL(c_string_list1 >= c_string_list2, true);
  BOOST_CHECK_EQUAL(c_string_list1 > c_string_list2, true);
  BOOST_CHECK_EQUAL(c_string_list1 <= c_string_list2, true);
  BOOST_CHECK_EQUAL(c_string_list1 < c_string_list2, true);

  // Comparing with flat values
  BOOST_CHECK_EQUAL(c_string1 == std::string("test2"), false);
  BOOST_CHECK_EQUAL(c_string1 >= std::string("test2"), false);
  BOOST_CHECK_EQUAL(c_string1 > std::string("test2"), false);
  BOOST_CHECK_EQUAL(c_string1 <= std::string("test2"), true);
  BOOST_CHECK_EQUAL(c_string1 < std::string("test2"), true);
  BOOST_CHECK_EQUAL(c_string1 == std::string("test1"), true);
  BOOST_CHECK_EQUAL(c_string1 == std::string("test4"), false);

  BOOST_CHECK_EQUAL(c_float1 == 2.2f, false);
  BOOST_CHECK_EQUAL(c_float1 >= 2.2f, false);
  BOOST_CHECK_EQUAL(c_float1 > 2.2f, false);
  BOOST_CHECK_EQUAL(c_float1 <= 2.2f, true);
  BOOST_CHECK_EQUAL(c_float1 < 2.2f, true);

  BOOST_CHECK_EQUAL(c_int1 == 43, false);
  BOOST_CHECK_EQUAL(c_int1 >= 43, false);
  BOOST_CHECK_EQUAL(c_int1 > 43, false);
  BOOST_CHECK_EQUAL(c_int1 <= 43, true);
  BOOST_CHECK_EQUAL(c_int1 < 43, true);
  BOOST_CHECK_EQUAL(c_int1 == 42, true);
  BOOST_CHECK_EQUAL(c_int1 >= 42, true);
  BOOST_CHECK_EQUAL(c_int1 > 42, false);
  BOOST_CHECK_EQUAL(c_int1 <= 42, true);
  BOOST_CHECK_EQUAL(c_int1 < 42, false);

  BOOST_CHECK_EQUAL(c_longint1 == 43l, false);
  BOOST_CHECK_EQUAL(c_longint1 >= 43l, false);
  BOOST_CHECK_EQUAL(c_longint1 > 43l, false);
  BOOST_CHECK_EQUAL(c_longint1 <= 43l, true);
  BOOST_CHECK_EQUAL(c_longint1 < 43l, true);
  BOOST_CHECK_EQUAL(c_longint1 == 42l, true);
  BOOST_CHECK_EQUAL(c_longint1 >= 42l, true);
  BOOST_CHECK_EQUAL(c_longint1 > 42l, false);
  BOOST_CHECK_EQUAL(c_longint1 <= 42l, true);
  BOOST_CHECK_EQUAL(c_longint1 < 42l, false);

  BOOST_CHECK_EQUAL(c_bool1 == false, false);
  BOOST_CHECK_EQUAL(c_bool1 >= false, true);
  BOOST_CHECK_EQUAL(c_bool1 > false, true);
  BOOST_CHECK_EQUAL(c_bool1 <= false, false);
  BOOST_CHECK_EQUAL(c_bool1 < false, false);
  BOOST_CHECK_EQUAL(c_bool1 == true, true);
  BOOST_CHECK_EQUAL(c_bool1 >= true, true);
  BOOST_CHECK_EQUAL(c_bool1 > true, false);
  BOOST_CHECK_EQUAL(c_bool1 <= true, true);
  BOOST_CHECK_EQUAL(c_bool1 < true, false);

  // not supported always return true
  BOOST_CHECK_EQUAL(c_bool_list1 == bool_list2, true);
  BOOST_CHECK_EQUAL(c_bool_list1 >= bool_list2, true);
  BOOST_CHECK_EQUAL(c_bool_list1 > bool_list2, true);
  BOOST_CHECK_EQUAL(c_bool_list1 <= bool_list2, true);
  BOOST_CHECK_EQUAL(c_bool_list1 < bool_list2, true);

  // not supported always return true
  BOOST_CHECK_EQUAL(c_float_list1 == float_list2, true);
  BOOST_CHECK_EQUAL(c_float_list1 >= float_list2, true);
  BOOST_CHECK_EQUAL(c_float_list1 > float_list2, true);
  BOOST_CHECK_EQUAL(c_float_list1 <= float_list2, true);
  BOOST_CHECK_EQUAL(c_float_list1 < float_list2, true);

  // not supported always return true
  BOOST_CHECK_EQUAL(c_int_list1 == int_list2, true);
  BOOST_CHECK_EQUAL(c_int_list1 >= int_list2, true);
  BOOST_CHECK_EQUAL(c_int_list1 > int_list2, true);
  BOOST_CHECK_EQUAL(c_int_list1 <= int_list2, true);
  BOOST_CHECK_EQUAL(c_int_list1 < int_list2, true);

  // not supported always return true
  BOOST_CHECK_EQUAL(c_string_list1 == string_list2, true);
  BOOST_CHECK_EQUAL(c_string_list1 >= string_list2, true);
  BOOST_CHECK_EQUAL(c_string_list1 > string_list2, true);
  BOOST_CHECK_EQUAL(c_string_list1 <= string_list2, true);
  BOOST_CHECK_EQUAL(c_string_list1 < string_list2, true);

  // Comparing different types is not supported
  BOOST_CHECK_EQUAL(c_string1 == c_float1, false);
  BOOST_CHECK_EQUAL(c_float1 == c_int1, false);
  BOOST_CHECK_EQUAL(c_int1 == c_longint1, false);
  BOOST_CHECK_EQUAL(c_longint1 == c_bool1, false);
  BOOST_CHECK_EQUAL(c_bool1 == c_bool_list1, false);
  BOOST_CHECK_EQUAL(c_bool_list1 == c_float_list1, false);
  BOOST_CHECK_EQUAL(c_float_list1 == c_int_list1, false);
  BOOST_CHECK_EQUAL(c_int_list1 == c_string_list1, false);

}

BOOST_AUTO_TEST_CASE(castValue_comparison_methods)
{
  // From other cast values
  CastValue c_uninitialized1;
  CastValue c_uninitialized2;
  BOOST_CHECK_EQUAL(c_uninitialized1.is_equal_to(c_uninitialized2), true);
  BOOST_CHECK_EQUAL(c_uninitialized1.is_greater_than(c_uninitialized2), false);
  BOOST_CHECK_EQUAL(c_uninitialized1.is_less_than(c_uninitialized2), false);

  CastValue c_string1(std::string("test1"));
  CastValue c_string2(std::string("test2"));
  BOOST_CHECK_EQUAL(c_string1.is_equal_to(c_string2), false);
  BOOST_CHECK_EQUAL(c_string1.is_greater_than(c_string2), false);
  BOOST_CHECK_EQUAL(c_string1.is_less_than(c_string2), true);
  CastValue c_string3(std::string("test1"));
  BOOST_CHECK_EQUAL(c_string1.is_equal_to(c_string3), true);
  CastValue c_string4(std::string("TEST1"));
  BOOST_CHECK_EQUAL(c_string1.is_equal_to(c_string4), false);
  
  // non case sensitive
  BOOST_CHECK_EQUAL(c_string1.is_equal_to(c_string4, false), true);

  CastValue c_float1(1.1f);
  CastValue c_float2(2.2f);
  BOOST_CHECK_EQUAL(c_float1.is_equal_to(c_float2), false);
  BOOST_CHECK_EQUAL(c_float1.is_greater_than(c_float2), false);
  BOOST_CHECK_EQUAL(c_float1.is_less_than(c_float2), true);
  CastValue c_float3(1.1f);
  BOOST_CHECK_EQUAL(c_float1.is_equal_to(c_float3), true);
  BOOST_CHECK_EQUAL(c_float1.is_greater_than(c_float3), false);
  BOOST_CHECK_EQUAL(c_float1.is_less_than(c_float3), false);

  CastValue c_int1(42);
  CastValue c_int2(43);
  BOOST_CHECK_EQUAL(c_int1.is_equal_to(c_int2), false);
  BOOST_CHECK_EQUAL(c_int1.is_greater_than(c_int2), false);
  BOOST_CHECK_EQUAL(c_int1.is_less_than(c_int2), true);
  CastValue c_int3(42);
  BOOST_CHECK_EQUAL(c_int1.is_equal_to(c_int3), true);
  BOOST_CHECK_EQUAL(c_int1.is_greater_than(c_int3), false);
  BOOST_CHECK_EQUAL(c_int1.is_less_than(c_int3), false);

  CastValue c_longint1(42l);
  CastValue c_longint2(43l);
  BOOST_CHECK_EQUAL(c_longint1.is_equal_to(c_longint2), false);
  BOOST_CHECK_EQUAL(c_longint1.is_greater_than(c_longint2), false);
  BOOST_CHECK_EQUAL(c_longint1.is_less_than(c_longint2), true);
  CastValue c_longint3(42l);
  BOOST_CHECK_EQUAL(c_longint1.is_equal_to(c_longint3), true);
  BOOST_CHECK_EQUAL(c_longint1.is_greater_than(c_longint3), false);
  BOOST_CHECK_EQUAL(c_longint1.is_less_than(c_longint3), false);

  CastValue c_bool1(true);
  CastValue c_bool2(false);
  BOOST_CHECK_EQUAL(c_bool1.is_equal_to(c_bool2), false);
  BOOST_CHECK_EQUAL(c_bool1.is_greater_than(c_bool2), true);
  BOOST_CHECK_EQUAL(c_bool1.is_less_than(c_bool2), false);
  CastValue c_bool3(true);
  BOOST_CHECK_EQUAL(c_bool1.is_equal_to(c_bool3), true);
  BOOST_CHECK_EQUAL(c_bool1.is_greater_than(c_bool3), false);
  BOOST_CHECK_EQUAL(c_bool1.is_less_than(c_bool3), false);

  std::vector<bool> bool_list1 = { true, false, true };
  CastValue c_bool_list1(bool_list1);
  std::vector<bool> bool_list2 = { false, true, false };
  CastValue c_bool_list2(bool_list2);
  // not supported always return true
  BOOST_CHECK_EQUAL(c_bool_list1.is_equal_to(c_bool_list2), true);
  BOOST_CHECK_EQUAL(c_bool_list1.is_greater_than(c_bool_list2), true);
  BOOST_CHECK_EQUAL(c_bool_list1.is_less_than(c_bool_list2), true);

  std::vector<float> float_list1 = { 1.1f, 2.2f, 3.3f };
  CastValue c_float_list1(float_list1);
  std::vector<float> float_list2 = { 4.4f, 5.5f, 6.6f };
  CastValue c_float_list2(float_list2);
  // not supported always return true
  BOOST_CHECK_EQUAL(c_float_list1.is_equal_to(c_float_list2), true);
  BOOST_CHECK_EQUAL(c_float_list1.is_greater_than(c_float_list2), true);
  BOOST_CHECK_EQUAL(c_float_list1.is_less_than(c_float_list2), true);

  std::vector<int> int_list1 = { 1, 2, 3 };
  CastValue c_int_list1(int_list1);
  std::vector<int> int_list2 = { 4, 5, 6 };
  CastValue c_int_list2(int_list2);
  // not supported always return true
  BOOST_CHECK_EQUAL(c_int_list1.is_equal_to(c_int_list2), true);
  BOOST_CHECK_EQUAL(c_int_list1.is_greater_than(c_int_list2), true);
  BOOST_CHECK_EQUAL(c_int_list1.is_less_than(c_int_list2), true);

  std::vector<std::string> string_list1 = { "one", "two", "three" };
  CastValue c_string_list1(string_list1);
  std::vector<std::string> string_list2 = { "four", "five", "six" };
  CastValue c_string_list2(string_list2);
  // not supported always return true
  BOOST_CHECK_EQUAL(c_string_list1.is_equal_to(c_string_list2), true);
  BOOST_CHECK_EQUAL(c_string_list1.is_greater_than(c_string_list2), true);
  BOOST_CHECK_EQUAL(c_string_list1.is_less_than(c_string_list2), true);

  // Comparing different types is not supported
  BOOST_CHECK_EQUAL(c_string1.is_equal_to(c_float1), false);
  BOOST_CHECK_EQUAL(c_float1.is_equal_to(c_int1), false);
  BOOST_CHECK_EQUAL(c_int1.is_equal_to(c_longint1), false);
  BOOST_CHECK_EQUAL(c_longint1.is_equal_to(c_bool1), false);
  BOOST_CHECK_EQUAL(c_bool1.is_equal_to(c_bool_list1), false);
  BOOST_CHECK_EQUAL(c_bool_list1.is_equal_to(c_float_list1), false);
  BOOST_CHECK_EQUAL(c_float_list1.is_equal_to(c_int_list1), false);
  BOOST_CHECK_EQUAL(c_int_list1.is_equal_to(c_string_list1), false);
}

BOOST_AUTO_TEST_CASE(castValue_to_string)
{
  CastValue c_uninitialized;
  BOOST_CHECK_EQUAL(std::string(c_uninitialized), "false");

  CastValue c_string(std::string("test"));
  BOOST_CHECK_EQUAL(std::string(c_string), "test");

  CastValue c_charptr("test");
  BOOST_CHECK_EQUAL(std::string(c_charptr), "test");

  CastValue c_float(1.1f);
  BOOST_CHECK_EQUAL(std::string(c_float), "1.1");

  CastValue c_int(42);
  BOOST_CHECK_EQUAL(std::string(c_int), "42");

  CastValue c_longint(42l);
  BOOST_CHECK_EQUAL(std::string(c_longint), "42");

  CastValue c_bool(true);
  BOOST_CHECK_EQUAL(std::string(c_bool), "true");

  std::vector<bool> bool_list = { true, false, true };
  CastValue c_bool_list(bool_list);
  BOOST_CHECK_EQUAL(std::string(c_bool_list), "[true,false,true]");

  std::vector<float> float_list = { 1.1f, 2.2f, 3.3f };
  CastValue c_float_list(float_list);
  BOOST_CHECK_EQUAL(std::string(c_float_list), "[1.1,2.2,3.3]");

  std::vector<int> int_list = { 1, 2, 3 };
  CastValue c_int_list(int_list);
  BOOST_CHECK_EQUAL(std::string(c_int_list), "[1,2,3]");

  std::vector<std::string> string_list = { "one", "two", "three" };
  CastValue c_string_list(string_list);
  BOOST_CHECK_EQUAL(std::string(c_string_list), "['one','two','three']");
}

BOOST_AUTO_TEST_CASE(castValue_set_tag_and_data)
{
  CastValue c;

  CastValue c_uninitialized;
  c.setTagAndData(CastValue::Type::UNINITIALIZED, false);
  BOOST_CHECK_EQUAL(c.getTag() == CastValue::Type::UNINITIALIZED, true);
  BOOST_CHECK_EQUAL(c.b_, false);

  c.setTagAndData(CastValue::Type::STRING, std::string("test"));
  BOOST_CHECK_EQUAL(c.getTag() == CastValue::Type::STRING, true);
  BOOST_CHECK_EQUAL(c.s_, "test");

  c.setTagAndData(CastValue::Type::STRING, "test");
  BOOST_CHECK_EQUAL(c.getTag() == CastValue::Type::STRING, true);
  BOOST_CHECK_EQUAL(c.s_, "test");

  c.setTagAndData(CastValue::Type::FLOAT, 1.1f);
  BOOST_CHECK_EQUAL(c.getTag() == CastValue::Type::FLOAT, true);
  BOOST_CHECK_CLOSE(c.f_, 1.1f, 1e-3f);

  c.setTagAndData(CastValue::Type::INT, 42);
  BOOST_CHECK_EQUAL(c.getTag() == CastValue::Type::INT, true);
  BOOST_CHECK_EQUAL(c.i_, 42);

  c.setTagAndData(CastValue::Type::LONG_INT, 42l);
  BOOST_CHECK_EQUAL(c.getTag() == CastValue::Type::LONG_INT, true);
  BOOST_CHECK_EQUAL(c.li_, 42l);

  c.setTagAndData(CastValue::Type::BOOL, true);
  BOOST_CHECK_EQUAL(c.getTag() == CastValue::Type::BOOL, true);
  BOOST_CHECK_EQUAL(c.b_, true);

  std::vector<bool> bool_list = { true, false, true };
  c.setTagAndData(CastValue::Type::BOOL_LIST, bool_list);
  BOOST_CHECK_EQUAL(c.getTag() == CastValue::Type::BOOL_LIST, true);
  BOOST_CHECK_EQUAL_COLLECTIONS(c.bl_.begin(), c.bl_.end(), bool_list.begin(), bool_list.end());

  std::vector<float> float_list = { 1.1f, 2.2f, 3.3f };
  c.setTagAndData(CastValue::Type::FLOAT_LIST, float_list);
  BOOST_CHECK_EQUAL(c.getTag() == CastValue::Type::FLOAT_LIST, true);
  BOOST_CHECK_EQUAL_COLLECTIONS(c.fl_.begin(), c.fl_.end(), float_list.begin(), float_list.end());

  std::vector<int> int_list = { 1, 2, 3 };
  c.setTagAndData(CastValue::Type::INT_LIST, int_list);
  BOOST_CHECK_EQUAL(c.getTag() == CastValue::Type::INT_LIST, true);
  BOOST_CHECK_EQUAL_COLLECTIONS(c.il_.begin(), c.il_.end(), int_list.begin(), int_list.end());

  std::vector<std::string> string_list = { "one", "two", "three" };
  c.setTagAndData(CastValue::Type::STRING_LIST, string_list);
  BOOST_CHECK_EQUAL(c.getTag() == CastValue::Type::STRING_LIST, true);
  BOOST_CHECK_EQUAL_COLLECTIONS(c.sl_.begin(), c.sl_.end(), string_list.begin(), string_list.end());

  // unknown type
  c.setTagAndData(CastValue::Type::UNKNOWN, "test");
  BOOST_CHECK_EQUAL(c.getTag() == CastValue::Type::UNKNOWN, true);
  BOOST_CHECK_EQUAL(c.s_, "test");

}

BOOST_AUTO_TEST_CASE(castValue_operator_extraction)
{
  std::ostringstream oss;

  CastValue c_uninitialized;
  oss << c_uninitialized;
  BOOST_CHECK_EQUAL(oss.str(), "false");
  oss.str("");
  oss.clear();

  CastValue c_string(std::string("test"));
  oss << c_string;
  BOOST_CHECK_EQUAL(oss.str(), "test");
  oss.str("");
  oss.clear();

  CastValue c_charptr("test");
  oss << c_charptr;
  BOOST_CHECK_EQUAL(oss.str(), "test");
  oss.str("");
  oss.clear();

  CastValue c_float(1.1f);
  oss << c_float;
  BOOST_CHECK_EQUAL(oss.str(), "1.1");
  oss.str("");
  oss.clear();

  CastValue c_int(42);
  oss << c_int;
  BOOST_CHECK_EQUAL(oss.str(), "42");
  oss.str("");
  oss.clear();

  CastValue c_longint(42l);
  oss << c_longint;
  BOOST_CHECK_EQUAL(oss.str(), "42");
  oss.str("");
  oss.clear();

  CastValue c_bool(true);
  oss << c_bool;
  BOOST_CHECK_EQUAL(oss.str(), "true");
  oss.str("");
  oss.clear();

  std::vector<bool> bool_list = { true, false, true };
  CastValue c_bool_list(bool_list);
  oss << c_bool_list;
  BOOST_CHECK_EQUAL(oss.str(), "[true,false,true]");
  oss.str("");
  oss.clear();

  std::vector<float> float_list = { 1.1f, 2.2f, 3.3f };
  CastValue c_float_list(float_list);
  oss << c_float_list;
  BOOST_CHECK_EQUAL(oss.str(), "[1.1,2.2,3.3]");
  oss.str("");
  oss.clear();

  std::vector<int> int_list = { 1, 2, 3 };
  CastValue c_int_list(int_list);
  oss << c_int_list;
  BOOST_CHECK_EQUAL(oss.str(), "[1,2,3]");
  oss.str("");
  oss.clear();

  std::vector<std::string> string_list = { "one", "two", "three" };
  CastValue c_string_list(string_list);
  oss << c_string_list;
  BOOST_CHECK_EQUAL(oss.str(), "['one','two','three']");
  oss.str("");
  oss.clear();
}

BOOST_AUTO_TEST_SUITE_END()
