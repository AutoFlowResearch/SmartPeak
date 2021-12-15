
#include <SmartPeak/core/ServerAppender.h>

namespace SmartPeak
{
	void ServerAppender::write(const plog::Record& record)
    {
      std::ostringstream ss;
      ss << PLOG_NSTR("[") << record.getFunc() << PLOG_NSTR("@") << record.getLine() << PLOG_NSTR("] ");
      ss << record.getMessage() << PLOG_NSTR("\n");
      
      #ifdef _WIN32
      const std::string sss = ss.str();
      plog::util::nstring str(sss.begin(), sss.end());
      #else
      plog::util::nstring str = ss.str();
      #endif
      std::lock_guard<std::mutex> g(messages_mutex);
      messages.emplace_back(record.getSeverity(), str);
    }

    std::vector<ServerAppender::ServerAppenderRecord> ServerAppender::getAppenderRecordList(plog::Severity severity)
    {
      std::vector<ServerAppender::ServerAppenderRecord> filtered;
      std::lock_guard<std::mutex> g(messages_mutex);
      for (const ServerAppender::ServerAppenderRecord& p : messages) {
        if (p.first <= severity) {
          filtered.push_back(p);
        }
      }
      messages.clear();
      return filtered;
    }
	
}
