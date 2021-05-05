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
#include <iostream>
#include <chrono>
#include <algorithm>
#include <plog/Log.h>

#include <SmartPeak/cli/Parser.h>
#include <SmartPeak/cli/ApplicationSettings.h>
#include <SmartPeak/cli/ApplicationManager.h>
#include <SmartPeak/cli/Task.h>


int main(int argc, char** argv) 
{
    namespace cli = SmartPeak::cli;

    auto parser = cli::Parser{argc, argv};
    auto application_settings = cli::ApplicationSettings{parser};
    auto application = cli::ApplicationManager{application_settings};
    try
    {
        application
            .add(std::make_shared<cli::InitializeApplicationSettings>())
            .add(std::make_shared<cli::InitializeLogger>())
            .add(std::make_shared<cli::LoadSession>())
            .add(std::make_shared<cli::InitializeWorkflowResources>())
            .add(std::make_shared<cli::InitializeWorkflowSettings>())
            .add(std::make_shared<cli::RunIntegrityChecks>())
            .add(std::make_shared<cli::RunWorkflow>())
            .add(std::make_shared<cli::ExportReport>());

        return !application.run();
    }
    catch(const std::exception& e)
    {
        LOG_ERROR << e.what();
        return 1;
    }
    return 0;
}