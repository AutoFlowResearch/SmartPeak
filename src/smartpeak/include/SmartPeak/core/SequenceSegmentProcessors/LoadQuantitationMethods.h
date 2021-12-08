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

#include <SmartPeak/core/SequenceSegmentProcessor.h>

namespace SmartPeak
{

  struct LoadQuantitationMethods : SequenceSegmentProcessor, IFilePickerHandler
  {
    LoadQuantitationMethods(bool static_filenames = false)
      : static_filenames_(static_filenames) {}
    bool static_filenames_;

    /* IFilePickerHandler */
    bool onFilePicked(const std::filesystem::path& filename, ApplicationHandler* application_handler) override;

    /* IProcessorDescription */
    virtual std::string getName() const override { return "LOAD_QUANTITATION_METHODS"; }
    virtual std::string getDescription() const override { return "Load each transitions calibration model defined in quantitationMethods from disk."; }
    virtual ParameterSet getParameterSchema() const override;
    virtual std::vector<std::string> getRequirements() const override;

    /**
      Load the quantitation methods file.
    */
    void process(
      SequenceSegmentHandler& sequenceSegmentHandler_IO,
      const SequenceHandler& sequenceHandler_I,
      const ParameterSet& params_I,
      Filenames& filenames_I
    ) const override;

    /* IFilenamesHandler */
    virtual void getFilenames(Filenames& filenames) const override;
  };

}