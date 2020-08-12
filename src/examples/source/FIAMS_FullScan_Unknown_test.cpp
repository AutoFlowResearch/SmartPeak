// TODO: Add copyright

#include <SmartPeak/test_config.h>

#include <SmartPeak/pipelines/FIAMS_FullScan_Unknown_example.h>
#include <SmartPeak/core/Filenames.h>
#include <SmartPeak/core/Utilities.h>

using namespace SmartPeak;
using namespace std;

void test_main_FIAMS_FullScan_Unknown()
{
  const std::string main_dir = SMARTPEAK_GET_EXAMPLES_DATA_PATH("FIAMS_FullScan_Unknowns");
  const Filenames static_filenames = Filenames::getDefaultStaticFilenames(main_dir);

  OpenMS::MzTab mt1 = example_FIAMS_FullScan_Unknowns(main_dir, static_filenames, ",");

  RawDataHandler rawDataHandler;
  LoadFeatures loadFeatures;
  LoadAnnotations loadAnnotations;
  Filenames filenames;

  filenames.featureXML_i = SMARTPEAK_GET_EXAMPLES_DATA_PATH("FIAMS_FullScan_Unknowns/features/20200618_QCserum_NewSourceSettings_80_20MeOH_water_100to1000Da_NEG_1_13_BatchName_1900-01-01_000000.featureXML");
  loadFeatures.process(rawDataHandler, {}, filenames);
  OpenMS::FeatureMap fm1 = rawDataHandler.getFeatureMap();

  rawDataHandler.clear();

  filenames.featureXML_i = SMARTPEAK_GET_EXAMPLES_DATA_PATH("FIAMS_FullScan_Unknowns/features/20200618_QCserum_NewSourceSettings_80_20MeOH_water_100to1000Da_NEG_1_test.featureXML");
  loadFeatures.process(rawDataHandler, {}, filenames);
  OpenMS::FeatureMap fm2 = rawDataHandler.getFeatureMap();
  filenames.featureXML_i = SMARTPEAK_GET_EXAMPLES_DATA_PATH("FIAMS_FullScan_Unknowns/features/20200618_QCserum_NewSourceSettings_80_20MeOH_water_100to1000Da_NEG_1_test.mzTab");
  loadAnnotations.process(rawDataHandler, {}, filenames);
  OpenMS::MzTab mt2 = rawDataHandler.getMzTab();
  
  rawDataHandler.clear();
  
  cout << "fm1 size: " << fm1.size() << endl;
  cout << "fm2 size: " << fm2.size() << endl;
  assert(fm1.size() == fm2.size());

  cout << "mt1 size: " << mt1.getSmallMoleculeSectionRows().size() << endl;
  cout << "mt2 size: " << mt2.getSmallMoleculeSectionRows().size() << endl;
  assert(mt1.getSmallMoleculeSectionRows().size() == 92784);

  const OpenMS::Feature* f1 = &fm1[0];
  const OpenMS::Feature* f2 = &fm2[0];
  cout << "0 native_id: " << f1->getMetaValue("native_id") << endl;
  cout << "0 peak_apex_int: " << f1->getMetaValue("peak_apex_int") << endl;
  cout << "0 getRT: " << f1->getRT() << endl;
  const auto* a1 = &mt1.getSmallMoleculeSectionRows().front();
  const auto* a2 = &mt2.getSmallMoleculeSectionRows().front();
  cout << "0 calc_mass_to_charge: " << a1->calc_mass_to_charge.get() << endl;
  cout << "0 chemical_formula: " << a1->chemical_formula.get() << endl;
  cout << "0 identifier: " << a1->identifier.get().at(0).get() << endl;
  assert(f1->getMetaValue("native_id") == "23dpg.23dpg_1.Heavy");
  assert(Utilities::assert_close((double)f1->getMetaValue("peak_apex_int"), 249.465663036488));
  assert(Utilities::assert_close((double)f1->getIntensity(), 228.975008430023053));
  assert(Utilities::assert_close((double)f1->getRT(), 0.0));
  assert(Utilities::assert_close((double)a1->calc_mass_to_charge.get(), 303.101676923849027));
  assert(static_cast<std::string>(a1->chemical_formula.get()) == "C15H14FN3O3");
  assert(a1->identifier.get().at(0).get() == "HMDB:HMDB0015336");
  assert(f1->getMetaValue("native_id") == f2->getMetaValue("native_id"));
  assert(Utilities::assert_close((double)f1->getIntensity(), (double)f2->getIntensity()));
  assert(Utilities::assert_close((double)f1->getMetaValue("peak_apex_int"), (double)f2->getMetaValue("peak_apex_int")));
  assert(Utilities::assert_close((double)f1->getRT(), (double)f2->getRT()));
  assert(Utilities::assert_close((double)a1->calc_mass_to_charge.get(), (double)a2->calc_mass_to_charge.get()));
  assert(static_cast<std::string>(a1->chemical_formula.get()) == static_cast<std::string>(a2->chemical_formula.get()));
  assert(a1->identifier.get().at(0).get() == a2->identifier.get().at(0).get());

  f1 = &fm1[50];
  f2 = &fm2[50];
  cout << "50 native_id: " << f1->getMetaValue("native_id") << endl;
  cout << "50 peak_apex_int: " << f1->getMetaValue("peak_apex_int") << endl;
  cout << "50 getRT: " << f1->getRT() << endl;
  a1 = &mt1.getSmallMoleculeSectionRows().back();
  a2 = &mt2.getSmallMoleculeSectionRows().back();
  cout << "-1 calc_mass_to_charge: " << a1->calc_mass_to_charge.get() << endl;
  cout << "-1 chemical_formula: " << a1->chemical_formula.get() << endl;
  cout << "-1 identifier: " << a1->identifier.get().at(0).get() << endl;
  assert(f1->getMetaValue("native_id") == "actp.actp_1.Heavy");
  assert(Utilities::assert_close((double)f1->getMetaValue("peak_apex_int"), 228.975008430023053));
  assert(Utilities::assert_close((double)f1->getIntensity(), 228.975008430023053));
  assert(Utilities::assert_close((double)f1->getRT(), 745.612));
  assert(Utilities::assert_close((double)a1->calc_mass_to_charge.get(), 999.620302583604257));
  assert(static_cast<std::string>(a1->chemical_formula.get()) == "C29H42O5");
  assert(a1->identifier.get().at(0).get() == "HMDB:HMDB0038683");
  assert(f1->getMetaValue("native_id") == f2->getMetaValue("native_id"));
  assert(Utilities::assert_close((double)f1->getIntensity(), (double)f2->getIntensity()));
  assert(Utilities::assert_close((double)f1->getMetaValue("peak_apex_int"), (double)f2->getMetaValue("peak_apex_int")));
  assert(Utilities::assert_close((double)f1->getRT(), (double)f2->getRT()));
  assert(Utilities::assert_close((double)a1->calc_mass_to_charge.get(), (double)a2->calc_mass_to_charge.get()));
  assert(static_cast<std::string>(a1->chemical_formula.get()) == static_cast<std::string>(a2->chemical_formula.get()));
  assert(a1->identifier.get().at(0).get() == a2->identifier.get().at(0).get());
}

int main()
{
  test_main_FIAMS_FullScan_Unknown();
  return 0;
}
