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

#include <SmartPeak/io/SequenceParser.h>
#include <SmartPeak/core/FeatureMetadata.h>
#include <SmartPeak/core/SampleType.h>
#include <SmartPeak/core/SequenceHandler.h>
#include <SmartPeak/core/ApplicationHandler.h>
#ifndef CSV_IO_NO_THREAD
#define CSV_IO_NO_THREAD
#endif
#include <SmartPeak/io/csv.h>
#include <SmartPeak/io/CSVWriter.h>
#include <SmartPeak/io/InputDataValidation.h>
#include <ctime>

#include <plog/Log.h>

namespace SmartPeak
{

  template<typename T>
  static bool validateAndConvert(
    const std::string& s,
    T& output
  )
  {
    std::string trim_string = Utilities::trimString(s);
    if (trim_string.empty() ||
       ((trim_string.size() == 2) &&
        (trim_string[0] == 'N' || trim_string[0] == 'n') &&
        (trim_string[1] == 'A' || trim_string[1] == 'a')))
    {
      return false;
    }

    if (std::is_same<T, int>::value)
    {
      output = std::stoi(s);
    }
    else if (std::is_same<T, float>::value)
    {
      output = std::stof(s);
    }
    else
    {
      LOGE << "Case not handled";
      return false;
    }

    return true;
  }

  template<typename DELIMITER>
  void SequenceParser::readSequenceFile(
    SequenceHandler& sequenceHandler,
    const std::filesystem::path& pathname
  )
  {
    const std::string s_sample_name{ "sample_name" };
    const std::string s_sample_group_name{ "sample_group_name" };
    const std::string s_sequence_segment_name{ "sequence_segment_name" };
    const std::string s_replicate_group_name{ "replicate_group_name" };
    const std::string s_sample_type{ "sample_type" };
    const std::string s_original_filename{ "original_filename" };
    const std::string s_proc_method_name{ "proc_method_name" };
    const std::string s_rack_number{ "rack_number" };
    const std::string s_plate_number{ "plate_number" };
    const std::string s_pos_number{ "pos_number" };
    const std::string s_inj_number{ "inj_number" };
    const std::string s_dilution_factor{ "dilution_factor" };
    const std::string s_acq_method_name{ "acq_method_name" };
    const std::string s_operator_name{ "operator_name" };
    const std::string s_acquisition_date_and_time{ "acquisition_date_and_time" };
    const std::string s_inj_volume{ "inj_volume" };
    const std::string s_inj_volume_units{ "inj_volume_units" };
    const std::string s_batch_name{ "batch_name" };
    const std::string s_scan_polarity{ "scan_polarity" };
    const std::string s_scan_mass_low{ "scan_mass_low" };
    const std::string s_scan_mass_high{ "scan_mass_high" };

    io::CSVReader<21, io::trim_chars<>, DELIMITER> reader(pathname.generic_string());

    reader.read_header(
      io::ignore_extra_column | io::ignore_missing_column,
      s_sample_name,
      s_sample_group_name,
      s_sequence_segment_name,
      s_replicate_group_name,
      s_sample_type,
      s_original_filename,
      s_proc_method_name,
      s_rack_number,
      s_plate_number,
      s_pos_number,
      s_inj_number,
      s_dilution_factor,
      s_acq_method_name,
      s_operator_name,
      s_acquisition_date_and_time,
      s_inj_volume,
      s_inj_volume_units,
      s_batch_name,
      s_scan_polarity,
      s_scan_mass_low,
      s_scan_mass_high
    );

    const std::vector<std::string> mandatory_columns =
    {
        s_sample_name,
        s_sample_group_name,
        s_sequence_segment_name,
        s_sample_type,
        s_original_filename,
        s_proc_method_name,
        s_rack_number,
        s_plate_number,
        s_pos_number,
        s_inj_number,
        s_dilution_factor,
        s_acq_method_name,
        s_operator_name,
        s_acquisition_date_and_time,
        s_inj_volume,
        s_inj_volume_units,
        s_batch_name,
        s_scan_polarity,
        s_scan_mass_low,
        s_scan_mass_high
    };
    for (const auto& mandatory_column : mandatory_columns)
    {
      if (!reader.has_column(mandatory_column)) {
        LOGE << "Missing column " << mandatory_column << " in file " << pathname.generic_string();
        throw std::runtime_error("Failed loading sequence file\n");
      }
    }

    unsigned int line_number = 0;
    while (true) {
      line_number++;
      std::string current_validating_column;
      try
      {
        MetaDataHandler t; // as in temporary
        std::string t_date;
        std::string t_sample_type;
        std::string t_rack_number;
        std::string t_plate_number;
        std::string t_pos_number;
        std::string t_inj_number;
        std::string t_dilution_factor;
        std::string t_inj_volume;
        std::string t_scan_mass_low;
        std::string t_scan_mass_high;
        
        bool is_valid = reader.read_row(t.getSampleName(), t.getSampleGroupName(),
          t.getSequenceSegmentName(), t.getReplicateGroupName(), t_sample_type, t.getFilename(),
          t.proc_method_name, t_rack_number, t_plate_number, t_pos_number,
          t_inj_number, t_dilution_factor, t.acq_method_name, t.operator_name,
          t_date, t_inj_volume, t.inj_volume_units, t.batch_name,
          t.scan_polarity, t_scan_mass_low, t_scan_mass_high);

        if (!is_valid)
          break;

        if (false == validateAndConvert(t_inj_number, t.inj_number)) {
          LOGW << "Warning: Empty cell in line " << line_number << ", column '" << s_inj_number << "'. Skipping entire row";
          continue;
        }

        if (t.inj_number <= 0) {
          LOGW << "Warning: Value '" << t.inj_number << "' is not valid in line " << line_number << ", column '" << s_inj_number << "'. Skipping entire row";
          continue;
        }

        if (!(t.scan_polarity == "positive" || t.scan_polarity == "negative" || t.scan_polarity == "")) {
          LOGW << "Warning: Value '" << t.scan_polarity << "' is not valid for in line " << line_number << ", column '" << s_scan_polarity << "'. Only 'positive' and 'negative' are allowed.  Skipping entire row";
          continue;
        }

        current_validating_column = s_rack_number;
        validateAndConvert(t_rack_number, t.rack_number);
        current_validating_column = s_plate_number;
        validateAndConvert(t_plate_number, t.plate_number);
        current_validating_column = s_pos_number;
        validateAndConvert(t_pos_number, t.pos_number);
        current_validating_column = s_dilution_factor;
        validateAndConvert(t_dilution_factor, t.dilution_factor);
        current_validating_column = s_inj_volume;
        validateAndConvert(t_inj_volume, t.inj_volume);
        current_validating_column = s_scan_mass_low;
        validateAndConvert(t_scan_mass_low, t.scan_mass_low);
        current_validating_column = s_scan_mass_high;
        validateAndConvert(t_scan_mass_high, t.scan_mass_high);

        if (stringToSampleType.count(t_sample_type)) {
          t.setSampleType(stringToSampleType.at(t_sample_type));
        }
        else
        {
          t.setSampleType(SampleType::Unrecognized);
        }

        std::tm& adt = t.acquisition_date_and_time;
        std::istringstream iss(t_date);
        iss.imbue(std::locale(""));
        iss >> std::get_time(&adt, "%d-%m-%Y %H:%M:%S");
        if (adt.tm_mday < 1 || adt.tm_mday > 31) {
          LOGD << "Invalid value for std::tm::tm_mday: " << adt.tm_mday << ". Setting to 1.";
          adt.tm_mday = 1;
        }

        if (t.getFilename().empty()) {
          LOGW << "Warning: No value provided for the original filename. Will create a unique default filename.";
          t.setFilename(t.getInjectionName());
        }

        sequenceHandler.addSampleToSequence(t, OpenMS::FeatureMap());
      }
      catch (const std::exception& e)
      {
        LOGE << "Error reading " << pathname.generic_string() << " in line " << line_number << ", column '" << current_validating_column << "'";
        throw;
      }
    }
  };

