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
// $Maintainer: Douglas McCloskey, Bertrand Boudaud $
// $Authors: Douglas McCloskey $
// --------------------------------------------------------------------------

#include <gtest/gtest.h>
#include <SmartPeak/test_config.h>
#include <SmartPeak/io/SessionDB.h>
#include <SmartPeak/core/SessionLoaderGenerator.h>

using namespace SmartPeak;
using namespace std;

TEST(SessionLoaderGenerator, getLoadingWorkflowCommands)
{
  SessionLoaderGenerator session_loader_generator;
  EXPECT_TRUE(session_loader_generator.getLoadingWorkflowCommands().empty());

  session_loader_generator.onApplicationProcessorStart(
    {
    "LOAD_RAW_DATA",
    "MAP_CHROMATOGRAMS",
    "EXTRACT_CHROMATOGRAM_WINDOWS",
    "ZERO_CHROMATOGRAM_BASELINE",
    "PICK_MRM_FEATURES",
    "QUANTIFY_FEATURES",
    "CHECK_FEATURES",
    "SELECT_FEATURES",
    "STORE_FEATURES"
    }
  );
  session_loader_generator.onApplicationProcessorEnd();

  std::vector<std::string> expected_loading_workflow =
  {
    "LOAD_RAW_DATA",
    "LOAD_FEATURES",
    "MAP_CHROMATOGRAMS",
    "EXTRACT_CHROMATOGRAM_WINDOWS",
    "ZERO_CHROMATOGRAM_BASELINE"
  };

  EXPECT_EQ(session_loader_generator.getLoadingWorkflowCommands(), expected_loading_workflow);
}

TEST(SessionLoaderGenerator, WriteAndReadSessionLoaderFilter)
{
  SessionDB session_db;
  auto path_db = std::tmpnam(nullptr);
  session_db.setDBFilePath(path_db);

  SessionLoaderGenerator session_loader_write_filter;
  session_loader_write_filter.onApplicationProcessorStart(
    {
    "LOAD_RAW_DATA",
    "MAP_CHROMATOGRAMS",
    "EXTRACT_CHROMATOGRAM_WINDOWS",
    "ZERO_CHROMATOGRAM_BASELINE",
    "PICK_MRM_FEATURES",
    "QUANTIFY_FEATURES",
    "CHECK_FEATURES",
    "SELECT_FEATURES",
    "STORE_FEATURES"
    }
  );
  session_loader_write_filter.onApplicationProcessorEnd();
  session_db.writePropertiesHandler(session_loader_write_filter);

  SessionLoaderGenerator session_loader_read_filter;
  session_db.readPropertiesHandler(session_loader_read_filter);

  std::vector<std::string> expected_loading_workflow =
  {
    "LOAD_RAW_DATA",
    "LOAD_FEATURES",
    "MAP_CHROMATOGRAMS",
    "EXTRACT_CHROMATOGRAM_WINDOWS",
    "ZERO_CHROMATOGRAM_BASELINE"
  };

  EXPECT_EQ(session_loader_read_filter.getLoadingWorkflowCommands(), expected_loading_workflow);
}
