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

#include <SmartPeak/core/Filenames.h>
#include <string>
#include <map>
#include <iostream>
#include <string>

namespace SmartPeak
{
  void Filenames::setFileVariants(
    const std::string& input_mzML_filename,
    const std::string& input_inj_name,
    const std::string& output_inj_name,
    const std::string& input_sample_name,
    const std::string& output_sample_name
  )
  {
    updateFileVariant(input_mzML_filename, FileScope::EFileScopeMzMLInput);
    updateFileVariant(input_inj_name, FileScope::EFileScopeInjectionInput);
    updateFileVariant(input_sample_name, FileScope::EFileScopeSampleGroupInput);
    updateFileVariant(output_sample_name, FileScope::EFileScopeSampleGroupOutput);
    updateFileVariant(output_inj_name, FileScope::EFileScopeInjectionOutput);
  }

  void Filenames::setRootPaths(
    const std::string& main_dir,
    const std::string& mzml_input_path,
    const std::string& input_path,
    const std::string& output_path
  )
  {
    mzml_input_path_ = mzml_input_path;
    input_path_ = input_path;
    output_path_ = output_path;
    main_dir_ = main_dir;
    updateRootPath(main_dir, FileScope::EFileScopeMain);
    updateRootPath(mzml_input_path, FileScope::EFileScopeMzMLInput);
    updateRootPath(input_path, FileScope::EFileScopeInjectionInput);
    updateRootPath(input_path, FileScope::EFileScopeSampleGroupInput);
    updateRootPath(output_path, FileScope::EFileScopeSampleGroupOutput);
    updateRootPath(output_path, FileScope::EFileScopeInjectionOutput);
  }

  void Filenames::addFileName(const std::string& id, const std::string& default_name, FileScope file_scope)
  {
    if (file_names_.find(id) == file_names_.end())
    {
      FileName f{ default_name , file_scope };
      updateFullPathName(f);
      file_names_.insert_or_assign(id, f);
    }
  }

  std::string Filenames::getFullPathName(const std::string& id) const
  {
    return file_names_.at(id).full_path_.generic_string();
  }

  void Filenames::updateRootPath(const std::string& dir, FileScope file_scope)
  {
    for (auto& f : file_names_)
    {
      if (f.second.file_scope_ == file_scope)
      {
        f.second.root_path_ = dir;
        updateFullPathName(f.second);
      }
    }
  }

  void Filenames::updateFileVariant(const std::string& variant, FileScope file_scope)
  {
    for (auto& f : file_names_)
    {
      if (f.second.file_scope_ == file_scope)
      {
        f.second.file_variant_ = variant;
        updateFullPathName(f.second);
      }
    }
  }

  void Filenames::setFullPathName(const std::string& id, const std::string& full_path)
  {
    if (file_names_.find(id) == file_names_.end())
    {
      addFileName(id, "", FileScope::EFileScopeUnspecified);
    }
    file_names_.at(id).full_path_override_ = true;
    file_names_.at(id).full_path_ = full_path;
  }

  void Filenames::updateFullPathName(FileName& filename)
  {
    if (!filename.full_path_override_)
    {
      filename.full_path_.clear();
      if (!filename.root_path_.empty())
      {
        filename.full_path_ = filename.root_path_;
      }
      if (!filename.file_variant_.empty())
      {
        filename.full_path_ /= filename.file_variant_;
      }
      filename.full_path_ += filename.default_name_;
    }
  }

  void Filenames::merge(const Filenames& other)
  {
    for (const auto& p : other.file_names_)
      file_names_.emplace(p.first, p.second);
  }

}