  void SequenceParser::readSequenceFile(
    SequenceHandler& sequenceHandler,
    const std::filesystem::path& pathname,
    const std::string& delimiter
  )
  {
    LOGD << "START readSequenceFile";
    LOGD << "Delimiter: " << delimiter;

    LOGI << "Loading: " << pathname.generic_string();

    if (pathname.empty()) {
      LOGE << "Pathname is empty";
      LOGD << "END readSequenceFile";
      return;
    }

    if (!InputDataValidation::fileExists(pathname)) {
      LOGE << "File not found";
      LOGD << "END readSequenceFile";
      return;
    }

    const std::string s_comma{ "," };
    const std::string s_semicolon{ ";" };
    const std::string s_tab{ "\t" };

    if (delimiter == s_comma)
    {
      readSequenceFile<io::no_quote_escape<','>>(sequenceHandler, pathname);
    }
    else if (delimiter == s_semicolon)
    {
      readSequenceFile<io::no_quote_escape<';'>>(sequenceHandler, pathname);
    }
    else if (delimiter == s_tab)
    {
      readSequenceFile<io::no_quote_escape<'\t'>>(sequenceHandler, pathname);
    }
    else
    {
      throw std::invalid_argument("Delimiter \"" + delimiter + "\" is not supported.");
    }

    LOGD << "END readSequenceFile";
  }

