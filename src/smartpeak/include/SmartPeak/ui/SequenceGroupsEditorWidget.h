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
// $Authors: Bertrand Boudaud $
// --------------------------------------------------------------------------

#pragma once

#include <string>
#include <utility>
#include <vector>
#include <functional>
#include <imgui.h>
#include <SmartPeak/core/SessionHandler.h>
#include <SmartPeak/ui/Widget.h>
#include <SmartPeak/ui/SequenceGroupsEditorWidget.h>
#include <unsupported/Eigen/CXX11/Tensor>

namespace SmartPeak
{
  class SequenceGroupsEditorWidget : public Widget
  {
  public:

    SequenceGroupsEditorWidget(const std::string& title,
                              const std::string& move_option_message,
                              const std::string& create_option_message,
                              const std::string& move_action_message,
                              const std::string& create_action_message):
      Widget(title),
      move_option_message_(move_option_message),
      create_option_message_(create_option_message),
      move_action_message_(move_action_message),
      create_action_message_(create_action_message)
    {
    };

    virtual void draw() override;

    void open(std::set<std::string>& choices, InjectionHandler* injection, std::function<void(const std::string&)> ok_callback);

  private:
    void setInputTextField(const std::string& value);

  protected:
    std::array<char, 256> input_text_field_ = { 0 };
    std::string new_sequence_segment_;
    std::set<std::string> sequence_groups_;
    InjectionHandler* injection_;
    enum
    {
      EActionChoice_MoveSegment,
      EActionChoice_CreateSegment,
    };
    int action_choice_ = EActionChoice_MoveSegment;
    std::function<void(const std::string&)> ok_callback_;
    std::string move_option_message_;
    std::string create_option_message_;
    std::string move_action_message_;
    std::string create_action_message_;
  };

}
