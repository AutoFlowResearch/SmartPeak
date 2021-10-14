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
#include <iostream>
#include <chrono>
#include <algorithm>
#include <plog/Log.h>

#include <SmartPeak/core/ApplicationHandler.h>
#include <SmartPeak/core/ApplicationProcessor.h>
#include <SmartPeak/core/WorkflowManager.h>
#include <SmartPeak/core/SessionHandler.h>
#include <SmartPeak/core/FeatureMetadata.h>
#include <SmartPeak/core/SharedProcessors.h>
#include <SmartPeak/io/SequenceParser.h>
#include <SmartPeak/io/InputDataValidation.h>

#include <SmartPeak/cli/ApplicationManager.h>


namespace SmartPeak {
namespace cli {

/**
 * The base class Task, defines an interface for command line tasks.
 */
class Task
{
public:
    Task() = default;
    virtual ~Task() = default;

public:
    /**
     * Executes the task. 
     * @param application_manager reference to ApplicationManager instance
     */
    virtual bool operator() (ApplicationManager& application_manager) = 0;
};

class InitializeApplicationSettings : public Task
{
public:
    virtual bool operator() (ApplicationManager& application_manager) override;
};

class InitializeLogger : public Task
{
public:
    virtual bool operator() (ApplicationManager& application_manager) override;
};

class LoadSession : public Task
{
public:
    virtual bool operator() (ApplicationManager& application_manager) override;
};

class RunIntegrityChecks : public Task
{
public:
    virtual bool operator() (ApplicationManager& application_manager) override;

private:
    void handle_integrity_check(bool check, bool allow_inconsistent, std::string msg);

    bool run_integrity_check(
        const SmartPeak::SequenceHandler& sequence_handler, 
        std::function<bool(const SmartPeak::SequenceHandler&)> f,
        const std::string& info_msg = "Run integrity check");
};

class InitializeWorkflowResources : public Task
{
public:
    virtual bool operator() (ApplicationManager& application_manager) override;
};

class InitializeWorkflowSettings : public Task
{
public:
    virtual bool operator() (ApplicationManager& application_manager) override;

private:
    void _update_filenames(
        SmartPeak::BuildCommandsFromNames& buildCommandsFromNames,
        SmartPeak::ApplicationHandler& application_handler);
};

class RunWorkflow : public Task, 
    public IApplicationProcessorObserver,
    public ISequenceProcessorObserver,
    public ISequenceSegmentProcessorObserver,
    public ISampleGroupProcessorObserver
{
public:
    virtual bool operator() (ApplicationManager& application_manager) override;

    /**
      IApplicationProcessorObserver
    */
    virtual void onApplicationProcessorStart(const std::vector<std::string>& commands) override {}
    virtual void onApplicationProcessorCommandStart(size_t command_index, const std::string& command_name) override {}
    virtual void onApplicationProcessorCommandEnd(size_t command_index, const std::string& command_name) override {}
    virtual void onApplicationProcessorEnd() override {}
    virtual void onApplicationProcessorError(const std::string& error) override {}

    /**
      ISequenceProcessorObserver
    */
    virtual void onSequenceProcessorStart(const size_t nb_injections) override {}
    virtual void onSequenceProcessorSampleStart(const std::string& sample) override 
    { 
        m_event_type = 0; 
        m_event_name = sample;
    }
    virtual void onSequenceProcessorSampleEnd(const std::string& sample) override {}
    virtual void onSequenceProcessorEnd() override {}
    virtual void onSequenceProcessorError(
      const std::string& sample_name,
      const std::string& processor_name,
      const std::string& error) override {};

    /**
      ISequenceSegmentProcessorObserver
    */
    virtual void onSequenceSegmentProcessorStart(const size_t nb_segments) override {}
    virtual void onSequenceSegmentProcessorSampleStart(const std::string& segment_name) override 
    { 
        m_event_type = 1; 
        m_event_name = segment_name;
    }
    virtual void onSequenceSegmentProcessorSampleEnd(const std::string& segment_name) override {}
    virtual void onSequenceSegmentProcessorEnd() override {}
    virtual void onSequenceSegmentProcessorError(const std::string& segment_name, const std::string& processor_name, const std::string& error) override {};

    /**
      ISampleGroupProcessorObserver
    */
    virtual void onSampleGroupProcessorStart(const size_t nb_groups) override {}
    virtual void onSampleGroupProcessorSampleStart(const std::string& group_name) override
    { 
        m_event_type = 2; 
        m_event_name = group_name;
    }
    virtual void onSampleGroupProcessorSampleEnd(const std::string& group_name) override {}
    virtual void onSampleGroupProcessorEnd() override {}
    virtual void onSampleGroupProcessorError(const std::string& group_name, const std::string& processor_name, const std::string& error) override {};

private:
    std::string formatted_time(const std::chrono::steady_clock::duration& duration) const;
    void show_progress(const ProgressInfo& progress_info, int bar_width=50) const;

private:
    int m_event_type;
    std::string m_event_name;
};

class ExportReport : public Task
{
public:
    virtual bool operator() (ApplicationManager& application_manager) override;

private:
    void _extract_report_sampletypes(
        const ApplicationSettings& application_settings, 
        std::set<SampleType>& report_sample_types);

    void _extract_report_metadata(
        const ApplicationSettings& application_settings, 
        std::vector<FeatureMetadata>& report_metadata);
};

} /* namespace cli */
} /* namespace SmartPeak */