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
// $Maintainer: Douglas McCloskey, Ahmed Khalil $
// $Authors: Douglas McCloskey $
// --------------------------------------------------------------------------

#pragma once

#include <string>
#include <utility>
#include <vector>
#include <imgui.h>
#include <SmartPeak/core/SessionHandler.h>
#include <SmartPeak/ui/Widget.h>
#include <SmartPeak/ui/ImEntry.h>
#include <SmartPeak/ui/ParameterEditorWidget.h>
#include <SmartPeak/iface/IParametersObserver.h>
#include <SmartPeak/iface/IWorkflowObserver.h>
#include <unsupported/Eigen/CXX11/Tensor>

namespace SmartPeak
{

  /**
    @brief Base Parameters table
  */
  class ParametersTableWidget final : 
    public Widget, 
    public IParametersObserver, 
    public IWorkflowObserver,
    public IParameterEditorWidgetObserver
  {
  public:
    ParametersTableWidget(SessionHandler& session_handler, ApplicationHandler& application_handler, const std::string& table_id, const std::string title = "");
    ~ParametersTableWidget();

    void draw() override;

  public:
    /**
     IParametersObserver
    */
    virtual void onParametersUpdated() override;
    /**
     IWorkflowObserver
    */
    virtual void onWorkflowUpdated() override;
    /** 
      IParameterEditorWidgetObserver
    */
    virtual void onParameterSet(const std::string& function_parameter, const Parameter& parameter);
    virtual void onParameterRemoved(const std::string& function_parameter, const Parameter& parameter);

  private:
    void updateParametersTable();
    const std::vector<std::string> header_names_ = { "function", "name", "type", "value", "restrictions" };
    std::vector<ImEntry> parameters_table_entries_;
    /**
      @brief return current parameter values in the same order as the columns : headers = { "name", "type", "value", "restrictions" }
     
      @param[in] param pointer to the parameter from which the values will be returned
    */
    const std::vector<std::string> getAllValues_(const Parameter* param) {
      return { param->getName(), param->getType(), param->getValueAsString(), param->getRestrictionsAsString()}; };
  protected:
    Eigen::Tensor<std::string, 1> headers_;
    Eigen::Tensor<std::string, 2> body_;
    const std::string table_id_;
  protected:
    SessionHandler& session_handler_;
    ApplicationHandler& application_handler_;
    bool refresh_needed_ = true;
    ParameterEditorWidget parameter_editor_widget_;
    ParameterSet parameters_; // Parameter to list on the table
    bool show_default_ = true;
    bool show_unused_ = true;
    int selected_col_ = 0;
    bool is_scanned_ = false;
  };

}