  void SequenceParser::makeSequenceFileSmartPeak(SequenceHandler& sequenceHandler, std::vector<std::vector<std::string>>& rows_out, std::vector<std::string>& headers_out)
  {
    using namespace std::string_literals;

    // Headers expected by the SmartPeak software
    headers_out.clear();
    headers_out = {
      "sample_name",
      "sample_group_name",
      "sequence_segment_name",
      "replicate_group_name",
      "sample_type",
      "original_filename",
      "proc_method_name",
      "rack_number",
      "plate_number",
      "pos_number",
      "inj_number",
      "dilution_factor",
      "acq_method_name",
      "operator_name",
      "acquisition_date_and_time",
      "inj_volume",
      "inj_volume_units",
      "batch_name",
      "scan_polarity",
      "scan_mass_low",
      "scan_mass_high"
    };

    rows_out.clear();
    for (const InjectionHandler& sampleHandler : sequenceHandler.getSequence()) {
      std::vector<std::string> row;
      const MetaDataHandler& mdh = sampleHandler.getMetaData();
      row.push_back(mdh.getSampleName());
      row.push_back(mdh.getSampleGroupName());
      row.push_back(mdh.getSequenceSegmentName());
      row.push_back(mdh.getReplicateGroupName());
      row.push_back(mdh.getSampleTypeAsString());
      row.push_back(mdh.getFilename());
      row.push_back(mdh.proc_method_name);
      row.push_back(mdh.getRackNumberAsString(""s));
      row.push_back(mdh.getPlateNumberAsString(""s));
      row.push_back(mdh.getPosNumberAsString(""s));
      row.push_back(mdh.getInjectionNumberAsString(""s));
      row.push_back(std::to_string(mdh.dilution_factor));
      row.push_back(mdh.acq_method_name);
      row.push_back(mdh.operator_name);
      row.push_back(mdh.getAcquisitionDateAndTimeAsString());
      row.push_back(std::to_string(mdh.inj_volume));
      row.push_back(mdh.inj_volume_units);
      row.push_back(mdh.batch_name);
      row.push_back(mdh.scan_polarity);
      row.push_back(mdh.getScanMassLowAsString(""s));
      row.push_back(mdh.getScanMassHighAsString(""s));
      rows_out.push_back(row);
    }
  }

  void SequenceParser::writeSequenceFileSmartPeak(SequenceHandler& sequenceHandler,
    const std::filesystem::path& filename,
    const std::string& delimiter)
  {
    LOGD << "START writeSequenceFileSmartPeak";
    LOGD << "Delimiter: " << delimiter;

    LOGI << "Loading: " << filename.generic_string();

    if (filename.empty()) {
      LOGE << "filename is empty";
      LOGD << "END writeSequenceFileSmartPeak";
      return;
    }

    // Make the file
    std::vector<std::vector<std::string>> rows;
    //  deepcode ignore ContainerUpdatedButNeverQueried: headers are used, looks like false positive
    std::vector<std::string> headers;
    makeSequenceFileSmartPeak(sequenceHandler, rows, headers);

    // Write the output file
    CSVWriter writer(filename.generic_string(), delimiter);
    const size_t cnt = writer.writeDataInRow(headers.cbegin(), headers.cend());

    if (cnt < headers.size()) {
      LOGD << "END writeSequenceFileSmartPeak";
    }

    for (const std::vector<std::string>& line : rows) {
      writer.writeDataInRow(line.cbegin(), line.cend());
    }

    LOGD << "END writeSequenceFileSmartPeak";
  }

  void SequenceParser::makeSequenceFileAnalyst(SequenceHandler& sequenceHandler, std::vector<std::vector<std::string>>& rows_out, std::vector<std::string>& headers_out)
  {
    // Headers expected by the Analyst software
    headers_out.clear();
    headers_out = {
      "% header=SampleName","SampleID","Comments","AcqMethod",
      "ProcMethod","RackCode","PlateCode","VialPos","SmplInjVol",
      "DilutFact","WghtToVol","Type","RackPos","PlatePos","SetName",
      "OutputFile" };

    // Rows expected by the Analyst software
    rows_out.clear();
    for (const InjectionHandler& sampleHandler : sequenceHandler.getSequence()) {
      std::vector<std::string> row;
      const MetaDataHandler& mdh = sampleHandler.getMetaData();
      row.push_back(mdh.getInjectionName());
      row.push_back(mdh.getSampleName());
      row.push_back("");
      row.push_back(mdh.acq_method_name + ".dam");
      row.push_back("none");
      if (mdh.rack_number < 10) row.push_back("CStk1-0" + std::to_string(mdh.rack_number));
      else row.push_back("CStk1-" + std::to_string(mdh.rack_number));
      row.push_back("MT96_or_VT54");
      row.push_back(std::to_string(mdh.pos_number));
      row.push_back(std::to_string(mdh.inj_volume));
      row.push_back(std::to_string(mdh.dilution_factor));
      row.push_back("0");
      row.push_back(mdh.getSampleTypeAsString());
      row.push_back(std::to_string(mdh.rack_number));
      row.push_back(std::to_string(mdh.plate_number));
      row.push_back(mdh.getAcquisitionDateAndTimeAsString());
      row.push_back(mdh.getAcquisitionDateAndTimeAsString() + "\\" + mdh.getFilename());
      rows_out.push_back(row);
    }
  }

