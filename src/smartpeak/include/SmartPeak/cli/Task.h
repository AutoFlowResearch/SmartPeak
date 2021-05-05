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

class RunWorkflow : public Task
{
public:
    virtual bool operator() (ApplicationManager& application_manager) override;
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