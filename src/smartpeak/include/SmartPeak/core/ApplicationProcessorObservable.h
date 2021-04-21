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
// $Maintainer: Bertrand Boudaud $
// $Authors: Douglas McCloskey, Bertrand Boudaud $
// --------------------------------------------------------------------------

#pragma once

#include <SmartPeak/iface/IApplicationProcessorObserver.h>
#include <memory>
#include <vector>
#include <algorithm>

namespace SmartPeak
{
  class ApplicationProcessorObservable
  {
  public:
    virtual void addApplicationProcessorObserver(IApplicationProcessorObserver* observer) { observers_.push_back(observer); };
    virtual void removeApplicationProcessorObserver(IApplicationProcessorObserver* observer) { observers_.erase(std::remove(observers_.begin(), observers_.end(), observer), observers_.end()); };
    void notifyApplicationProcessorStart(const size_t nb_commands)
    {
      for (auto& observer : observers_)
      {
        observer->onApplicationProcessorStart(nb_commands);
      }
    }
    void notifyApplicationProcessorCommandStart(size_t command_index, const std::string& command_name)
    {
      for (auto& observer : observers_)
      {
        observer->onApplicationProcessorCommandStart(command_index, command_name);
      }
    }
    void notifyApplicationProcessorCommandEnd(size_t command_index, const std::string& command_name)
    {
      for (auto& observer : observers_)
      {
        observer->onApplicationProcessorCommandEnd(command_index, command_name);
      }
    }
    void notifyApplicationProcessorEnd()
    {
      for (auto& observer : observers_)
      {
        observer->onApplicationProcessorEnd();
      }
    }
  protected:
    std::vector<IApplicationProcessorObserver*> observers_;
  };
}
