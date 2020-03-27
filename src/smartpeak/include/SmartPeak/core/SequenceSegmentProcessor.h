// TODO: Add copyright

#pragma once

#include <SmartPeak/core/Filenames.h>
#include <SmartPeak/core/MetaDataHandler.h>
#include <SmartPeak/core/SampleType.h>
#include <SmartPeak/core/SequenceHandler.h>
#include <SmartPeak/core/SequenceSegmentHandler.h>

namespace SmartPeak
{
  struct SequenceSegmentProcessor
  {
    // C.67: A polymorphic class should suppress copying
    SequenceSegmentProcessor(const SequenceSegmentProcessor& other) = delete;
    SequenceSegmentProcessor& operator=(const SequenceSegmentProcessor& other) = delete;

    // C.35: A base class destructor should be either public and virtual, or protected and nonvirtual
    // C.127: A class with a virtual function should have a virtual or protected destructor
    virtual ~SequenceSegmentProcessor() = default;

    virtual int getID() const = 0; /// get the raw data processor struct ID
    virtual std::string getName() const = 0; /// get the raw data processor struct name
    virtual std::string getDescription() const = 0; /// get the raw data processor struct description

    /**
      Interface to all  sequence segment processing methods.

      @param[in,out] sequenceSegmentHandler_IO Sequence segment handler
      @param[in] sequenceHandler_I Sequence handler
      @param[in] params_I Dictionary of parameter names, values, descriptions, and tags
      @param[in] filenames Info about where data should be read from or written to
    */
    virtual void process(
      SequenceSegmentHandler& sequenceSegmentHandler_IO,
      const SequenceHandler& sequenceHandler_I,
      const std::map<std::string, std::vector<std::map<std::string, std::string>>>& params_I,
      const Filenames& filenames
    ) const = 0;

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
      const SampleType sampleType,
      std::vector<size_t>& sampleIndices
    );

  protected:
    // Forced to write this, because the other user-defined constructors inhibit
    // the implicit definition of a default constructor
    // Even though this class is abstract and hence can't be instantiated,
    // derived classes will call the base's constructor
    SequenceSegmentProcessor() = default;
  };

  struct CalculateCalibration : SequenceSegmentProcessor
  {
    int getID() const { return 15; }
    std::string getName() const { return "CALCULATE_CALIBRATION"; }
    std::string getDescription() const { return "Determine the optimal relationship between known sample concentration and measured intensity."; }

    /**
      Optimize the calibration curve for all components.
    */
    void process(
      SequenceSegmentHandler& sequenceSegmentHandler_IO,
      const SequenceHandler& sequenceHandler_I,
      const std::map<std::string, std::vector<std::map<std::string, std::string>>>& params_I,
      const Filenames& filenames
    ) const;
  };

  struct LoadStandardsConcentrations : SequenceSegmentProcessor
  {
    int getID() const { return -1; }
    std::string getName() const { return "LOAD_STANDARDS_CONCENTRATIONS"; }
    std::string getDescription() const { return "Load the standards concentrations file that gives the relationship between injection, component, and known concentration from disk."; }

    /**
      Load the standards concentration file.
    */
    void process(
      SequenceSegmentHandler& sequenceSegmentHandler_IO,
      const SequenceHandler& sequenceHandler_I,
      const std::map<std::string, std::vector<std::map<std::string, std::string>>>& params_I,
      const Filenames& filenames
    ) const;
  };

  struct LoadQuantitationMethods : SequenceSegmentProcessor
  {
    int getID() const { return 17; }
    std::string getName() const { return "LOAD_QUANTITATION_METHODS"; }
    std::string getDescription() const { return "Load each transitions calibration model defined in quantitationMethods from disk."; }

    /**
      Load the quantitation methods file.
    */
    void process(
      SequenceSegmentHandler& sequenceSegmentHandler_IO,
      const SequenceHandler& sequenceHandler_I,
      const std::map<std::string, std::vector<std::map<std::string, std::string>>>& params_I,
      const Filenames& filenames
    ) const;
  };

  struct StoreQuantitationMethods : SequenceSegmentProcessor
  {
    int getID() const { return 16; }
    std::string getName() const { return "STORE_QUANTITATION_METHODS"; }
    std::string getDescription() const { return "Write each transitions calibration model to disk for later use."; }

    /**
      Write the quantitation methods to disk.
    */
    void process(
      SequenceSegmentHandler& sequenceSegmentHandler_IO,
      const SequenceHandler& sequenceHandler_I,
      const std::map<std::string, std::vector<std::map<std::string, std::string>>>& params_I,
      const Filenames& filenames
    ) const;
  };

  struct PlotCalibrators : SequenceSegmentProcessor
  {
    int getID() const { return -1; }
    std::string getName() const { return "PLOT_CALIBRATORS"; }
    std::string getDescription() const { return "Plot the calibration points for each component where the x-axis is concentration ratio and the y-axis is intensity ratio."; }

    /**
      Plot the calibration points for each component.

      NOTE: Not yet implemented
    */
    void process(
      SequenceSegmentHandler& sequenceSegmentHandler_IO,
      const SequenceHandler& sequenceHandler_I,
      const std::map<std::string, std::vector<std::map<std::string, std::string>>>& params_I,
      const Filenames& filenames
    ) const;
  };
}
