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
// $Maintainer: Douglas McCloskey $
// $Authors: Douglas McCloskey, Bertrand Boudaud $
// --------------------------------------------------------------------------

#include <SmartPeak/ui/LogWidget.h>
#include <imgui.h>
#include <plog/Log.h>

namespace SmartPeak
{
  void LogWidget::draw()
  {
    showQuickHelpToolTip("Log");
    
    const char* items[] = { "NONE", "FATAL", "ERROR", "WARN", "INFO", "DEBUG", "VERB" }; // reflects the strings in plog's Severity.h
    static int selected_severity = 4;
    static plog::Severity severity = plog::Severity::info;

    if (ImGui::Combo("Level", &selected_severity, items, IM_ARRAYSIZE(items)))
    {
      severity = plog::severityFromString(items[selected_severity]);
    }

    ImGui::Separator();
    ImGui::BeginChild("Log child");
    const std::vector<plog::util::nstring> message_list = appender_.getMessageList(severity);
    int message_list_start = (message_list.size() > 500) ? message_list.size() - 500 : 0;
    for (int i = message_list_start; i < message_list.size(); ++i)
    {
      std::string str(message_list.at(i).data(), message_list.at(i).data() + message_list.at(i).size());
      ImGui::Text("%s", str.c_str());
    }
    ImGui::EndChild();
  }
}
