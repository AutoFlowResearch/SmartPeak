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
// $Maintainer: Douglas McCloskey, Bertrand Boudaud $
// $Authors: Douglas McCloskey, Bertrand Boudaud $
// --------------------------------------------------------------------------

#include <SmartPeak/core/SessionLoaderGenerator.h>
#include <plog/Log.h>

namespace SmartPeak
{

  void SessionLoaderGenerator::onApplicationProcessorStart(const std::vector<std::string>& commands)
  {
    for (const auto& command : commands)
    {
      if (command == "CLEAR_DATA")
      {
        loading_workflow_commands_.clear();
      }
      else if (command == "LOAD_RAW_DATA")
      {
        loading_workflow_commands_.emplace(0, "LOAD_RAW_DATA");
      }
      else if (command == "STORE_FEATURES")
      {
        loading_workflow_commands_.emplace(10, "LOAD_FEATURES");
      }
      else if (command == "MAP_CHROMATOGRAMS")
      {
        loading_workflow_commands_.emplace(20, "MAP_CHROMATOGRAMS");
      }
      else if (command == "EXTRACT_CHROMATOGRAM_WINDOWS")
      {
        loading_workflow_commands_.emplace(21, "EXTRACT_CHROMATOGRAM_WINDOWS");
      }
      else if (command == "ZERO_CHROMATOGRAM_BASELINE")
      {
        loading_workflow_commands_.emplace(22, "ZERO_CHROMATOGRAM_BASELINE");
      }
      else if (command == "MERGE_SPECTRA")
      {
        loading_workflow_commands_.emplace(23, "MERGE_SPECTRA");
      }
      else if (command == "EXTRACT_SPECTRA_WINDOWS")
      {
        loading_workflow_commands_.emplace(24, "EXTRACT_SPECTRA_WINDOWS");
      }
    }
  }

  std::string SessionLoaderGenerator::getPropertiesHandlerName() const
  {
    return "loading_workflow";
  }

  std::map<std::string, CastValue::Type> SessionLoaderGenerator::getPropertiesSchema() const
  {
    return { {"commands", CastValue::Type::STRING_LIST } };
  }

  std::optional<CastValue> SessionLoaderGenerator::getProperty(const std::string& property, const size_t row) const
  {
    if (property == "commands")
    {
      return getLoadingWorkflowCommands();
    }
    return std::nullopt;
  }

  void SessionLoaderGenerator::setProperty(const std::string& property, const CastValue& value, const size_t row)
  {
    int order = 0;
    for (const auto& command : value.sl_)
    {
      loading_workflow_commands_.emplace(order, command);
      ++order; // Note: the order is used only when constructing the loading workflow. no need to serialize it, so we fake it there.
    }
  }

  std::vector<std::string> SessionLoaderGenerator::getLoadingWorkflowCommands() const
  {
    std::vector<std::string> commands;
    for (const auto& [order, command] : loading_workflow_commands_)
    {
      commands.push_back(command);
    }
    return commands;
  }

}
