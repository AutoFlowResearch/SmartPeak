// TODO: Add copyright

#include <SmartPeak/core/SequenceHandler.h>

namespace SmartPeak
{
  // """A class to manage the mapping of metadata and FeatureMaps
  //     (i.e., multiple samples in a run batch/sequence)"""

  void SequenceHandler::clear()
  {
    sequence_.clear();
    index_to_sample_.clear();
    sample_to_index_.clear();
    sequence_segments_.clear();
    sample_groups_.clear();

    dir_static_.clear();
    dir_dynamic_.clear();
    filenames_.clear();
  }

  void SequenceHandler::setFilenames(std::vector<std::string>& filenames)
  {
    filenames_ = filenames;
  }

  std::vector<std::string>& SequenceHandler::getFilenames()
  {
    return filenames_;
  }

  std::vector<std::string> SequenceHandler::getFilenames() const
  {
    return filenames_;
  }

  void setDirStatic(std::string& dir_static)
  {
    dir_static_ = dir_static;
    setFilenames(getDefaultStaticFilenames(dir_static));
  }

  std::string& getDirStatic()
  {
    return dir_static_;
  }

  std::string getDirStatic() const
  {
    return dir_static_;
  }

  void setDirDynamic(std::string& dir_dynamic)
  {
    dir_dynamic_ = dir_dynamic;
  }

  std::string& getDirDynamic()
  {
    return dir_dynamic_;
  }

  std::string getDirDynamic() const
  {
    return dir_dynamic_;
  }

  void setSequence(std::vector<SampleHandler>& sequence)
  {
    sequence_ = sequence;
  }

  std::vector<SampleHandler>& getSequence()
  {
    return sequence_;
  }

  std::vector<SampleHandler> getSequence() const
  {
    return sequence_;
  }

  void setSequenceSegments(std::vector<SequenceSegmentHandler>& sequence_segments)
  {
    sequence_segments_ = sequence_segments;
  }

  std::vector<SequenceSegmentHandler>& getSequenceSegments()
  {
    return sequence_segments_;
  }

  std::vector<SequenceSegmentHandler> getSequenceSegments() const
  {
    return sequence_segments_;
  }

  void setSampleGroups(std::vector<SampleGroupHandler>& sample_groups)
  {
    sample_groups_ = sample_groups;
  }

  std::vector<SampleGroupHandler>& getSampleGroups()
  {
    return sample_groups_;
  }

  std::vector<SampleGroupHandler> getSampleGroups() const
  {
    return sample_groups_;
  }

  def getDefaultStaticFilenames(dir_I):
      """Return the default map of filetype to filename for static files

      Static files are small and consist of application settings that
      are loaded in at the start of the application.
      
      Args:
          dir_I (str): the directory that all files can be found
          
      Returns:
          dict: filenames
      """
      
      filenames = {
          # static
          'sequence_csv_i': '''%s/%s''' % (dir_I, "sequence.csv"),
          'parameters_csv_i': '''%s/%s''' % (dir_I, "parameters.csv"),
          'traML_csv_i': '''%s/%s''' % (dir_I, "traML.csv"),
          'featureFilterComponents_csv_i': '''%s/%s''' % (
              dir_I, "featureFilterComponents.csv"),
          'featureFilterComponentGroups_csv_i': '''%s/%s''' % (
              dir_I, "featureFilterComponentGroups.csv"),
          'featureQCComponents_csv_i': '''%s/%s''' % (
              dir_I, "featureQCComponents.csv"),
          'featureQCComponentGroups_csv_i': '''%s/%s''' % (
              dir_I, "featureQCComponentGroups.csv"),
          'quantitationMethods_csv_i': '''%s/%s''' % (
              dir_I, "quantitationMethods.csv"),
          'standardsConcentrations_csv_i': '''%s/%s''' % (
              dir_I, "standardsConcentrations.csv"),
          'db_json_i': '''%s/%s''' % (dir_I, "db.json")
          }
      return filenames

  def getDefaultDynamicFilenames(dir_I, sample_name_I):
      """Return the default map of filetype to filename for dynamic files

      Dynamic files are often much larger and are read/written to
      disk as needed by the application
      
      Args:
          dir_I (str): the directory that all files can be found
          sample_name_I (str): the name of the file
              (also the sample_name, sample_group_name, or sequence_segment_name)
          
      Returns:
          dict: filenames
      """
      
