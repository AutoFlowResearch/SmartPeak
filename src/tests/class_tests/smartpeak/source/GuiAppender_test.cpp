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
#include <SmartPeak/test_config.h>
#include <SmartPeak/ui/GuiAppender.h>
#include <thread>

using namespace SmartPeak;
using namespace std;

void f(GuiAppender& appender, const std::vector<int>& numbers)
{
  for (int n : numbers) {
    plog::Record r(plog::Severity::none, "some func", 0, "some file", nullptr);
    r << std::this_thread::get_id() << " " << n;
    appender.write(r);
  }
}

TEST(Guiappender, constructor)
{
  GuiAppender* p1 = nullptr;
  GuiAppender* nullPointer = nullptr;
  p1 = new GuiAppender();
  EXPECT_NE(p1, nullPointer);
  delete p1;
}

TEST(Guiappender, thread_safety)
{
  GuiAppender appender;
  const vector<int> numbers = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
  std::cout << "spawning threads...\n";
  std::thread t1(f, std::ref(appender), std::cref(numbers));
  std::thread t2(f, std::ref(appender), std::cref(numbers));
  std::thread t3(f, std::ref(appender), std::cref(numbers));
  std::cout << "joining threads...\n";
  t1.join();
  t2.join();
  t3.join();
  std::cout << "threads joined.\n";

  const std::vector<plog::util::nstring> log = appender.getMessageList(plog::Severity::verbose);

  EXPECT_EQ(log.size(), 30); // ensure the correct number of entries

  // auto endsWith = [](const plog::util::nchar* pattern)
  // {
  //   return [pattern](const plog::util::nstring& s)
  //   {
  //     const std::string::size_type n = s.rfind(pattern);
  //     return n != std::string::npos;
  //   };
  // };

  // ensure that entries end with the expected pattern, for the correct number of times
  // since we have three threads appending the same vector elements, we expect the same
  // string endings three times for each symbol
  // BOOST_CHECK_EQUAL(std::count_if(log.cbegin(), log.cend(), endsWith("0\n")), 3);
  // BOOST_CHECK_EQUAL(std::count_if(log.cbegin(), log.cend(), endsWith("1\n")), 3);
  // BOOST_CHECK_EQUAL(std::count_if(log.cbegin(), log.cend(), endsWith("2\n")), 3);
  // BOOST_CHECK_EQUAL(std::count_if(log.cbegin(), log.cend(), endsWith("3\n")), 3);
  // BOOST_CHECK_EQUAL(std::count_if(log.cbegin(), log.cend(), endsWith("4\n")), 3);
  // BOOST_CHECK_EQUAL(std::count_if(log.cbegin(), log.cend(), endsWith("5\n")), 3);
  // BOOST_CHECK_EQUAL(std::count_if(log.cbegin(), log.cend(), endsWith("6\n")), 3);
  // BOOST_CHECK_EQUAL(std::count_if(log.cbegin(), log.cend(), endsWith("7\n")), 3);
  // BOOST_CHECK_EQUAL(std::count_if(log.cbegin(), log.cend(), endsWith("8\n")), 3);
  // BOOST_CHECK_EQUAL(std::count_if(log.cbegin(), log.cend(), endsWith("9\n")), 3);
}
