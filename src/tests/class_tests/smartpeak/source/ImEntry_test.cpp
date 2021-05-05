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
// $Maintainer: Bertrand Boudaud $
// $Authors: Bertrand Boudaud $
// --------------------------------------------------------------------------

#define BOOST_TEST_MODULE ImEntry test suite
#include <boost/test/included/unit_test.hpp>
#include <SmartPeak/ui/ImEntry.h>
#include <SmartPeak/test_config.h>

BOOST_AUTO_TEST_SUITE(ImEntry)

BOOST_AUTO_TEST_CASE(is_digit)
{
  BOOST_CHECK_EQUAL(SmartPeak::ImEntry::is_digit('4'), true);
  BOOST_CHECK_EQUAL(SmartPeak::ImEntry::is_digit('0'), true);
  BOOST_CHECK_EQUAL(SmartPeak::ImEntry::is_digit('A'), false);
  BOOST_CHECK_EQUAL(SmartPeak::ImEntry::is_digit('?'), false);
}

BOOST_AUTO_TEST_CASE(is_number)
{
  BOOST_CHECK_EQUAL(SmartPeak::ImEntry::is_number("42"), true);
  BOOST_CHECK_EQUAL(SmartPeak::ImEntry::is_number("Monday"), false);
  BOOST_CHECK_EQUAL(SmartPeak::ImEntry::is_number("-42"), false);
  BOOST_CHECK_EQUAL(SmartPeak::ImEntry::is_number("1e3"), false);
  BOOST_CHECK_EQUAL(SmartPeak::ImEntry::is_number("-1e3"), false);
  BOOST_CHECK_EQUAL(SmartPeak::ImEntry::is_number("3.14"), false);
}

BOOST_AUTO_TEST_CASE(lexicographical_sort)
{
  BOOST_CHECK_EQUAL(SmartPeak::ImEntry::lexicographical_sort("Monday", "Tuesday"), -7);
  BOOST_CHECK_EQUAL(SmartPeak::ImEntry::lexicographical_sort("Test1", "Test 2"), -1);
  BOOST_CHECK_EQUAL(SmartPeak::ImEntry::lexicographical_sort("", "Empty test 1"), -1);
  BOOST_CHECK_EQUAL(SmartPeak::ImEntry::lexicographical_sort("Empty test 2", ""), 1);
  BOOST_CHECK_EQUAL(SmartPeak::ImEntry::lexicographical_sort("AaAa", "AAAA"), 0);
  BOOST_CHECK_EQUAL(SmartPeak::ImEntry::lexicographical_sort("5-HTP", "5-HTP"), 0);
}


BOOST_AUTO_TEST_CASE(CompareWithSortSpecs)
{
  SmartPeak::ImEntry entry1{ 1, { "Monday", "Test1", "", "Empty test 2", "AaAa", "5-HTP" } };
  SmartPeak::ImEntry entry2{ 2, { "Tuesday", "Test 2", "Empty test 1", "", "AAAA", "5-HTP" } };

  ImGuiTableColumnSortSpecs table_specs;
  table_specs.SortDirection = ImGuiSortDirection_Ascending;
  table_specs.SortOrder = 0;
  ImGuiTableSortSpecs specs;
  specs.Specs = &table_specs;
  specs.SpecsCount = 1;
  SmartPeak::ImEntry::s_current_sort_specs = &specs;

  table_specs.ColumnIndex = 0;
  BOOST_CHECK_EQUAL(SmartPeak::ImEntry::CompareWithSortSpecs(&entry1, &entry2), -1);
  table_specs.ColumnIndex = 1;
  BOOST_CHECK_EQUAL(SmartPeak::ImEntry::CompareWithSortSpecs(&entry1, &entry2), -1);
  table_specs.ColumnIndex = 2;
  BOOST_CHECK_EQUAL(SmartPeak::ImEntry::CompareWithSortSpecs(&entry1, &entry2), -1);
  table_specs.ColumnIndex = 3;
  BOOST_CHECK_EQUAL(SmartPeak::ImEntry::CompareWithSortSpecs(&entry1, &entry2), 1);
  table_specs.ColumnIndex = 4;
  BOOST_CHECK_EQUAL(SmartPeak::ImEntry::CompareWithSortSpecs(&entry1, &entry2), 0);
  table_specs.ColumnIndex = 5;
  BOOST_CHECK_EQUAL(SmartPeak::ImEntry::CompareWithSortSpecs(&entry1, &entry2), 0);

  table_specs.SortDirection = ImGuiSortDirection_Descending;
  table_specs.ColumnIndex = 0;
  BOOST_CHECK_EQUAL(SmartPeak::ImEntry::CompareWithSortSpecs(&entry1, &entry2), 1);
  table_specs.ColumnIndex = 1;
  BOOST_CHECK_EQUAL(SmartPeak::ImEntry::CompareWithSortSpecs(&entry1, &entry2), 1);
  table_specs.ColumnIndex = 2;
  BOOST_CHECK_EQUAL(SmartPeak::ImEntry::CompareWithSortSpecs(&entry1, &entry2), 1);
  table_specs.ColumnIndex = 3;
  BOOST_CHECK_EQUAL(SmartPeak::ImEntry::CompareWithSortSpecs(&entry1, &entry2), -1);
  table_specs.ColumnIndex = 4;
  BOOST_CHECK_EQUAL(SmartPeak::ImEntry::CompareWithSortSpecs(&entry1, &entry2), 0);
  table_specs.ColumnIndex = 5;
  BOOST_CHECK_EQUAL(SmartPeak::ImEntry::CompareWithSortSpecs(&entry1, &entry2), 0);
}

BOOST_AUTO_TEST_SUITE_END()