  void SequenceParser::writeSequenceFileAnalyst(SequenceHandler& sequenceHandler,
    const std::filesystem::path& filename,
    const std::string& delimiter
  )
  {
    LOGD << "START writeSequenceFileAnalyst";
    LOGD << "Delimiter: " << delimiter;

    LOGI << "Loading: " << filename.generic_string();

    if (filename.empty()) {
      LOGE << "filename is empty";
      LOGD << "END writeSequenceFileAnalyst";
      return;
    }

    // Make the file
    std::vector<std::vector<std::string>> rows;
    //  deepcode ignore ContainerUpdatedButNeverQueried: headers are used, looks like false positive
    std::vector<std::string> headers;
    makeSequenceFileAnalyst(sequenceHandler, rows, headers);

    // Write the output file
    CSVWriter writer(filename.generic_string(), delimiter);
    const size_t cnt = writer.writeDataInRow(headers.cbegin(), headers.cend());

    if (cnt < headers.size()) {
      LOGD << "END writeSequenceFileAnalyst";
    }

    for (const std::vector<std::string>& line : rows) {
      writer.writeDataInRow(line.cbegin(), line.cend());
    }

    LOGD << "END writeSequenceFileAnalyst";
  }

  void SequenceParser::makeSequenceFileMasshunter(SequenceHandler& sequenceHandler, std::vector<std::vector<std::string>>& rows_out, std::vector<std::string>& headers_out)
  {
    // Headers expected by the Agilent Masshunter software
    headers_out.clear();
    headers_out = {
      "Name","Vial","Method Path","Mathod File",
      "Data Path","Data File","Type","Level","Dil.",
      "Vol.","Tray Name","Comment" };

    // Rows expected by the Agilent Masshunter software
    rows_out.clear();
    for (const InjectionHandler& sampleHandler : sequenceHandler.getSequence()) {
      std::vector<std::string> row;
      const MetaDataHandler& mdh = sampleHandler.getMetaData();
      row.push_back(mdh.getSampleName());
      row.push_back(std::to_string(mdh.pos_number));
      row.push_back("D:\\DATA\\TODO");
      row.push_back(mdh.acq_method_name + ".M");
      row.push_back("D:\\DATA\\TODO");
      row.push_back(mdh.getAcquisitionDateAndTimeAsString() + "\\" + mdh.getFilename());
      row.push_back(mdh.getSampleTypeAsString());
      row.push_back("");
      row.push_back(std::to_string(mdh.dilution_factor));
      row.push_back(std::to_string(mdh.inj_volume));
      row.push_back("Rack " + std::to_string(mdh.rack_number));
      row.push_back("");
      rows_out.push_back(row);
    }
  }

  void SequenceParser::writeSequenceFileMasshunter(SequenceHandler& sequenceHandler,
    const std::filesystem::path& filename,
    const std::string& delimiter)
  {
    LOGD << "START writeSequenceFileMasshunter";
    LOGD << "Delimiter: " << delimiter;

    LOGI << "Loading: " << filename.generic_string();

    if (filename.empty()) {
      LOGE << "filename is empty";
      LOGD << "END writeSequenceFileMasshunter";
      return;
    }

    // Make the file
    std::vector<std::vector<std::string>> rows;
    //  deepcode ignore ContainerUpdatedButNeverQueried: headers are used, looks like false positive
    std::vector<std::string> headers;
    makeSequenceFileMasshunter(sequenceHandler, rows, headers);

    // Write the output file
    CSVWriter writer(filename.generic_string(), delimiter);
    const size_t cnt = writer.writeDataInRow(headers.cbegin(), headers.cend());

    if (cnt < headers.size()) {
      LOGD << "END writeSequenceFileMasshunter";
    }

    for (const std::vector<std::string>& line : rows) {
      writer.writeDataInRow(line.cbegin(), line.cend());
    }

    LOGD << "END writeSequenceFileMasshunter";
  }

  void SequenceParser::makeSequenceFileXcalibur(SequenceHandler& sequenceHandler, std::vector<std::vector<std::string>>& rows_out, std::vector<std::string>& headers_out)
  {
    // Headers expected by the Thermo Xcalibur software
    headers_out.clear();
    headers_out = {
      "Sample Type","File Name","Sample ID","Path",
      "Instrument Method","Process Method","Calibration File",
      "Position","Inj Vol","Level","Sample Wt","Sample Vol",
      "ISTD Amt","Dil Factor","L1 Study","L2 Client","L3 Laboratory",
      "L4 Company","L5 Phone","Comment","Sample Name" };

    // Rows expected by the Thermo Xcalibur software
    rows_out.clear();
    for (const InjectionHandler& sampleHandler : sequenceHandler.getSequence()) {
      std::vector<std::string> row;
      const MetaDataHandler& mdh = sampleHandler.getMetaData();
      row.push_back(mdh.getSampleTypeAsString());
      row.push_back(mdh.getAcquisitionDateAndTimeAsString() + "\\" + mdh.getFilename());
      row.push_back(std::to_string(mdh.inj_number));
      row.push_back("D:\\DATA\\TODO");
      row.push_back(mdh.acq_method_name);
      row.push_back("");
      row.push_back("");
      row.push_back("R:TODO" + std::to_string(mdh.pos_number));
      row.push_back(std::to_string(mdh.inj_volume));
      row.push_back("");
      row.push_back("0");
      row.push_back("0");
      row.push_back("0");
      row.push_back(std::to_string(mdh.dilution_factor));
      row.push_back("");
      row.push_back("");
      row.push_back("");
      row.push_back("");
      row.push_back("");
      row.push_back("");
      row.push_back(mdh.getSampleName());
      rows_out.push_back(row);
    }
  }

