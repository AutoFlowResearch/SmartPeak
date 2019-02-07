// TODO: Add copyright

#include <SmartPeak/test_config.h>

#include <SmartPeak/core/LCMS_MRM_Validation_example.h>
#include <SmartPeak/core/Filenames.h>

using namespace SmartPeak;
using namespace std;

void test_main_Validation()
{
  const std::string main_dir = SMARTPEAK_GET_EXAMPLES_DATA_PATH("mzML_calibrators");
  Filenames static_filenames = Filenames::getDefaultStaticFilenames(main_dir);
  static_filenames.sequence_csv_i = ""; // abs path to the 18MB csv file
  static_filenames.quantitationMethods_csv_i = ""; // abs path to the quant method csv file
  static_filenames.referenceData_csv_i = ""; // abs path to the sampleNames.csv file

  example_LCMS_MRM_Validation(main_dir, static_filenames, ",");
}

int main(int argc, char **argv)
{
  test_main_Validation();
  return 0;
}
