// TODO: Add copyright

#include <SmartPeak/test_config.h>

#include <SmartPeak/pipelines/HPLC_UV_Unknown_example.h>
#include <SmartPeak/core/Filenames.h>
#include <SmartPeak/core/Utilities.h>
#include <plog/Log.h>
#include <plog/Appenders/ConsoleAppender.h>

using namespace SmartPeak;
using namespace std;

void test_main_HPLC_UV_Unknown()
{
  const std::string main_dir = SMARTPEAK_GET_EXAMPLES_DATA_PATH("HPLC_UV_Unknowns");
  const Filenames static_filenames = Filenames::getDefaultStaticFilenames(main_dir);

  example_HPLC_UV_Unknowns(main_dir, static_filenames, ",");

  RawDataHandler rawDataHandler;
  LoadFeatures loadFeatures;
  Filenames filenames;

  filenames.featureXML_i = SMARTPEAK_GET_EXAMPLES_DATA_PATH("HPLC_UV_Unknowns/features/20171013_HMP_C61_ISO_P1_GA1_UV_VIS_2_1_BatchName_1900-01-01_000000.featureXML");
  loadFeatures.process(rawDataHandler, {}, filenames);
  OpenMS::FeatureMap fm1 = rawDataHandler.getFeatureMap();

  rawDataHandler.clear();

  filenames.featureXML_i = SMARTPEAK_GET_EXAMPLES_DATA_PATH("HPLC_UV_Unknowns/features/20171013_HMP_C61_ISO_P1_GA1_UV_VIS_2_test.featureXML");
  loadFeatures.process(rawDataHandler, {}, filenames);
  OpenMS::FeatureMap fm2 = rawDataHandler.getFeatureMap();

  cout << "fm1.size(): " << fm1.size() << endl;
  cout << "fm2.size(): " << fm2.size() << endl;

  assert(fm1.size() == 27);
  assert(fm1.size() == fm2.size());

  cout << "fm1[0].getSubordinates().size(): " << fm1[0].getSubordinates().size() << endl;
  cout << "fm2[0].getSubordinates().size(): " << fm2[0].getSubordinates().size() << endl;

  assert(fm1[0].getSubordinates().size() == 2);
  assert(fm1[0].getSubordinates().size() == fm2[0].getSubordinates().size());
  const OpenMS::Feature* f1 = &fm1[0].getSubordinates()[0];
  const OpenMS::Feature* f2 = &fm2[0].getSubordinates()[0];

  cout << "native_id: "     << f1->getMetaValue("native_id") << endl;
  cout << "peak_apex_int: " << f1->getMetaValue("peak_apex_int") << endl;
  cout << "getRT: "         << f1->getRT() << endl;

  assert(f1->getMetaValue("native_id") == "5-HTP");
  assert(Utilities::assert_close((double)f1->getMetaValue("peak_apex_int"), 2.830286591566697));
  assert(Utilities::assert_close((double)f1->getRT(), 2.50809));
  assert(f1->getMetaValue("native_id") == f2->getMetaValue("native_id"));
  assert(Utilities::assert_close((double)f1->getMetaValue("peak_apex_int"), (double)f2->getMetaValue("peak_apex_int")));
  assert(Utilities::assert_close((double)f1->getRT(), (double)f2->getRT()));

  cout << "fm1[6].getSubordinates().size(): " << fm1[6].getSubordinates().size() << endl;
  cout << "fm2[6].getSubordinates().size(): " << fm2[6].getSubordinates().size() << endl;

  assert(fm1[6].getSubordinates().size() == 2);
  assert(fm1[6].getSubordinates().size() == fm2[6].getSubordinates().size());
  f1 = &fm1[6].getSubordinates()[0];
  f2 = &fm2[6].getSubordinates()[0];

  cout << "native_id: "     << f1->getMetaValue("native_id") << endl;
  cout << "peak_apex_int: " << f1->getMetaValue("peak_apex_int") << endl;
  cout << "getRT: "         << f1->getRT() << endl;

  assert(f1->getMetaValue("native_id") == "Acetylserotonin");
  assert(Utilities::assert_close((double)f1->getMetaValue("peak_apex_int"), 2.646960722065395));
  assert(Utilities::assert_close((double)f1->getRT(), 5.28569));
  assert(f1->getMetaValue("native_id") == f2->getMetaValue("native_id"));
  assert(Utilities::assert_close((double)f1->getMetaValue("peak_apex_int"), (double)f2->getMetaValue("peak_apex_int")));
  assert(Utilities::assert_close((double)f1->getRT(), (double)f2->getRT()));

  cout << "fm1[26].getSubordinates().size(): " << fm1[26].getSubordinates().size() << endl;
  cout << "fm2[26].getSubordinates().size(): " << fm2[26].getSubordinates().size() << endl;

  assert(fm1[26].getSubordinates().size() == 2);
  assert(fm1[26].getSubordinates().size() == fm2[26].getSubordinates().size());
  f1 = &fm1[26].getSubordinates()[0];
  f2 = &fm2[26].getSubordinates()[0];

  cout << "native_id: "     << f1->getMetaValue("native_id") << endl;
  cout << "peak_apex_int: " << f1->getMetaValue("peak_apex_int") << endl;
  cout << "getRT: "         << f1->getRT() << endl;

  assert(f1->getMetaValue("native_id") == "Tryptophan");
  assert(Utilities::assert_close((double)f1->getMetaValue("peak_apex_int"), 2.076895443935449));
  assert(Utilities::assert_close((double)f1->getRT(), 3.87883));
  assert(f1->getMetaValue("native_id") == f2->getMetaValue("native_id"));
  assert(Utilities::assert_close((double)f1->getMetaValue("peak_apex_int"), (double)f2->getMetaValue("peak_apex_int")));
  assert(Utilities::assert_close((double)f1->getRT(), (double)f2->getRT()));

  // cout << "\n\nSTART ENTIRE OUTPUT FOR DEBUGGING" << endl;
  // for (const OpenMS::Feature& f : fm1) {
  //   // cout << f.getMetaValue("native_id") << "\t" << f.getMetaValue("peak_apex_int") << "\t" << f.getRT() << "\n";
  //   for (const OpenMS::Feature& sub : f.getSubordinates()) {
  //     cout << sub.getMetaValue("native_id") << "\t\t" << sub.getMetaValue("peak_apex_int") << "\t\t" << sub.getRT() << "\n";
  //   }
  //   cout << "\n";
  // }
  // cout << "END ENTIRE OUTPUT FOR DEBUGGING" << endl;
}

int main()
{
  // Add .csv appender: 32 MiB per file, max. 100 log files
  static plog::RollingFileAppender<plog::CsvFormatter>
    fileAppender("smartpeak_log.csv", 1024 * 1024 * 32, 100);

  // Add console appender, instead of only the file one
  static plog::ConsoleAppender<plog::TxtFormatter> consoleAppender;

  // Init logger with two appenders
  plog::init(plog::info, &fileAppender).addAppender(&consoleAppender);
  test_main_HPLC_UV_Unknown();
  return 0;
}
