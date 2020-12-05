
#ifndef SMARTPEAK_EXAMPLE_CONFIG_H
#define SMARTPEAK_EXAMPLE_CONFIG_H

// Macro to construct a c string containing the complete path to the test data
#define SMARTPEAK_GET_EXAMPLES_DATA_PATH(filename) (std::string("C:/Users/dmccloskey/Documents/GitHub/SmartPeak/src/examples/data/") + filename).c_str()
#define SMARTPEAK_GET_EXAMPLES_DATA_PATH_MESSAGE(prefix,filename,suffix) (prefix + std::string("C:/Users/dmccloskey/Documents/GitHub/SmartPeak/src/examples/data/") + filename + suffix).c_str()

#endif // SMARTPEAK_EXAMPLE_CONFIG_H
