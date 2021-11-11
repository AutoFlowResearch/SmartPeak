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

#include <SmartPeak/core/ApplicationHandler.h>
#include <SmartPeak/iface/IApplicationProcessorObserver.h>

namespace SmartPeak
{
  /**
    This class aims at loading and saving ui layout.
    As UI must be constructed before to load the layout, It will detect that a session has been loaded to
    then load the layout from the session file.
    The saving is actually not an issue as the UI is already constructed,
    but for consistency and simplicity, this class also handle it the same way.
  */
  struct LayoutLoader : IApplicationProcessorObserver
  {
    LayoutLoader(ApplicationHandler& application_handler);

    /**
    IApplicationProcessorObserver
    */
    virtual void onApplicationProcessorStart(const std::vector<std::string>& commands) override;
    virtual void onApplicationProcessorCommandStart(size_t command_index, const std::string& command_name) override;
    virtual void onApplicationProcessorCommandEnd(size_t command_index, const std::string& command_name) override;
    virtual void onApplicationProcessorEnd() override;
    virtual void onApplicationProcessorError(const std::string& error) override;

    /**
    * @brief will load / save the layout if the session has just been loaded (to call once ui is ready to be setup).
    */
    void process();

    std::vector<IPropertiesHandler*> properties_handlers_;
  
  protected:
    bool session_loaded_ = false;
    bool loading_session_ = false;
    bool session_saved_ = false;
    bool saving_session_ = false;
    ApplicationHandler& application_handler_;
  };
}
