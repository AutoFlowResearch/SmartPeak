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
#include <SmartPeak/ui/UIUtilities.h>

using namespace SmartPeak;
using namespace std;

TEST(utilities, makeHumanReadable)
{
  SmartPeak::ImEntry dir_entry_1;
  dir_entry_1.ID = 0;
  dir_entry_1.entry_contents.resize(4, "");
  dir_entry_1.entry_contents[0] = "testfile.csv";
  dir_entry_1.entry_contents[1] = "1325000000";
  dir_entry_1.entry_contents[2] = ".csv";
  dir_entry_1.entry_contents[3] = "2021-03-22 06:59:29";

  UIUtilities::makeHumanReadable(dir_entry_1);
  EXPECT_STREQ(dir_entry_1.entry_contents[0].c_str(), "testfile.csv");
  EXPECT_STREQ(dir_entry_1.entry_contents[1].c_str(), "1.32 GB");
  EXPECT_STREQ(dir_entry_1.entry_contents[2].c_str(), "csv");
  EXPECT_STREQ(dir_entry_1.entry_contents[3].c_str(), "Mon Mar 22 06:59:29 2021");
}
