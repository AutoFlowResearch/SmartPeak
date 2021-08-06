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
// $Authors: Douglas McCloskey, Pasquale Domenico Colaianni $
// --------------------------------------------------------------------------

#pragma once

#include <string>
#include <filesystem>
#include <map>

namespace SmartPeak
{
  class Filenames
  {
public:

    void setFileVariants(
      const std::string& input_mzML_filename,
      const std::string& input_inj_name,
      const std::string& output_inj_name,
      const std::string& input_sample_name,
      const std::string& output_sample_name
    );

    void setRootPaths(
      const std::string& main_dir,
      const std::string& mzml_input_path,
      const std::string& features_input_path,
      const std::string& output_path
    );

    enum class FileScope
    {
      EFileScopeMain,
      EFileScopeMzMLInput,
      EFileScopeInjectionOutput,
      EFileScopeInjectionInput,
      EFileScopeSampleGroupInput,
      EFileScopeSampleGroupOutput,
      EFileScopeUnspecified
    };

    void addFileName(const std::string& id, const std::string& default_name, FileScope file_scope);
    std::string getFullPathName(const std::string& id) const;
    void setFullPathName(const std::string& id, const std::string& full_path);
    void merge(const Filenames& other);

  protected:

    friend class Filenames;

    struct FileName
    {
      std::string default_name_;
      FileScope file_scope_;
      std::string file_variant_;
      std::string root_path_;
      std::string full_path_;
      bool full_path_override_ = false;
    };

    void updateRootPath(const std::string& dir, FileScope file_scope);
    void updateFileVariant(const std::string& variant, FileScope file_scope);
    void updateFullPathName(FileName& filname);

    std::map<std::string, FileName> file_names_;
    std::string main_dir_;
    std::string mzml_input_path_;
    std::string input_path_;
    std::string output_path_;
  };
}
