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

#include <SmartPeak/ui/ParameterEditorWidget.h>
#include <imgui.h>
#include <imgui_internal.h>
#include <SmartPeak/core/Utilities.h>
#include <plog/Log.h>

namespace SmartPeak
{
  void ParameterEditorWidget::draw()
  {
    float popup_width = 400;

    if (!ImGui::BeginPopupModal("Edit Parameter", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
      return;
    }

    // one documented way to set popup width
    const auto cursor_pos = ImGui::GetCursorPosX();
    ImGui::SetCursorPosX(popup_width);
    ImGui::SetCursorPosX(cursor_pos);

    ImGui::Text(title_.c_str());
    ImGui::Separator();
    ImGui::PushTextWrapPos();
    ImGui::Text(parameter_.getDescription().c_str());
    ImGui::PopTextWrapPos();

    if (!valid_string_.empty() && ImGui::BeginCombo("Value", input_text_field_.data()))
    {
      for (const auto valid_string : valid_string_)
      {
        if (ImGui::Selectable(valid_string.c_str()))
        {
          setInputTextField(valid_string);
        }
      }
      ImGui::EndCombo();
    }
    else if (valid_string_.empty())
    {
      ImGui::InputText("Value", input_text_field_.data(), input_text_field_.size());
    }

    ImGui::Text(default_value_.c_str());

    // display min/max
    if (!parameter_.getRestrictionsAsString().empty() && parameter_.getValidStrings().empty())
    {
      ImGui::Text(parameter_.getRestrictionsAsString().c_str());
    }


    // Test if user input is valid
    CastValue validity_test_cast;
    std::string validity_test_string(input_text_field_.data());
    Utilities::parseString(validity_test_string, validity_test_cast);
    if ((validity_test_cast.getTag() == CastValue::Type::INT) && (parameter_.getType() == "float"))
    {
      // the parser will consider non decimal-annoted number as int, 
      // which will not be valid if the parameter is float type.
      // force conversion.
      validity_test_cast = static_cast<float>(validity_test_cast.i_);
    }
    bool valid = parameter_.isValid(validity_test_cast);

    if (!valid) ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
    if (ImGui::Button("Set"))
    {
      if (valid)
      {
        ParameterSet& user_parameters = application_handler_.sequenceHandler_.getSequence().at(0).getRawData().getParameters();
        Parameter* existing_parameter = user_parameters.findParameter(function_parameter_, parameter_.getName());
        if (existing_parameter)
        {
          existing_parameter->setValueFromString(std::string(input_text_field_.data()), false);
        }
        else
        {
          parameter_.setValueFromString(std::string(input_text_field_.data()), false);
          user_parameters.addParameter(function_parameter_, parameter_);
        }
        application_handler_.sequenceHandler_.notifyParametersChanged();
        ImGui::CloseCurrentPopup();
      }
      else
      {
        LOGE << "ParameterEditorWidget : Parameter value for " << parameter_.getName() << ", " << input_text_field_.data();
        LOGE << "is invalid. Check constraints and type. Expected " << parameter_.getType() << " " << parameter_.getRestrictionsAsString();
      }
    }
    if (!valid) ImGui::PopStyleVar();

    ImGui::SameLine();
    if (ImGui::Button("Unset"))
    {
      ParameterSet& user_parameters = application_handler_.sequenceHandler_.getSequence().at(0).getRawData().getParameters();
      if (user_parameters.count(function_parameter_) == 1)
      {
        user_parameters[function_parameter_].removeParameter(parameter_.getName());
        application_handler_.sequenceHandler_.notifyParametersChanged();
      }
      ImGui::CloseCurrentPopup();
    }

    ImGui::SameLine();
    if (ImGui::Button("Cancel"))
    {
      ImGui::CloseCurrentPopup();
    }

    ImGui::EndPopup();
  }

  void ParameterEditorWidget::setParameter(const std::string& function_parameter, const Parameter& parameter, const std::string& default_value)
  { 
    function_parameter_ = function_parameter;
    std::ostringstream default_value_stream;
    default_value_stream << "Default value: " << default_value;
    default_value_ = default_value_stream.str();
    parameter_ = parameter;
    setInputTextField(parameter.getValueAsString());
    if (parameter_.getType() == "bool")
    {
      valid_string_ = {
          "true",
          "false"
      };
    }
    else
    {
      valid_string_.clear();
      for (const auto& cast_value : parameter.getValidStrings())
      {
        valid_string_.push_back(cast_value);
      }
    }
    std::ostringstream title_stream;
    title_stream << parameter_.getName() << " (" << parameter.getType() << ")";
    title_ = title_stream.str();
  };

  void ParameterEditorWidget::setInputTextField(const std::string& value)
  {
    std::fill(input_text_field_.begin(), input_text_field_.end(), 0);
    if (value.size() < input_text_field_.size())
    {
      std::copy(value.begin(), value.end(), input_text_field_.data());
    }
    else
    {
      // else we are in trouble, abnormal long value parameter, do not display.
      LOGE << "ParameterEditorWidget : Parameter value for " << parameter_.getName() << " is too long to display. (" << value.size() << ")";
      std::string long_value_placeholder("# unable to display");
      std::copy(long_value_placeholder.begin(), long_value_placeholder.end(), input_text_field_.data());
    };
  }

}
