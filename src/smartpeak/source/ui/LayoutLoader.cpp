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

#include <SmartPeak/ui/LayoutLoader.h>
#include <SmartPeak/core/ApplicationProcessors/SavePropertiesHandlers.h>
#include <SmartPeak/core/ApplicationProcessors/LoadPropertiesHandlers.h>

namespace SmartPeak
{
  LayoutLoader::LayoutLoader(ApplicationHandler& application_handler) :
    application_handler_(application_handler)
  {};

  void LayoutLoader::onApplicationProcessorStart(const std::vector<std::string>& commands)
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

  void LayoutLoader::onApplicationProcessorCommandStart(size_t command_index, const std::string& command_name)
  {};

  void LayoutLoader::onApplicationProcessorCommandEnd(size_t command_index, const std::string& command_name)
  {};

  void LayoutLoader::onApplicationProcessorEnd()
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

  void LayoutLoader::onApplicationProcessorError(const std::string& error)
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

  void LayoutLoader::process()
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
}