  void SequenceParser::writeSequenceFileXcalibur(SequenceHandler& sequenceHandler,
    const std::filesystem::path& filename,
    const std::string& delimiter)
  {
    LOGD << "START writeSequenceFileXcalibur";
    LOGD << "Delimiter: " << delimiter;
    LOGI << "Loading: " << filename.generic_string();

    if (filename.empty()) {
      LOGE << "filename is empty";
      LOGD << "END writeSequenceFileXcalibur";
      return;
    }

    // Make the file
    std::vector<std::vector<std::string>> rows;
    std::vector<std::string> headers;
    makeSequenceFileXcalibur(sequenceHandler, rows, headers);

    // Write the output file
    CSVWriter writer(filename.generic_string(), delimiter);
    std::vector<std::string> pre_headers;
    for (int i = 0; i < headers.size(); ++i) {
      if (i == 0) pre_headers.push_back("Bracket Type=4");
      else pre_headers.push_back("");
    }
    const size_t cnt0 = writer.writeDataInRow(pre_headers.cbegin(), pre_headers.cend());
    const size_t cnt1 = writer.writeDataInRow(headers.cbegin(), headers.cend());

    if (cnt0 < headers.size() || cnt1 < headers.size()) {
      LOGD << "END writeSequenceFileXcalibur";
    }

    for (const std::vector<std::string>& line : rows) {
      writer.writeDataInRow(line.cbegin(), line.cend());
    }

    LOGD << "END writeSequenceFileXcalibur";
  }

