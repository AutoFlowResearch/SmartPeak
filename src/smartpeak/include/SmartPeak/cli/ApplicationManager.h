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
// $Maintainer: Krzysztof Abram $
// $Authors: Douglas McCloskey, Pasquale Domenico Colaianni $
// --------------------------------------------------------------------------
#pragma once
#include <algorithm>
#include <memory>

#include <SmartPeak/core/ConsoleHandler.h>
#include <SmartPeak/core/ApplicationHandler.h>
#include <SmartPeak/core/SessionHandler.h>
#include <SmartPeak/core/WorkflowManager.h>
#include <SmartPeak/cli/ApplicationSettings.h>


namespace SmartPeak {
namespace cli {

class Task;

/**
 * Command line application manager, stores and initializes dependencies.
 * Defines and executes the control flow of the application.
 * The flow is defined as a sequence of Tasks, executed one by one, 
 * the whole pipeline fails if one task fails.
*/
class ApplicationManager
{
public:
    /**
     * Constructs the ApplicationManager object.
     * @param application_settings reference to ApplicationSettings instance
     */
    explicit ApplicationManager(ApplicationSettings& application_settings) 
        : m_application_settings{application_settings} {}

    virtual ~ApplicationManager() = default;

public:
    /* get/set */
    ApplicationSettings& get_application_settings() { return m_application_settings; }
    const ApplicationSettings& get_application_settings() const { return m_application_settings; }

    ApplicationHandler& get_application_handler() { return m_application_handler; }
    const ApplicationHandler& get_application_handler() const { return m_application_handler; }

    SessionHandler& get_session_handler() { return m_session_handler; }
    const SessionHandler& get_session_handler() const { return m_session_handler; }

    WorkflowManager& get_workflow_manager() { return m_workflow_manager; }
    const WorkflowManager& get_workflow_manager() const { return m_workflow_manager; }

    void set_workflow_commands(const std::vector<ApplicationHandler::Command>& commands) { m_commands = commands; }
    const std::vector<ApplicationHandler::Command>& get_workflow_commands() { return m_commands; }

public:
    /**
     * Appends a new task to ordered list of tasks.
     * 
     * @param task_ptr shared pointer to an instance of Task subclass
     * @return Reference to this
     */
    ApplicationManager& add(const std::shared_ptr<Task>& task_ptr);

    /**
     * Runs pipeline from task sequence.
     * 
     * @return Returns true on success, otherwise false
     */
    bool run();

    /**
     * Returns size of task list.
     */
    size_t size() { return m_task_pool.size(); }

private:
    std::vector<std::shared_ptr<Task>> m_task_pool;
    ApplicationSettings& m_application_settings;

    ApplicationHandler m_application_handler;
    SessionHandler m_session_handler;
    WorkflowManager m_workflow_manager;

    std::vector<ApplicationHandler::Command> m_commands;
};

} /* namespace cli */
} /* namespace SmartPeak */