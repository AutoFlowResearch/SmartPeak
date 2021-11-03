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
#include <SmartPeak/core/ApplicationProcessor.h>


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
    LayoutLoader(ApplicationHandler& application_handler) :
      application_handler_(application_handler)
    { };

    /**
    IApplicationProcessorObserver
    */
    virtual void onApplicationProcessorStart(const std::vector<std::string>& commands) override
    {
      if ((commands.size() == 1) && (commands.at(0) == "LOAD_SESSION"))
      {
        loading_session_ = true;
        session_loaded_ = false;
      }
      else if ((commands.size() == 1) && (commands.at(0) == "SAVE_SESSION"))
      {
        saving_session_ = true;
        session_saved_ = false;
      }
    };
    virtual void onApplicationProcessorCommandStart(size_t command_index, const std::string& command_name) override { };
    virtual void onApplicationProcessorCommandEnd(size_t command_index, const std::string& command_name) override { };
    virtual void onApplicationProcessorEnd() override 
    {
      if (loading_session_)
      {
        loading_session_ = false;
        session_loaded_ = true;
      }
      if (saving_session_)
      {
        saving_session_ = false;
        session_saved_ = true;
      }
    };
    virtual void onApplicationProcessorError(const std::string& error) override
    {
      if (loading_session_)
      {
        loading_session_ = false;
        session_loaded_ = false;
      }
      if (saving_session_)
      {
        saving_session_ = false;
        session_saved_ = false;
      }
    };

    /**
    * @brief will load / save the layout if the session has just been loaded (to call once ui is ready to be setup).
    */
    void process()
    {
      if (session_loaded_)
      {
        if (!application_handler_.filenames_.getSessionDB().getDBFilePath().empty())
        {
          LoadPropertiesHandlers load_layout(application_handler_);
          load_layout.properties_handlers = properties_handlers_;
          load_layout.process();
        }
        session_loaded_ = false;
      }
      if (session_saved_)
      {
        if (!application_handler_.filenames_.getSessionDB().getDBFilePath().empty())
        {
          SavePropertiesHandlers save_properties_handlers(application_handler_);
          save_properties_handlers.properties_handlers = properties_handlers_;
          save_properties_handlers.process();
        }
        session_saved_ = false;
      }
    }

    std::vector<IPropertiesHandler*> properties_handlers_;

  protected:
    bool session_loaded_ = false;
    bool loading_session_ = false;
    bool session_saved_ = false;
    bool saving_session_ = false;
    ApplicationHandler& application_handler_;
  };
}
