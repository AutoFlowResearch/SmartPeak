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
// $Maintainer: Ahmed Khalil $
// $Authors: Ahmed Khalil $
// --------------------------------------------------------------------------

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
