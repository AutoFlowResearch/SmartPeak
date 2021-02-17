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
// $Authors: Douglas McCloskey, Pasquale Domenico Colaianni $
// --------------------------------------------------------------------------

#include <SmartPeak/core/WorkflowManager.h>
#include <SmartPeak/core/ApplicationHandler.h>
#include <SmartPeak/core/ApplicationProcessor.h>
#include <thread>
#include <future>

namespace SmartPeak {
  void WorkflowManager::addWorkflow(ApplicationHandler& source_app_handler, const std::set<std::string>& injection_names, const std::set<std::string>& sequence_segment_names, const std::set<std::string>& sample_group_names, const std::vector<ApplicationHandler::Command>& commands)
  {
    // do not run workflows concurrently
    if (!done_) {
      return;
    }
    application_handler_ = source_app_handler;
    done_ = false;

    // making local copies
    const std::set<std::string> injection_names_(injection_names);
    const std::set<std::string> sequence_segment_names_(sequence_segment_names);
    const std::set<std::string> sample_group_names_(sample_group_names);
    const std::vector<ApplicationHandler::Command> commands_(commands);

    std::thread t(run_and_join, std::ref(application_handler_), std::ref(done_), injection_names_, sequence_segment_names_, sample_group_names_, commands_);
    LOGD << "Created thread (to be detached): " << t.get_id();
    t.detach();
    LOGD << "Thread has been detached";
  }

  bool WorkflowManager::isWorkflowDone() const
  {
    return done_;
  }

  void WorkflowManager::run_and_join(ApplicationHandler& application_handler, bool& done, const std::set<std::string>& injection_names, const std::set<std::string>& sequence_segment_names, const std::set<std::string>& sample_group_names, const std::vector<ApplicationHandler::Command>& commands)
  {
    // run workflow asynchronously
    std::future<void> f = std::async(
      std::launch::async,
      ApplicationProcessors::processCommands,
      std::ref(application_handler),
      std::cref(commands),
      std::cref(injection_names), std::cref(sequence_segment_names), std::cref(sample_group_names)
    );

    LOGD << "Waiting on async operation...";
    f.wait();
    LOGD << "Async operation completed";

    try {
      f.get(); // checking for exception
    } catch (const std::exception& e) {
      LOGE << e.what();
    }

    done = true;
    LOGI << "State updated with workflow's results";
  }

  void WorkflowManager::updateApplicationHandler(ApplicationHandler& source_app_handler)
  {
    // update the status for this workflow, to be used in the gui
    source_app_handler = std::move(application_handler_);
  }
}
