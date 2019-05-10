#include <SmartPeak/ui/AppManager.h>
#include <plog/Log.h>
#include <plog/Appenders/ConsoleAppender.h>

using namespace SmartPeak;

// Inspired by plog::TxtFormatter
struct ConsoleFormatter
{
  static plog::util::nstring header()
  {
    return plog::util::nstring();
  }

  static plog::util::nstring format(const plog::Record& record)
  {
    if (record.getSeverity() > plog::info) {
      return "";
    }

    std::tm t;
    (plog::util::localtime_s)(&t, &record.getTime().time);

    plog::util::nostringstream ss;
    ss << t.tm_year + 1900 << "-" << std::setfill(PLOG_NSTR('0')) << std::setw(2) << t.tm_mon + 1 << PLOG_NSTR("-") << std::setfill(PLOG_NSTR('0')) << std::setw(2) << t.tm_mday << PLOG_NSTR(" ");
    ss << std::setfill(PLOG_NSTR('0')) << std::setw(2) << t.tm_hour << PLOG_NSTR(":") << std::setfill(PLOG_NSTR('0')) << std::setw(2) << t.tm_min << PLOG_NSTR(":") << std::setfill(PLOG_NSTR('0')) << std::setw(2) << t.tm_sec << PLOG_NSTR(" ");
    // ss << std::setfill(PLOG_NSTR(' ')) << std::setw(5) << std::left << severityToString(record.getSeverity()) << PLOG_NSTR(" ");
    // ss << PLOG_NSTR("[") << record.getTid() << PLOG_NSTR("] ");
    ss << record.getMessage() << PLOG_NSTR("\n");

    return ss.str();
  }
};

int main()
{
  AppManager cli;
  cli.initFileLogger();

  // Add console appender
  static plog::ConsoleAppender<ConsoleFormatter> consoleAppender;
  plog::get()->addAppender(&consoleAppender);

  LOGN << "\n\n" << cli.gettingStartedString();
  while (true) {
    cli.menuMain();
  }

  return 0;
}
