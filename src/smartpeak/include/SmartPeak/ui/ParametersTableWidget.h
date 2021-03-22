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

#pragma once

#include <string>
#include <utility>
#include <vector>
#include <imgui.h>
#include <SmartPeak/core/SessionHandler.h>
#include <SmartPeak/ui/Widget.h>
#include <SmartPeak/ui/ParameterEditorWidget.h>
#include <SmartPeak/iface/IParametersObserver.h>
#include <SmartPeak/iface/IWorkflowObserver.h>
#include <unsupported/Eigen/CXX11/Tensor>

namespace SmartPeak
{

  /**
    @brief Base Parameters table
  */
  class ParametersTableWidget : public Widget, public IParametersObserver, public IWorkflowObserver
  {
  public:
    ParametersTableWidget(SessionHandler& session_handler, ApplicationHandler& application_handler, const std::string& table_id);
    ~ParametersTableWidget();

    void draw() override;
  public:
    /**
     IParametersObserver
    */
    virtual void parametersUpdated() override;
    /**
     IWorkflowObserver
    */
    virtual void workflowUpdated() override;
  private:
    void updateParametersTable();
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
  };

}