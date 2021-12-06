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
// $Authors: Douglas McCloskey $
// --------------------------------------------------------------------------

#include <SmartPeak/ui/WindowSizesAndPositions.h>
#include <imgui.h>

namespace SmartPeak
{

  std::string WindowSizesAndPositions::getPropertiesHandlerName() const
  {
    return "WindowSizesAndPositions";
  }

  std::map<std::string, CastValue::Type> WindowSizesAndPositions::getPropertiesSchema() const
  {
    std::map<std::string, CastValue::Type> properties;
    properties.emplace("imgui_ini", CastValue::Type::STRING);
    return properties;
  }

  std::optional<CastValue> WindowSizesAndPositions::getProperty(const std::string& property, const size_t row) const
  {
    if (property == "imgui_ini")
    {
      std::string imgui_ini(ImGui::SaveIniSettingsToMemory());
      return imgui_ini;
    }
    return std::nullopt;
  }

  void WindowSizesAndPositions::setProperty(const std::string& property, const CastValue& value, const size_t row)
  {
    if (property == "imgui_ini")
    {
      imgui_ini_ = value.s_;
      imgui_ini_updated_ = true;
    }
  }

  void WindowSizesAndPositions::applyLayout()
  {
    if (imgui_ini_updated_)
    {
      ImGui::LoadIniSettingsFromMemory(imgui_ini_.c_str());
      imgui_ini_updated_ = false;
    }
  }
}
