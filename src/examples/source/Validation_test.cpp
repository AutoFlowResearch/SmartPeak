// TODO: Add copyright

#include <SmartPeak/test_config.h>

#include <SmartPeak/core/LCMS_MRM_Validation_example.h>
#include <SmartPeak/core/Filenames.h>

using namespace SmartPeak;
using namespace std;

void test_main_Validation(std::string dirpath, std::string sequence_csv_i, std::string quantitationMethods_csv_i, std::string referenceData_csv_i)
{
  const std::string main_dir = SMARTPEAK_GET_EXAMPLES_DATA_PATH(dirpath);
  Filenames static_filenames = Filenames::getDefaultStaticFilenames(main_dir);
  static_filenames.sequence_csv_i = sequence_csv_i; // abs path to the 18MB csv file
  static_filenames.quantitationMethods_csv_i = quantitationMethods_csv_i; // abs path to the quant method csv file
  static_filenames.referenceData_csv_i = referenceData_csv_i; // abs path to the sampleNames.csv file

  std::cout << static_filenames.quantitationMethods_csv_i  << std::endl;
  std::cout << static_filenames.referenceData_csv_i  << std::endl;
  example_LCMS_MRM_Validation(main_dir, static_filenames, ",", true);
}

int main(int argc, char **argv)
{
  test_main_Validation(argv[1], argv[2], argv[3], argv[4]);
  return 0;
}