  void SequenceParser::makeDataTableFromMetaValue(
    const SequenceHandler& sequenceHandler,
    std::vector<std::vector<std::string>>& rows_out,
    std::vector<std::string>& headers_out,
    const std::vector<std::string>& meta_data,
    const std::set<SampleType>& sample_types,
    const std::set<std::string>& sample_names,
    const std::set<std::string>& component_group_names,
    const std::set<std::string>& component_names) {
    std::vector<std::string> headers = {
      "sample_name", "sample_type", "component_group_name", "replicate_group_name", "component_name", "batch_name",
      "rack_number", "plate_number", "pos_number", "inj_number", "dilution_factor", "inj_volume",
      "inj_volume_units", "operator_name", "acq_method_name", "proc_method_name",
      "original_filename", "acquisition_date_and_time", "scan_polarity", "scan_mass_low", "scan_mass_high", "injection_name", "used_"
    };
    headers.insert(headers.end(), meta_data.cbegin(), meta_data.cend());
    for (size_t i = 0; i < headers.size() - 1; ++i) { // checking headers are unique, stable (maintaining the same positions)
      for (size_t j = i + 1; j < headers.size(); ) {
        if (headers[i] == headers[j])
          headers.erase(headers.begin() + j);
        else
          ++j;
      }
    }
    headers_out = headers;

    const std::string delimiter {"_____"};

    rows_out.clear();
    for (const InjectionHandler& sampleHandler : sequenceHandler.getSequence()) {
      const MetaDataHandler& mdh = sampleHandler.getMetaData();
      if (sample_types.count(mdh.getSampleType()) == 0)
        continue;
      if (sample_names.size() > 0 && sample_names.count(mdh.getSampleName()) == 0)
        continue;

      // feature_map_history_ is needed in order to export all "used_" = true and false features
      for (const OpenMS::Feature& feature : sampleHandler.getRawData().getFeatureMapHistory()) {
        if (!feature.metaValueExists(s_PeptideRef) || feature.getMetaValue(s_PeptideRef).isEmpty()) {
          LOGV << "component_group_name is absent or empty. Skipping this feature";
          continue;
        }
        const std::string component_group_name = feature.getMetaValue(s_PeptideRef);
        if (component_group_names.size() > 0 && component_group_names.count(component_group_name) == 0)
          continue;

        // Case #1: Features only
        if (feature.getSubordinates().size() <= 0) {
          std::vector<std::string> row;
          row.push_back(mdh.getSampleName());
          row.push_back(sampleTypeToString.at(mdh.getSampleType()));
          row.push_back(component_group_name);
          row.push_back(mdh.getReplicateGroupName());
          row.push_back("");
          row.push_back(mdh.batch_name);
          row.push_back(std::to_string(mdh.rack_number));
          row.push_back(std::to_string(mdh.plate_number));
          row.push_back(std::to_string(mdh.pos_number));
          row.push_back(std::to_string(mdh.inj_number));
          row.push_back(std::to_string(mdh.dilution_factor));
          row.push_back(std::to_string(mdh.inj_volume));
          row.push_back(mdh.inj_volume_units);
          row.push_back(mdh.operator_name);
          row.push_back(mdh.acq_method_name);
          row.push_back(mdh.proc_method_name);
          row.push_back(mdh.getFilename());
          row.push_back(mdh.getAcquisitionDateAndTimeAsString());
          row.push_back(mdh.scan_polarity);
          row.push_back(std::to_string(mdh.scan_mass_low));
          row.push_back(std::to_string(mdh.scan_mass_high));
          row.push_back(mdh.getInjectionName());
          row.push_back(feature.metaValueExists("used_") ? feature.getMetaValue("used_").toString() : "");
          for (const std::string& meta_value_name : meta_data) {
            if (feature.metaValueExists(meta_value_name) && meta_value_name == "QC_transition_group_message") {
              OpenMS::StringList messages = feature.getMetaValue(meta_value_name).toStringList();
              row.push_back(
                Utilities::join(messages.begin(), messages.end(), delimiter)
              );
            }
            else 
            {
              CastValue datum = SequenceHandler::getMetaValue(feature, feature, meta_value_name);
              if (datum.getTag() == CastValue::Type::FLOAT && datum.f_ != 0.0) {
                // NOTE: to_string() rounds at 1e-6. Therefore, some precision might be lost.
                row.push_back(std::to_string(datum.f_));
              }
              else 
              {
                row.push_back("");
              }
            }
          }
          rows_out.push_back(row);
        }

        // Case #2: Features and subordinates
        for (const OpenMS::Feature& subordinate : feature.getSubordinates()) {
          std::vector<std::string> row;
          row.push_back(mdh.getSampleName());
          row.push_back(sampleTypeToString.at(mdh.getSampleType()));
          row.push_back(component_group_name);
          if (!subordinate.metaValueExists(s_native_id) ||
              subordinate.getMetaValue(s_native_id).isEmpty() ||
              subordinate.getMetaValue(s_native_id).toString().empty()) {
            LOGV << "component_name is absent or empty. Skipping this subordinate";
            continue;
          }
          const std::string component_name = subordinate.getMetaValue(s_native_id);
          if (component_names.size() > 0 && component_names.count(component_name) == 0)
            continue;
          row.push_back(mdh.getReplicateGroupName());
          row.push_back(component_name);
          row.push_back(mdh.batch_name);
          row.push_back(std::to_string(mdh.rack_number));
          row.push_back(std::to_string(mdh.plate_number));
          row.push_back(std::to_string(mdh.pos_number));
          row.push_back(std::to_string(mdh.inj_number));
          row.push_back(std::to_string(mdh.dilution_factor));
          row.push_back(std::to_string(mdh.inj_volume));
          row.push_back(mdh.inj_volume_units);
          row.push_back(mdh.operator_name);
          row.push_back(mdh.acq_method_name);
          row.push_back(mdh.proc_method_name);
          row.push_back(mdh.getFilename());
          row.push_back(mdh.getAcquisitionDateAndTimeAsString());
          row.push_back(mdh.scan_polarity);
          row.push_back(std::to_string(mdh.scan_mass_low));
          row.push_back(std::to_string(mdh.scan_mass_high));
          row.push_back(mdh.getInjectionName());
          row.push_back(subordinate.metaValueExists("used_") ? subordinate.getMetaValue("used_").toString() : "");
          for (const std::string& meta_value_name : meta_data) {
            if (subordinate.metaValueExists(meta_value_name) && meta_value_name == "QC_transition_message") {
              OpenMS::StringList messages = subordinate.getMetaValue(meta_value_name).toStringList();
              row.push_back(
                Utilities::join(messages.begin(), messages.end(), delimiter)
              );
            }
            else if (feature.metaValueExists(meta_value_name) && meta_value_name == "QC_transition_group_message") 
            {
              OpenMS::StringList messages = feature.getMetaValue(meta_value_name).toStringList();
              row.push_back(
                Utilities::join(messages.begin(), messages.end(), delimiter)
              );
            }
            else 
            {
              CastValue datum = SequenceHandler::getMetaValue(feature, subordinate, meta_value_name);
              if (datum.getTag() == CastValue::Type::FLOAT && datum.f_ != 0.0) {
                // NOTE: to_string() rounds at 1e-6. Therefore, some precision might be lost.
                row.push_back(std::to_string(datum.f_));
              } 
              else 
              {
                row.push_back("");
              }
            }
          }
          rows_out.push_back(row);
        }
      }
    }
  }

  bool SequenceParser::writeDataTableFromMetaValue(
    const SequenceHandler& sequenceHandler,
    const std::filesystem::path& filename,
    const std::vector<FeatureMetadata>& meta_data,
    const std::set<SampleType>& sample_types
  )
  {
    LOGD << "START writeDataTableFromMetaValue";
    LOGI << "Storing: " << filename.generic_string();

    std::vector<std::vector<std::string>> rows;
    std::vector<std::string> headers;
    std::vector<std::string> meta_data_strings;
    for (const FeatureMetadata& m : meta_data) {
      meta_data_strings.push_back(metadataToString.at(m));
    }
    makeDataTableFromMetaValue(sequenceHandler, rows, headers, meta_data_strings, sample_types, std::set<std::string>(), std::set<std::string>(), std::set<std::string>());

    CSVWriter writer(filename.generic_string(), ",");
    const size_t cnt = writer.writeDataInRow(headers.cbegin(), headers.cend());

    if (cnt < headers.size()) {
      LOGD << "END writeDataTableFromMetaValue";
      return false;
    }

    for (const std::vector<std::string>& line : rows) {
      writer.writeDataInRow(line.cbegin(), line.cend());
    }

    LOGD << "END writeDataTableFromMetaValue";
    return true;
  }

