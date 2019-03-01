// TODO: Add copyright

#pragma once

#include <SmartPeak/core/Filenames.h>
#include <SmartPeak/core/MetaDataHandler.h>
#include <SmartPeak/core/SequenceHandler.h>
#include <SmartPeak/core/SequenceSegmentHandler.h>

namespace SmartPeak
{
  class SequenceSegmentProcessor
  {
  public:
    SequenceSegmentProcessor() = default;
    ~SequenceSegmentProcessor() = default;

    virtual int getID() const = 0;  ///< get the raw data processor class ID
    virtual std::string getName() const = 0;  ///< get the raw data processor class name
    virtual std::string getDescription() const = 0;  ///< get the raw data processor class description

    /** Interface to all  sequence segment processing methods.

      @param[in,out] sequenceSegmentHandler_IO Sequence segment handler
      @param[in] sequenceHandler_I Sequence handler
      @param[in] params_I Dictionary of parameter names, values, descriptions, and tags
      @param[in] filenames Info about where data should be read from or written to
    */
    virtual void process(
      SequenceSegmentHandler& sequenceSegmentHandler_IO,
      const SequenceHandler& sequenceHandler_I,
      const std::map<std::string, std::vector<std::map<std::string, std::string>>>& params_I,
      const Filenames& filenames,
      const bool verbose_I = false) const = 0;

    /**
      Return all injection indices that belong to a given sample type.

      @param[in] sequenceSegmentHandler Sequence segment handler
      @param[in] sequenceHandler Sequence handler
      @param[in] sampleType Sample type filter
      @param[out] sampleIndices Output sample indices
    */
    static void getSampleIndicesBySampleType(
      const SequenceSegmentHandler& sequenceSegmentHandler,
      const SequenceHandler& sequenceHandler,
      const MetaDataHandler::SampleType sampleType,
      std::vector<size_t>& sampleIndices
    );
  };

  class CalculateCalibration : public SequenceSegmentProcessor
  {
  public:
    using SequenceSegmentProcessor::SequenceSegmentProcessor;

    int getID() const { return id_; };
    std::string getName() const { return name_; };
    std::string getDescription() const { return description_; };

    /**
      Optimize the calibration curve for all components.
    */
    void process(
      SequenceSegmentHandler& sequenceSegmentHandler_IO,
      const SequenceHandler& sequenceHandler_I,
      const std::map<std::string, std::vector<std::map<std::string, std::string>>>& params_I,
      const Filenames& filenames,
      const bool verbose_I = false
    ) const;

  protected:
    int id_ = 1;
    std::string name_ = "CALCULATE_CALIBRATION_CURVES";
    std::string description_ = "Determine the optimal relationship between known sample concentration and measured intensity.";
  };

  class LoadStandardsConcentrations : public SequenceSegmentProcessor
  {
  public:
    using SequenceSegmentProcessor::SequenceSegmentProcessor;

    int getID() const { return id_; };
    std::string getName() const { return name_; };
    std::string getDescription() const { return description_; };

    /**
      Load the standards concentration file.
    */
    void process(
      SequenceSegmentHandler& sequenceSegmentHandler_IO,
      const SequenceHandler& sequenceHandler_I,
      const std::map<std::string, std::vector<std::map<std::string, std::string>>>& params_I,
      const Filenames& filenames,
      const bool verbose_I = false
    ) const;

  protected:
    int id_ = 2;
    std::string name_ = "LOAD_STANDARDS_CONCENTRATIONS";
    std::string description_ = "Load the standards concentrations file that gives the relationship between injection, component, and known concentration from disk.";
  };

  class LoadQuantitationMethods : public SequenceSegmentProcessor
  {
  public:
    using SequenceSegmentProcessor::SequenceSegmentProcessor;

    int getID() const { return id_; };
    std::string getName() const { return name_; };
    std::string getDescription() const { return description_; };

    /**
      Load the quantitation methods file.
    */
    void process(
      SequenceSegmentHandler& sequenceSegmentHandler_IO,
      const SequenceHandler& sequenceHandler_I,
      const std::map<std::string, std::vector<std::map<std::string, std::string>>>& params_I,
      const Filenames& filenames,
      const bool verbose_I = false
    ) const;

  protected:
    int id_ = 3;
    std::string name_ = "LOAD_QUANTITATION_METHODS";
    std::string description_ = "Load each transitions calibration model defined in quantitationMethods from disk.";
  };

  class StoreQuantitationMethods : public SequenceSegmentProcessor
  {
  public:
    using SequenceSegmentProcessor::SequenceSegmentProcessor;

    int getID() const { return id_; };
    std::string getName() const { return name_; };
    std::string getDescription() const { return description_; };

    /**
      Write the quantitation methods to disk.
    */
    void process(
      SequenceSegmentHandler& sequenceSegmentHandler_IO,
      const SequenceHandler& sequenceHandler_I,
      const std::map<std::string, std::vector<std::map<std::string, std::string>>>& params_I,
      const Filenames& filenames,
      const bool verbose_I = false
    ) const;

  protected:
    int id_ = 4;
    std::string name_ = "STORE_QUANTITATION_METHODS";
    std::string description_ = "Write each transitions calibration model to disk for later use.";
  };

  class PlotCalibrators : public SequenceSegmentProcessor
  {
  public:
    using SequenceSegmentProcessor::SequenceSegmentProcessor;

    int getID() const { return id_; };
    std::string getName() const { return name_; };
    std::string getDescription() const { return description_; };

    /**
      Plot the calibration points for each component.

      NOTE: Not yet implemented
    */
    void process(
      SequenceSegmentHandler& sequenceSegmentHandler_IO,
      const SequenceHandler& sequenceHandler_I,
      const std::map<std::string, std::vector<std::map<std::string, std::string>>>& params_I,
      const Filenames& filenames,
      const bool verbose_I = false
    ) const;

  protected:
    int id_ = 5;
    std::string name_ = "PLOT_CALIBRATORS";
    std::string description_ = "Plot the calibration points for each component where the x-axis is concentration ratio and the y-axis is intensity ratio.";
  };
}
