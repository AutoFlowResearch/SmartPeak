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

#include <SmartPeak/core/Filenames.h>
#include <SmartPeak/core/SampleType.h>
#include <SmartPeak/core/SequenceHandler.h>
#include <SmartPeak/core/SequenceSegmentHandler.h>
#include <SmartPeak/iface/IProcessorDescription.h>
#include <SmartPeak/iface/IFilenamesHandler.h>
#include <SmartPeak/core/Parameters.h>
#include <SmartPeak/core/SequenceSegmentObservable.h>
#include <SmartPeak/iface/IFilePickerHandler.h>

namespace SmartPeak
{
  struct SequenceSegmentProcessor : IProcessorDescription, IFilenamesHandler
  {
    SequenceSegmentProcessor(const SequenceSegmentProcessor& other) = delete;
    SequenceSegmentProcessor& operator=(const SequenceSegmentProcessor& other) = delete;
    virtual ~SequenceSegmentProcessor() = default;

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
      const ParameterSet& params_I,
      Filenames& filenames_I
    ) const;

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

    /* IFilenamesHandler */
    virtual void getFilenames(Filenames& filenames) const override { };

    /* IProcessorDescription */
    virtual std::vector<std::string> getRequirements() const override { return {}; };
    virtual std::set<std::string> getInputs() const override { return {}; };
    virtual std::set<std::string> getOutputs() const override { return {}; };

    SequenceSegmentObservable* sequence_segment_observable_ = nullptr;

  protected:
    virtual void doProcess(
      SequenceSegmentHandler& sequenceSegmentHandler_IO,
      const SequenceHandler& sequenceHandler_I,
      const ParameterSet& params_I,
      Filenames& filenames_I
    ) const = 0;

    // Forced to write this, because the other user-defined constructors inhibit
    // the implicit definition of a default constructor
    // Even though this class is abstract and hence can't be instantiated,
    // derived classes will call the base's constructor
    SequenceSegmentProcessor() = default;

    void processForAllSegments(std::vector<SmartPeak::SequenceSegmentHandler>& sequence_segment_handlers,
                               SequenceSegmentObservable* sequence_segment_observable,
                               Filenames& filenames);

    /** 
      Utility method: most of the loading / storing processors can load / store per sequence
      But also from one unique file to or from multiple sequence (static filename)
      this method aims at returning the appropriate filename based on "static filename" behavior.
    */
    std::string constructFilename(const std::string& filename, bool static_filename) const;
  };

}
