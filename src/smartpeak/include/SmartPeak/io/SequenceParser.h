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

#pragma once

#include <SmartPeak/core/FeatureMetadata.h>
#include <SmartPeak/core/SampleType.h>
#include <SmartPeak/core/SequenceHandler.h>
#include <SmartPeak/core/Utilities.h>
#include <SmartPeak/iface/IFilePickerHandler.h>
#include <plog/Log.h>
#include <unsupported/Eigen/CXX11/Tensor>

namespace SmartPeak
{
  class SequenceParser
  {
public:
    SequenceParser()                                 = delete;
    ~SequenceParser()                                = delete;
    SequenceParser(const SequenceParser&)            = delete;
    SequenceParser& operator=(const SequenceParser&) = delete;
    SequenceParser(SequenceParser&&)                 = delete;
    SequenceParser& operator=(SequenceParser&&)      = delete;

    static constexpr char const * const s_PeptideRef {"PeptideRef"};
    static constexpr char const * const s_native_id {"native_id"};

    static void readSequenceFile(
      SequenceHandler& sequenceHandler,
      const std::filesystem::path& pathname,
      const std::string& delimiter
    );

    static void makeSequenceFileSmartPeak(
      SequenceHandler& sequenceHandler,
      std::vector<std::vector<std::string>>& rows_out,
      std::vector<std::string>& headers_out
    );

    static void writeSequenceFileSmartPeak(
      SequenceHandler& sequenceHandler,
      const std::filesystem::path& filename,
      const std::string& delimiter = ","
    );

    static void makeSequenceFileAnalyst(
      SequenceHandler& sequenceHandler,
      std::vector<std::vector<std::string>>& rows_out,
      std::vector<std::string>& headers_out
    );

    static void writeSequenceFileAnalyst(
      SequenceHandler& sequenceHandler,
      const std::filesystem::path& filename,
      const std::string& delimiter = "\t"
    );

    static void makeSequenceFileMasshunter(
      SequenceHandler& sequenceHandler,
      std::vector<std::vector<std::string>>& rows_out,
      std::vector<std::string>& headers_out
    );

    static void writeSequenceFileMasshunter(
      SequenceHandler& sequenceHandler,
      const std::filesystem::path& filename,
      const std::string& delimiter = "\t"
    );

    static void makeSequenceFileXcalibur(
      SequenceHandler& sequenceHandler,
      std::vector<std::vector<std::string>>& rows_out,
      std::vector<std::string>& headers_out
    );

    static void writeSequenceFileXcalibur(
      SequenceHandler& sequenceHandler,
      const std::filesystem::path& filename,
      const std::string& delimiter = "\t"
    );

    /*
    @brief make a table (row major) of string representations of
      all meta_data for all sample_types in the feature history.

    NOTE: Internally, to_string() rounds at 1e-6. Therefore, some precision might be lost.
    */
    static void makeDataTableFromMetaValue(
      const SequenceHandler& sequenceHandler,
      std::vector<std::vector<std::string>>& rows_out,
      std::vector<std::string>& headers_out,
      const std::vector<std::string>& meta_data,
      const std::set<SampleType>& sample_types,
      const std::set<std::string>& sample_names,
      const std::set<std::string>& component_group_names,
      const std::set<std::string>& component_names
    );

    static bool writeDataTableFromMetaValue(
      const SequenceHandler& sequenceHandler,
      const std::filesystem::path& filename,
      const std::vector<FeatureMetadata>& meta_data,
      const std::set<SampleType>& sample_types
    );

    static void makeGroupDataTableFromMetaValue(
      const SequenceHandler& sequenceHandler,
      std::vector<std::vector<std::string>>& rows_out,
      std::vector<std::string>& headers_out,
      const std::vector<std::string>& meta_data,
      const std::set<SampleType>& sample_types,
      const std::set<std::string>& sample_names,
      const std::set<std::string>& component_group_names,
      const std::set<std::string>& component_names
    );

    static bool writeGroupDataTableFromMetaValue(
      const SequenceHandler& sequenceHandler,
      const std::filesystem::path& filename,
      const std::vector<FeatureMetadata>& meta_data,
      const std::set<SampleType>& sample_types
    );

    static void makeDataMatrixFromMetaValue(
      const SequenceHandler& sequenceHandler,
      Eigen::Tensor<float,2>& data_out,
      Eigen::Tensor<std::string, 1>& columns_out,
      Eigen::Tensor<std::string, 2>& rows_out,
      const std::vector<std::string>& meta_data,
      const std::set<SampleType>& sample_types,
      const std::set<std::string>& sample_names,
      const std::set<std::string>& component_group_names,
      const std::set<std::string>& component_names
    );

    // NOTE: Internally, to_string() rounds at 1e-6. Therefore, some precision might be lost.
    static bool writeDataMatrixFromMetaValue(
      const SequenceHandler& sequenceHandler,
      const std::filesystem::path& filename,
      const std::vector<FeatureMetadata>& meta_data,
      const std::set<SampleType>& sample_types
    );

    static void makeGroupDataMatrixFromMetaValue(
      const SequenceHandler& sequenceHandler,
      Eigen::Tensor<float, 2>& data_out,
      Eigen::Tensor<std::string, 1>& columns_out,
      Eigen::Tensor<std::string, 2>& rows_out,
      const std::vector<std::string>& meta_data,
      const std::set<SampleType>& sample_types,
      const std::set<std::string>& sample_names,
      const std::set<std::string>& component_group_names,
      const std::set<std::string>& component_names
    );

    static bool writeGroupDataMatrixFromMetaValue(
      const SequenceHandler& sequenceHandler,
      const std::filesystem::path& filename,
      const std::vector<FeatureMetadata>& meta_data,
      const std::set<SampleType>& sample_types
    );

    private:
      template<typename delimiter>
      static void readSequenceFile(SequenceHandler& sequenceHandler, const std::filesystem::path& pathname);
  };

  struct StoreSequenceFileAnalyst : IFilePickerHandler, IFilenamesHandler {

    /* IFilePickerHandler */
    bool onFilePicked(const std::filesystem::path& filename, ApplicationHandler* application_handler) override;

    /* IFilenamesHandler */
    virtual void getFilenames(Filenames& filenames) const override;
  };

  struct StoreSequenceFileMasshunter : IFilePickerHandler, IFilenamesHandler {

    /* IFilePickerHandler */
    bool onFilePicked(const std::filesystem::path& filename, ApplicationHandler* application_handler) override;

    /* IFilenamesHandler */
    virtual void getFilenames(Filenames& filenames) const override;
  };

  struct StoreSequenceFileXcalibur : IFilePickerHandler, IFilenamesHandler {

    /* IFilePickerHandler */
    bool onFilePicked(const std::filesystem::path& filename, ApplicationHandler* application_handler) override;

    /* IFilenamesHandler */
    virtual void getFilenames(Filenames& filenames) const override;
  };
}
