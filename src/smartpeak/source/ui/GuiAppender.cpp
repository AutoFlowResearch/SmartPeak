#include <SmartPeak/ui/GuiAppender.h>
#include <vector>
#include <plog/Log.h>

namespace SmartPeak
{
  void GuiAppender::write(const plog::Record& record)
  {
    plog::util::nstring str = plog::TxtFormatter::format(record);
    // TODO: use lock guard, multiple threads should not modify data at the same time
    messages.emplace_back(record.getSeverity(), str);
  }

  std::vector<plog::util::nstring> GuiAppender::getMessageList(plog::Severity severity) const
  {
    std::vector<plog::util::nstring> filtered;
    // TODO: use lock guard, because other threads might be modifying the data
    // and for example trigger a resize (reallocation, hence different addresses to access)
    for (const GARecord& p : messages) {
      if (p.first <= severity) {
        filtered.push_back(p.second);
      }
    }
    return filtered;
  }
}
