// TODO: Add copyright

#include <SmartPeak/test_config.h>

#include <SmartPeak/pipelines/FIAMS_FullScan_Unknown_example.h>
#include <SmartPeak/core/Filenames.h>
#include <SmartPeak/core/Utilities.h>
#include <SmartPeak/core/RawDataProcessors/LoadFeatures.h>
#include <SmartPeak/core/RawDataProcessors/LoadAnnotations.h>

using namespace SmartPeak;
using namespace std;

void test_main_FIAMS_FullScan_Unknown()
{
  const std::string main_dir = SMARTPEAK_GET_EXAMPLES_DATA_PATH("FIAMS_FullScan_Unknowns");
  Filenames filenames_main;
  filenames_main.setTagValue(Filenames::Tag::MAIN_DIR, main_dir);
  filenames_main.setTagValue(Filenames::Tag::MZML_INPUT_PATH, main_dir + "/mzML/");
  filenames_main.setTagValue(Filenames::Tag::FEATURES_INPUT_PATH, main_dir + "/features/");
  filenames_main.setTagValue(Filenames::Tag::FEATURES_OUTPUT_PATH, main_dir + "/features/");

  example_FIAMS_FullScan_Unknowns(main_dir, filenames_main, ",");

  RawDataHandler rawDataHandler;
  LoadFeatures loadFeatures;
  LoadAnnotations loadAnnotations;
  Filenames filenames;

  filenames.setFullPath("featureXML_i", SMARTPEAK_GET_EXAMPLES_DATA_PATH("FIAMS_FullScan_Unknowns/features/20200618_QCserum_NewSourceSettings_80_20MeOH_water_100to1000Da_NEG_1_13_BatchName_1900-01-01_000000.featureXML"));
  loadFeatures.process(rawDataHandler, {}, filenames);
  OpenMS::FeatureMap fm1 = rawDataHandler.getFeatureMap();
  filenames.setFullPath("mzTab_i", SMARTPEAK_GET_EXAMPLES_DATA_PATH("FIAMS_FullScan_Unknowns/features/20200618_QCserum_NewSourceSettings_80_20MeOH_water_100to1000Da_NEG_1_13_BatchName_1900-01-01_000000.mzTab"));
  loadAnnotations.process(rawDataHandler, {}, filenames);
  OpenMS::MzTab mt1 = rawDataHandler.getMzTab();

  rawDataHandler.clear();

  filenames.setFullPath("featureXML_i", SMARTPEAK_GET_EXAMPLES_DATA_PATH("FIAMS_FullScan_Unknowns/features/20200618_QCserum_NewSourceSettings_80_20MeOH_water_100to1000Da_NEG_1_test.featureXML"));
  loadFeatures.process(rawDataHandler, {}, filenames);
  OpenMS::FeatureMap fm2 = rawDataHandler.getFeatureMap();
  filenames.setFullPath("mzTab_i", SMARTPEAK_GET_EXAMPLES_DATA_PATH("FIAMS_FullScan_Unknowns/features/20200618_QCserum_NewSourceSettings_80_20MeOH_water_100to1000Da_NEG_1_test.mzTab"));
  loadAnnotations.process(rawDataHandler, {}, filenames);
  OpenMS::MzTab mt2 = rawDataHandler.getMzTab();
  
  rawDataHandler.clear();
  
  std::cout << "fm1 size: " << fm1.size() << std::endl;
  std::cout << "fm2 size: " << fm2.size() << std::endl;
  assert(fm1.size() == fm2.size());

  std::cout << "mt1 size: " << mt1.getSmallMoleculeSectionRows().size() << std::endl;
  std::cout << "mt2 size: " << mt2.getSmallMoleculeSectionRows().size() << std::endl;
  assert(mt1.getSmallMoleculeSectionRows().size() == 7669);

  // First feature
  {
    const auto& f1 = fm1[0].getSubordinates()[0];
    const auto& f2 = fm2[0].getSubordinates()[0];

    std::cout << "0 peak_apex_int: " << f1.getMetaValue("peak_apex_int") << std::endl;
    std::cout << "0 getRT: " << f1.getRT() << std::endl;
    const auto* a1 = &mt1.getSmallMoleculeSectionRows().front();
    const auto* a2 = &mt2.getSmallMoleculeSectionRows().front();
    std::cout << "0 calc_mass_to_charge: " << a1->calc_mass_to_charge.get() << std::endl;
    std::cout << "0 chemical_formula: " << a1->chemical_formula.get() << std::endl;
    std::cout << "0 identifier: " << a1->identifier.get().at(0).get() << std::endl;
    assert(Utilities::assert_close((double)f1.getMetaValue("peak_apex_int"), 3.5241834375e05));
    assert(Utilities::assert_close((double)f1.getRT(), 0.0));
    assert(Utilities::assert_close((double)a1->calc_mass_to_charge.get(), 102.957));
    assert(static_cast<std::string>(a1->chemical_formula.get()) == "H3O3P");
    assert(a1->identifier.get().at(0).get() == "HMDB:HMDB0001443");
    assert(Utilities::assert_close((double)f1.getIntensity(), (double)f2.getIntensity()));
    assert(Utilities::assert_close((double)f1.getMetaValue("peak_apex_int"), (double)f2.getMetaValue("peak_apex_int")));
    assert(Utilities::assert_close((double)f1.getRT(), (double)f2.getRT()));
    assert(Utilities::assert_close((double)a1->calc_mass_to_charge.get(), (double)a2->calc_mass_to_charge.get()));
    assert(static_cast<std::string>(a1->chemical_formula.get()) == static_cast<std::string>(a2->chemical_formula.get()));
    assert(a1->identifier.get().at(0).get() == a2->identifier.get().at(0).get());
  }
  // Last feature
  {
    const auto& f1 = fm1.back().getSubordinates()[0];
    const auto& f2 = fm2.back().getSubordinates()[0];

    std::cout << "50 peak_apex_int: " << f1.getMetaValue("peak_apex_int") << std::endl;
    std::cout << "50 getRT: " << f1.getRT() << std::endl;
    const auto* a1 = &mt1.getSmallMoleculeSectionRows().back();
    const auto* a2 = &mt2.getSmallMoleculeSectionRows().back();
    std::cout << "-1 calc_mass_to_charge: " << a1->calc_mass_to_charge.get() << std::endl;
    std::cout << "-1 chemical_formula: " << a1->chemical_formula.get() << std::endl;
    std::cout << "-1 identifier: " << a1->identifier.get().at(0).get() << std::endl;
    assert(Utilities::assert_close((double)f1.getMetaValue("peak_apex_int"), 2.3098934375e05));
    assert(Utilities::assert_close((double)f1.getRT(), 0.0));
    assert(Utilities::assert_close((double)a1->calc_mass_to_charge.get(), 942.346));
    assert(static_cast<std::string>(a1->chemical_formula.get()) == "C37H59N5O12S2");
    assert(a1->identifier.get().at(0).get() == "HMDB:HMDB0127611");
    assert(Utilities::assert_close((double)f1.getIntensity(), (double)f2.getIntensity()));
    assert(Utilities::assert_close((double)f1.getMetaValue("peak_apex_int"), (double)f2.getMetaValue("peak_apex_int")));
    assert(Utilities::assert_close((double)f1.getRT(), (double)f2.getRT()));
    assert(Utilities::assert_close((double)a1->calc_mass_to_charge.get(), (double)a2->calc_mass_to_charge.get()));
    assert(static_cast<std::string>(a1->chemical_formula.get()) == static_cast<std::string>(a2->chemical_formula.get()));
    assert(a1->identifier.get().at(0).get() == a2->identifier.get().at(0).get());
  }
}

int main()
{
  test_main_FIAMS_FullScan_Unknown();
  return 0;
}
