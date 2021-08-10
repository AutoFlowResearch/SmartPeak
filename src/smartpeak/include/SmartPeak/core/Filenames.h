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

    /**
      @brief sets the file name vairants: injection name, group names ...
    */
    void setFileVariants(
      const std::string& input_mzML_filename,
      const std::string& input_inj_name,
      const std::string& output_inj_name,
      const std::string& input_sample_name,
      const std::string& output_sample_name
    );

    /**
      @brief sets the directories paths
    */
    void setRootPaths(
      const std::filesystem::path& main_dir,
      const std::filesystem::path& mzml_input_path,
      const std::filesystem::path& features_input_path,
      const std::filesystem::path& output_path
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

    /**
      @brief Adds file to the Filename
    */
    void addFileName(const std::string& id, const std::string& default_name, FileScope file_scope);

    /**
      @brief Returns the full path, with root path and variant applied (or the overridden full path).
    */
    std::string getFullPath(const std::string& id) const;

    /**
      @brief Sets the ful path name, overriding computation using variant and root path.
    */
    void setFullPath(const std::string& id, const std::filesystem::path& full_path);

    /**
      @brief Merges two Filenames. Will not overwrite file that already exists.
    */
    void merge(const Filenames& other);

    struct FileName
    {
      std::string default_name_;
      FileScope file_scope_;
      std::string file_variant_;
      std::filesystem::path root_path_;
      std::filesystem::path full_path_;
      bool full_path_override_ = false;
    };

    /**
      @brief returns detailed filenames.
    */
    const std::map<std::string, FileName>& getFileNames() const { return file_names_; };

  protected:

    friend class Filenames;

    void updateRootPath(const std::filesystem::path& dir, FileScope file_scope);
    void updateRootPaths();
    void updateFileVariants();
    void updateFileVariant(const std::string& variant, FileScope file_scope);
    void updateFullPath(FileName& filname);

    std::map<std::string, FileName> file_names_;
    std::filesystem::path main_dir_;
    std::filesystem::path mzml_input_path_;
    std::filesystem::path input_path_;
    std::filesystem::path output_path_;
    std::string input_mzML_filename_;
    std::string input_inj_name_;
    std::string output_inj_name_;
    std::string input_sample_name_;
    std::string output_sample_name_;
  };
}