      filenames = {
          # dynamic
          'mzML_i': '''%s/mzML/%s.mzML''' % (dir_I, sample_name_I),
          'featureXML_o': '''%s/features/%s.FeatureXML''' % (dir_I, sample_name_I),
          'feature_csv_o': '''%s/features/%s.csv''' % (dir_I, sample_name_I),
          'featureXML_i': '''%s/features/%s.FeatureXML''' % (dir_I, sample_name_I),
          'features_pdf_o': '''%s/features/%s''' % (dir_I, sample_name_I),
          # .pdf added dynamically
          'calibrators_pdf_o': '''%s/features/%s''' % (dir_I, sample_name_I),
          # .pdf added dynamically
          'quantitationMethods_csv_o': '''%s/features/%s_quantitationMethods.csv''' % (
              dir_I, sample_name_I),
          'componentsToConcentrations_csv_o': '''%s/features/
              %s_componentsToConcentrations.csv''' % (dir_I, sample_name_I),
          }
      return filenames

  def addSampleToSequence(
      meta_data_I, featureMap_I, 
  ):
      """add meta_data and featureMap to a sequence list

      Args:
          meta_data_I (dict): dictionary of meta data (e.g., sample_name)
          featureMap_I (FeatureMap): processed data in a FeatureMap

      Returns:
          dict: injection: dictionary of meta_data and FeatureMap
      """

      meta_data = self.parse_metaData(meta_data_I)

      sample = SampleHandler()
      sample.meta_data = meta_data
      sample.featureMap = featureMap_I

      self.sequence.append(sample)
      self.index_to_sample[len(self.sequence)-1] = meta_data["sample_name"]
      self.sample_to_index[meta_data["sample_name"]] = len(self.sequence)-1

  def getSamplesInSequence(sample_names):
      """Return samples in a sequence
      
      Args:
          sample_names (list): list of str sample names
      """

      samples = [
          self.sequence[self.sample_to_index[sample]] for sample in sample_names
          if sample in self.sample_to_index.keys()]
      return samples

  def getMetaValue(feature, subordinate, meta_value):
      """Returns the metaValue
      
      Args:
          feature (Feature): OpenMS::Feature corresponding to transition_group
          subordinate (Feature): OPENMS::Feature corresponding to transition

      Returns:
          float: datum: metaValue extraction from feature or subordinate
          
      """
      datum = None
      if meta_value == "RT":
          datum = feature.getRT()
      else:
          datum = feature.getMetaValue(meta_value)
          if datum is None:
              datum = subordinate.getMetaValue(meta_value)
      return datum

  def getRequiredHeaders(self):
      """Return required headers in a sequence file"""

      # # MultiQuant Example
      # required_headers = [
      #     "SampleName","SampleID",
      #     "Comments","AcqMethod",
      #     "ProcMethod","RackCode","PlateCode","VialPos","SmplInjVol",
      #     "DilutFact","WghtToVol","Type","RackPos","PlatePos",
      #     "SetName","OutputFile"
      #     ]

      # optional_headers = [
      #     "comments", "acquisition_method", "processing_method",
      #     "rack_code", "plate_code", "vial_position", "rack_position", 
      #     "plate_position",
      #     "injection_volume", "dilution_factor", "weight_to_volume",
      #     "set_name"]

      required_headers = [
          "sample_name", "sample_group_name", "sample_type", "filename",
          "sequence_segment_name"
          ]

      return required_headers

  def parse_metaData(meta_data):
      """Parse a sequence file to ensure all headers are present
      
      Args:
          meta_data (dict): a dictionary of sample information

      Returns:
          dict: meta_data
      """

      sample_types = ["Unknown", "Standard", "QC", "Blank", "Double Blank", "Solvent"]
      sample_types_str = ",".join(sample_types)

      # check for required headers
      for header in self.getRequiredHeaders():
          if header not in meta_data:
              print(
                  'SequenceFile Error: required header in sequence list "' +
                  header + '" not found.')
              raise NameError("sequenceFile header")
              # meta_data[header] = None  # not needed
          
      # check for correctness of data
      if meta_data["sample_name"] is None:
          print(
              "SequenceFile Error: sample_name must be specified.")
          raise NameError("sample name")
      if meta_data["sample_group_name"] is None:
          print(
              "SequenceFile Error: sample_group_name must be specified.")
          raise NameError("sample group name")
      if meta_data["sequence_segment_name"] is None:
          print(
              "SequenceFile Error: sequence_segment_name must be specified.")
          raise NameError("sequence group name")
      if meta_data["filename"] is None:
          print(
              "SequenceFile Error: filename must be specified.")
          raise NameError("filename name")
      if meta_data["filename"] is None:
          print(
              "SequenceFile Error: filename must be specified.")
          raise NameError("filename name")

      if meta_data["sample_type"] is None or\
          meta_data["sample_type"] not in sample_types:
          print(
              "SequenceFile Error: sample_type for sample_name " +
              meta_data["sample_name"] + " is not correct.")
          print(
              "Supported samples types are the following: " +
              sample_types_str)
          raise NameError("sample type")

      # other checks...

      return meta_data
}