  struct Row
  {
    Row() = default;
    ~Row() = default;
    Row(const Row&) = default;
    Row& operator=(const Row&) = default;
    Row(Row&&) = default;
    Row& operator=(Row&&) = default;

    Row(const std::string& cgn, const std::string& cn, const std::string& mvn) :
      component_group_name(cgn),
      component_name(cn),
      meta_value_name(mvn) {}

    std::string component_group_name;
    std::string component_name;
    std::string meta_value_name;
  };

  struct Row_less
  {
    bool operator()(const Row& lhs, const Row& rhs) const
    {
      return lhs.component_group_name + lhs.component_name + lhs.meta_value_name <
        rhs.component_group_name + rhs.component_name + rhs.meta_value_name;
    }
  };

  void SequenceParser::makeDataMatrixFromMetaValue(
    const SequenceHandler& sequenceHandler,
    Eigen::Tensor<float, 2>& data_out,
    Eigen::Tensor<std::string, 1>& columns_out,
    Eigen::Tensor<std::string, 2>& rows_out,
    const std::vector<std::string>& meta_data,
    const std::set<SampleType>& sample_types,
    const std::set<std::string>& sample_names,
    const std::set<std::string>& component_group_names,
    const std::set<std::string>& component_names
  )
  {
    std::set<std::string> columns;
    std::set<Row,Row_less> rows;
    std::map<std::string,std::map<Row,float,Row_less>> data_dict;

    for (const InjectionHandler& sampleHandler : sequenceHandler.getSequence()) {
      const MetaDataHandler& mdh = sampleHandler.getMetaData();
      std::map<std::string, float> validation_metrics = sampleHandler.getRawData().getValidationMetrics();
      const std::string& sample_name = mdh.getSampleName();
      if (sample_names.size() && sample_names.count(sample_name) == 0) 
        continue;
      if (sample_types.count(mdh.getSampleType()) == 0) 
        continue;
      data_dict.insert({ sample_name, std::map<Row,float,Row_less>() });
      for (const std::string& meta_value_name : meta_data) {
        // feature_map_history is not needed here as we are only interested in the current/"used" features
        for (const OpenMS::Feature& feature : sampleHandler.getRawData().getFeatureMapHistory()) {
          if (component_group_names.size() && component_group_names.count(feature.getMetaValue(s_PeptideRef).toString()) == 0)
            continue;
          if (feature.metaValueExists("used_")) {
            const std::string used = feature.getMetaValue("used_").toString();
            if (used.empty() || used[0] == 'f' || used[0] == 'F')
              continue;
          }

          // Case #1 Features only
          if (feature.getSubordinates().size() <= 0) {
            const Row row_tuple_name(
              feature.getMetaValue(s_PeptideRef).toString(),
              "",
              meta_value_name
            );
            CastValue datum;
            if (meta_value_name == "accuracy" || meta_value_name == "n_features") {
              datum = validation_metrics.at(meta_value_name);
            }
            else 
            {
              datum = SequenceHandler::getMetaValue(feature, feature, meta_value_name);
            }
            if (datum.getTag() == CastValue::Type::FLOAT && datum.f_ != 0.0 && !std::isnan(datum.f_)) { // Skip NAN (replaced by 0 later)
              data_dict[sample_name].emplace(row_tuple_name, datum.f_);
              columns.insert(sample_name);
              rows.insert(row_tuple_name);
            }
          }

          // Case #2 Features and subordinates
          for (const OpenMS::Feature& subordinate : feature.getSubordinates()) {
            if (component_names.size() && component_names.count(subordinate.getMetaValue(s_native_id).toString()) == 0) 
              continue;
            if (subordinate.metaValueExists("used_")) {
              const std::string used = subordinate.getMetaValue("used_").toString();
              if (used.empty() || used[0] == 'f' || used[0] == 'F') 
                continue;
            }
            const Row row_tuple_name(
              feature.getMetaValue(s_PeptideRef).toString(),
              subordinate.getMetaValue(s_native_id).toString(),
              meta_value_name
            );
            CastValue datum;
            if (meta_value_name == "accuracy" || meta_value_name == "n_features") {
              datum = validation_metrics.at(meta_value_name);
            }
            else 
            {
              datum = SequenceHandler::getMetaValue(feature, subordinate, meta_value_name);
            }
            if (meta_value_name == "validation") {
              if (datum.s_ == "TP") datum = static_cast<float>(1.0);
              else if (datum.s_ == "FP") datum = static_cast<float>(-1.0);
              else datum = static_cast<float>(-2.0);
            }
            if (datum.getTag() == CastValue::Type::FLOAT && datum.f_ != 0.0 && !std::isnan(datum.f_)) { // Skip NAN (replaced by 0 later)
              data_dict[sample_name].emplace(row_tuple_name, datum.f_);
              columns.insert(sample_name);
              rows.insert(row_tuple_name);
            }
          }
        }
      }
    }

    // Copy over the rows
    rows_out.resize((int)rows.size(), 3);
    int row = 0;
    for (const auto& r: rows) {
      rows_out(row, 0) = r.component_name;
      rows_out(row, 1) = r.component_group_name;
      rows_out(row, 2) = r.meta_value_name;
      ++row;
    }

    // Copy over the columns
    columns_out.resize((int)columns.size());
    int col = 0;
    for (const auto& c : columns) {
      columns_out(col) = c;
      ++col;
    }

    // Copy over the data
    data_out.resize((int)rows.size(), (int)columns.size());
    data_out.setConstant(0.0); // for now, initialize to 0 instead of NAN even though there are clear benefits to using NAN in packages that support NAN
    col = 0;
    for (const auto& c : columns) {
      row = 0;
      for (const auto& r : rows) {
        if (data_dict.count(c) && data_dict[c].count(r)) {
          data_out(row,col) = data_dict[c][r];
        }
        ++row;
      }
      ++col;
    }
  }

