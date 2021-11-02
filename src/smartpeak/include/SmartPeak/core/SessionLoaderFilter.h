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

#pragma once

#include <SmartPeak/iface/IApplicationProcessorObserver.h>
#include <SmartPeak/iface/IPropertiesHandler.h>

namespace SmartPeak
{
  /**
    This class records the executed workflow in order to be able to
    construct a workflow capable of restoring the state.
    the constructed workflow will be recorded in the session file and executed 
    while the session is loaded.
  */
  struct SessionLoaderFilter : IPropertiesHandler, IApplicationProcessorObserver
  {
    SessionLoaderFilter() { };

    /**
    IApplicationProcessorObserver
    */
    virtual void onApplicationProcessorStart(const std::vector<std::string>& commands) override;
    virtual void onApplicationProcessorCommandStart(size_t command_index, const std::string& command_name) override {};
    virtual void onApplicationProcessorCommandEnd(size_t command_index, const std::string& command_name) override {};
    virtual void onApplicationProcessorEnd() override {};
    virtual void onApplicationProcessorError(const std::string& error) override {};

    /**
    IPropertiesHandler
    */
    virtual std::string getPropertiesHandlerName() const override;
    virtual std::map<std::string, CastValue::Type> getPropertiesSchema() const override;
    virtual std::optional<CastValue> getProperty(const std::string& property, const size_t row) const override;
    virtual void setProperty(const std::string& property, const CastValue& value, const size_t row) override;

    void clear() { loading_workflow_commands_.clear(); }

    std::vector<std::string> getLoadingWorkflowCommands() const;

  protected:
    std::map<int, std::string> loading_workflow_commands_;
  };
}
