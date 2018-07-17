// TODO: Add copyright

#pragma once

namespace SmartPeak
{
  class SequenceSegmentHandler
  {
public:
    SequenceSegmentHandler() = default;
    ~SequenceSegmentHandler() = default;

    def __init__(self):
        """Sequence Group
        """
        self.sequence_segment_name = None
        self.sample_indices = None
        self.standards_concentrations = None
        self.quantitation_methods = None
        self.components_to_concentrations = None

    def clear_data(self):
        self.sequence_segment_name = None
        self.sample_indices = None
        self.standards_concentrations = None
        self.quantitation_methods = None
        self.components_to_concentrations = None

    def setSequenceSegmentName(self, sequence_segment_name_I):
        """Set the sequence_segment_name"""
        self.sequence_segment_name = sequence_segment_name_I
        
    def getSequenceSegmentName(self):
        """Return the sequence_segment_name"""
        return self.sequence_segment_name
    
    def setSampleIndices(self, sample_indices_I):
        """Set the sample_indices"""
        self.sample_indices = sample_indices_I
        
    def getSampleIndices(self):
        """Return the sample_indices"""
        return self.sample_indices

    def setStandardsConcentrations(self, standards_concentrations_I):
        """Set the standards_concentrations"""
        self.standards_concentrations = standards_concentrations_I
        
    def getStandardsConcentrations(self):
        """Return the standards_concentrations"""
        return self.standards_concentrations

    def setQuantitationMethods(self, quantitation_methods_I):
        """Set the quantitation_methods"""
        self.quantitation_methods = quantitation_methods_I
        
    def getQuantitationMethods(self):
        """Return the quantitation_methods"""
        return self.quantitation_methods

    def setComponentsToConcentrations(
        self,
        components_to_concentrations_I
    ):
        """Set the components_to_concentrations"""
        self.components_to_concentrations = \
            components_to_concentrations_I
        
    def getComponentsToConcentrations(self):
        """Return the components_to_concentrations"""
        return self.components_to_concentrations

    # def getDefaultSampleOrder(self, sample_type):
    #     """Return the default order for each sample in a group
        
    #     Args:
    #         sample_type (str): type of sample

    #     Returns:
    #         int: order
            
    #     """

    #     order = -1
    #     if sample_type == "Standard":
    #         order = 0
    #     elif sample_type == "Unknown":
    #         order = 1 
    #     elif sample_type == "QC":
    #         order = 2 
    #     elif sample_type == "Blank":
    #         order = 3
    #     elif sample_type == "Double Blank":
    #         order = 3
    #     elif sample_type == "Solvent":
    #         pass

    #     return order

private:
  };
}