  bool SequenceParser::writeDataMatrixFromMetaValue(
    const SequenceHandler& sequenceHandler,
    const std::filesystem::path& filename,
    const std::vector<FeatureMetadata>& meta_data,
    const std::set<SampleType>& sample_types
  )
  {
    LOGD << "START writeDataMatrixFromMetaValue";

    LOGI << "Storing: " << filename.generic_string();

    Eigen::Tensor<float,2> data;
    Eigen::Tensor<std::string,1> columns;
    Eigen::Tensor<std::string, 2> rows;
    std::vector<std::string> meta_data_strings;
    for (const FeatureMetadata& m : meta_data) {
      meta_data_strings.push_back(metadataToString.at(m));
    }
    makeDataMatrixFromMetaValue(sequenceHandler, data, columns, rows, meta_data_strings, sample_types, std::set<std::string>(), std::set<std::string>(), std::set<std::string>());

    std::vector<std::string> headers = {"component_group_name", "component_name", "meta_value"};
    for (int i=0;i<columns.size();++i) headers.push_back(columns(i));

    CSVWriter writer(filename.generic_string(), ",");
    const size_t cnt = writer.writeDataInRow(headers.cbegin(), headers.cend());

    if (cnt < headers.size()) {
      LOGD << "END writeDataMatrixFromMetaValue";
      return false;
    }

    for (size_t i = 0; i < rows.dimension(0); ++i) {
      std::vector<std::string> line;
      for (size_t j = 0; j < rows.dimension(1); ++j) {
        line.push_back(rows(i,j));
      }
      for (size_t j = 0; j < data.dimension(1); ++j) {
        // NOTE: to_string() rounds at 1e-6. Therefore, some precision might be lost.
        line.emplace_back(std::to_string(data(i,j)));
      }
      writer.writeDataInRow(line.cbegin(), line.cend());
    }

    LOGD << "END writeDataMatrixFromMetaValue";
    return true;
  }

  bool StoreSequenceFileSmartPeak::onFilePicked(const std::filesystem::path& filename, ApplicationHandler* application_handler)
  {
    if (application_handler->sequenceHandler_.getSequence().size() == 0)
    {
      LOGE << "File cannot be stored without first loading the sequence.";
      return false;
    }
    SequenceParser::writeSequenceFileSmartPeak(application_handler->sequenceHandler_, filename);
    return true;
  }

  bool StoreSequenceFileAnalyst::onFilePicked(const std::filesystem::path& filename, ApplicationHandler* application_handler)
  {
    if (application_handler->sequenceHandler_.getSequence().size() == 0)
    {
      LOGE << "File cannot be stored without first loading the sequence.";
      return false;
    }
    SequenceParser::writeSequenceFileAnalyst(application_handler->sequenceHandler_, filename);
    return true;
  }

  bool StoreSequenceFileMasshunter::onFilePicked(const std::filesystem::path& filename, ApplicationHandler* application_handler)
  {
    if (application_handler->sequenceHandler_.getSequence().size() == 0)
    {
      LOGE << "File cannot be stored without first loading the sequence.";
      return false;
    }
    SequenceParser::writeSequenceFileMasshunter(application_handler->sequenceHandler_, filename);
    return true;
  }

  bool StoreSequenceFileXcalibur::onFilePicked(const std::filesystem::path& filename, ApplicationHandler* application_handler)
  {
    if (application_handler->sequenceHandler_.getSequence().size() == 0)
    {
      LOGE << "File cannot be stored without first loading the sequence.";
      return false;
    }
    SequenceParser::writeSequenceFileXcalibur(application_handler->sequenceHandler_, filename);
    return true;
  }
}
