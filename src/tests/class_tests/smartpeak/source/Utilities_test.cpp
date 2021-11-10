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
// $Maintainer: Douglas McCloskey, Ahmed Khalil $
// $Authors: Douglas McCloskey $
// --------------------------------------------------------------------------

#include <gtest/gtest.h>
#include <SmartPeak/test_config.h>
#include <filesystem>
#include <sys/stat.h>
#include <SmartPeak/core/Utilities.h>
#include <OpenMS/ANALYSIS/OPENSWATH/MRMFeatureFinderScoring.h>

using namespace SmartPeak;
using namespace std;
namespace fs = std::filesystem;

const unsigned int nb_files_in_data_directory = 68;

TEST(utilities, getCallstack)
{
  std::string path = "~/Library/Logs/DiagnosticReports";
  std::cout << "Checking files in " << path << std::endl;
  try
  {
    for (const auto& entry : fs::directory_iterator(path))
    {
      std::cout << "=============================" << std::endl;
      std::cout << entry.path() << std::endl;
      std::cout << "=============================" << std::endl;
      string line;
      ifstream myfile(entry.path());
      if (myfile.is_open())
      {
        while (getline(myfile, line))
        {
          cout << line << '\n';
        }
        myfile.close();
      }
      else cout << "Unable to open file" << std::endl;
    }
  }
  catch (const std::exception& e)
  {
    cout << e.what() << std::endl;
  }
}

