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
#include <vector>

namespace SmartPeak
{
  class Filenames
  {
public:

  /**
      @brief Adds file to the Filename
    */
    void addFileName(const std::string& id, const std::string& name_pattern);

    /**
      @brief Returns the full path, with root path and variant applied (or the overridden full path).
    */
    std::filesystem::path getFullPath(const std::string& id) const;

    /**
      @brief Sets the ful path name, overriding computation using variant and root path.
    */
    void setFullPath(const std::string& id, const std::filesystem::path& full_path);

    /**
      @brief Merges two Filenames. Will not overwrite file that already exists.
    */
    void merge(const Filenames& other);

    /**
      @brief returns registered file ids.
    */
    std::vector<std::string> getFileIds() const;

    /**
      @brief set tags and update paths.
    */
    void setTag(const std::string& tag_id, const std::string& value);

  protected:

    struct FileName
    {
      std::string name_pattern_;
      std::filesystem::path full_path_;
      bool full_path_override_ = false;
    };

    friend class Filenames;

    void updateFullPaths();
    void updateFullPath(FileName& filename);

    std::map<std::string, FileName> file_names_;
    std::map<std::string, std::string> tags_;
  };
}
