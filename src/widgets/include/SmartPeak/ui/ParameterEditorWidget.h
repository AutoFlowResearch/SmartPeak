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
// $Maintainer: Douglas McCloskey, Bertrand Boudaud, Ahmed Khalil $
// $Authors: Douglas McCloskey, Bertrand Boudaud $
// --------------------------------------------------------------------------
#pragma once

#include <SmartPeak/ui/Widget.h>
#include <SmartPeak/core/Parameters.h>
#include <string>
#include <vector>

namespace SmartPeak
{
  struct IParameterEditorWidgetObserver
  {
    virtual void onParameterSet(const std::string& function_parameter, const Parameter& parameter) = 0;
    virtual void onParameterRemoved(const std::string& function_parameter, const Parameter& parameter) = 0;
  };

  class ParameterEditorWidget final : public Widget
  {
  public:
    ParameterEditorWidget(IParameterEditorWidgetObserver& observer, bool enable_remove = false) :
      parameter_("parameter"), // dummy name
      input_text_field_(),
      observer_(observer),
      enable_remove_(enable_remove)
    {
    };
    void draw() override;
    void setParameter(const std::string& function_parameter, const Parameter& parameter);

  protected:
    std::string function_parameter_;
    Parameter parameter_;
    std::string title_;
    std::string default_value_;
    std::array<char, 256> input_text_field_ = { 0 };
    std::vector<std::string> valid_string_;
  private:
    void setInputTextField(const std::string& value);
    IParameterEditorWidgetObserver& observer_;
    bool enable_remove_;
  };
}
