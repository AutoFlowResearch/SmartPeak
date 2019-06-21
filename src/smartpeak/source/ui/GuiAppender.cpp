#include <SmartPeak/ui/GuiAppender.h>
#include <vector>
#include <plog/Log.h>

namespace SmartPeak
{
  void GuiAppender::write(const plog::Record& record)
  {
    const plog::util::nstring str = plog::TxtFormatter::format(record);
    // TODO: use lock guard, multiple threads shoul not push_back at the same time
    messages.push_back(str);
  }

  std::vector<plog::util::nstring> GuiAppender::getMessageList() const
  {
    // TODO: use lock guard, because other threads might be modifying the data
    // and for example trigger a resize (reallocation, hence different addresses to access)
    return messages;
  }
}
