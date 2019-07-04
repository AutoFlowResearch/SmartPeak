// TODO: Add copyright

#include <SmartPeak/test_config.h>

#define BOOST_TEST_MODULE GuiAppender test suite
#include <boost/test/included/unit_test.hpp>
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

BOOST_AUTO_TEST_SUITE(guiappender)

BOOST_AUTO_TEST_CASE(constructor)
{
  GuiAppender* p1 = nullptr;
  GuiAppender* nullPointer = nullptr;
  p1 = new GuiAppender();
  BOOST_CHECK_NE(p1, nullPointer);
  delete p1;
}

BOOST_AUTO_TEST_CASE(thread_safety)
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

  BOOST_CHECK_EQUAL(log.size(), 30); // ensure the correct number of entries

  auto endsWith = [](const plog::util::nchar* pattern)
  {
    return [pattern](const plog::util::nstring& s)
    {
      const std::string::size_type n = s.rfind(pattern);
      return n != std::string::npos;
    };
  };

  int my_count_if = [endsWith](const std::vector<plog::util::nstring>& v, const plog::util::nchar* pattern)
  {
    return std::count_if(v.cbegin(), v.cend(), endsWith(pattern));
  };

  // ensure that entries end with the expected pattern, for the correct number of times
  // since we have three threads appending the same vector elements, we expect the same
  // string endings three times for each symbol
  // BOOST_CHECK_EQUAL(my_count_if(log, "0\n"), 3);
  // BOOST_CHECK_EQUAL(my_count_if(log, "1\n"), 3);
  // BOOST_CHECK_EQUAL(my_count_if(log, "2\n"), 3);
  // BOOST_CHECK_EQUAL(my_count_if(log, "3\n"), 3);
  // BOOST_CHECK_EQUAL(my_count_if(log, "4\n"), 3);
  // BOOST_CHECK_EQUAL(my_count_if(log, "5\n"), 3);
  // BOOST_CHECK_EQUAL(my_count_if(log, "6\n"), 3);
  // BOOST_CHECK_EQUAL(my_count_if(log, "7\n"), 3);
  // BOOST_CHECK_EQUAL(my_count_if(log, "8\n"), 3);
  // BOOST_CHECK_EQUAL(my_count_if(log, "9\n"), 3);
}

BOOST_AUTO_TEST_SUITE_END()
