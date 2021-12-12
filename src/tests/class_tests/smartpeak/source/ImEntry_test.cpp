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
// $Maintainer: Bertrand Boudaud, Ahmed Khalil, Douglas McCloskey $
// $Authors: Bertrand Boudaud $
// --------------------------------------------------------------------------

#include <gtest/gtest.h>
#include <SmartPeak/ui/ImEntry.h>
#include <SmartPeak/test_config.h>

TEST(ImEntry, is_digit)
{
  EXPECT_TRUE(SmartPeak::ImEntry::is_digit('4'));
  EXPECT_TRUE(SmartPeak::ImEntry::is_digit('0'));
  EXPECT_FALSE(SmartPeak::ImEntry::is_digit('A'));
  EXPECT_FALSE(SmartPeak::ImEntry::is_digit('?'));
}

TEST(ImEntry, is_number)
{
  EXPECT_TRUE(SmartPeak::ImEntry::is_number("42"));
  EXPECT_FALSE(SmartPeak::ImEntry::is_number("Monday"));
  EXPECT_FALSE(SmartPeak::ImEntry::is_number("-42"));
  EXPECT_FALSE(SmartPeak::ImEntry::is_number("1e3"));
  EXPECT_FALSE(SmartPeak::ImEntry::is_number("-1e3"));
  EXPECT_FALSE(SmartPeak::ImEntry::is_number("3.14"));
}

TEST(ImEntry, lexicographical_sort)
{
  EXPECT_EQ(SmartPeak::ImEntry::lexicographical_sort("Monday", "Tuesday"), -7);
  EXPECT_EQ(SmartPeak::ImEntry::lexicographical_sort("Test1", "Test 2"), -1);
  EXPECT_EQ(SmartPeak::ImEntry::lexicographical_sort("", "Empty test 1"), -1);
  EXPECT_EQ(SmartPeak::ImEntry::lexicographical_sort("Empty test 2", ""), 1);
  EXPECT_EQ(SmartPeak::ImEntry::lexicographical_sort("AaAa", "AAAA"), 0);
  EXPECT_EQ(SmartPeak::ImEntry::lexicographical_sort("5-HTP", "5-HTP"), 0);
}

TEST(ImEntry, CompareWithSortSpecs)
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
  EXPECT_EQ(SmartPeak::ImEntry::CompareWithSortSpecs(&entry1, &entry2), -1);
  table_specs.ColumnIndex = 1;
  EXPECT_EQ(SmartPeak::ImEntry::CompareWithSortSpecs(&entry1, &entry2), -1);
  table_specs.ColumnIndex = 2;
  EXPECT_EQ(SmartPeak::ImEntry::CompareWithSortSpecs(&entry1, &entry2), -1);
  table_specs.ColumnIndex = 3;
  EXPECT_EQ(SmartPeak::ImEntry::CompareWithSortSpecs(&entry1, &entry2), 1);
  table_specs.ColumnIndex = 4;
  EXPECT_EQ(SmartPeak::ImEntry::CompareWithSortSpecs(&entry1, &entry2), 0);
  table_specs.ColumnIndex = 5;
  EXPECT_EQ(SmartPeak::ImEntry::CompareWithSortSpecs(&entry1, &entry2), 0);

  table_specs.SortDirection = ImGuiSortDirection_Descending;
  table_specs.ColumnIndex = 0;
  EXPECT_EQ(SmartPeak::ImEntry::CompareWithSortSpecs(&entry1, &entry2), 1);
  table_specs.ColumnIndex = 1;
  EXPECT_EQ(SmartPeak::ImEntry::CompareWithSortSpecs(&entry1, &entry2), 1);
  table_specs.ColumnIndex = 2;
  EXPECT_EQ(SmartPeak::ImEntry::CompareWithSortSpecs(&entry1, &entry2), 1);
  table_specs.ColumnIndex = 3;
  EXPECT_EQ(SmartPeak::ImEntry::CompareWithSortSpecs(&entry1, &entry2), -1);
  table_specs.ColumnIndex = 4;
  EXPECT_EQ(SmartPeak::ImEntry::CompareWithSortSpecs(&entry1, &entry2), 0);
  table_specs.ColumnIndex = 5;
  EXPECT_EQ(SmartPeak::ImEntry::CompareWithSortSpecs(&entry1, &entry2), 0);
}
