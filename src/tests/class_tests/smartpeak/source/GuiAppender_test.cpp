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
  for (const plog::util::nstring& s : appender.getMessageList(plog::Severity::verbose)) {
    std::cout << s << std::endl;
  }
}

BOOST_AUTO_TEST_SUITE_END()
