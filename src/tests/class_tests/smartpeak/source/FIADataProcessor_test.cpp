// TODO: Add copyright

#include <SmartPeak/test_config.h>

#define BOOST_TEST_MODULE FIADataProcessor test suite
#include <boost/test/included/unit_test.hpp>
#include <SmartPeak/algorithm/FIADataProcessor.h>
#include <boost/algorithm/string/predicate.hpp>
#include <iomanip>

#include <OpenMS/CONCEPT/ClassTest.h>
#include <OpenMS/FORMAT/FeatureXMLFile.h>
#include <OpenMS/TRANSFORMATIONS/FEATUREFINDER/PeakWidthEstimator.h>
#include <OpenMS/TRANSFORMATIONS/RAW2PEAK/PeakPickerCWT.h>
#include <OpenMS/ANALYSIS/OPENSWATH/TargetedSpectraExtractor.h>
#include <OpenMS/ANALYSIS/ID/AccurateMassSearchEngine.h>
#include <OpenMS/FORMAT/MzTabFile.h>
#include <OpenMS/FORMAT/MzTab.h>
#include <OpenMS/FILTERING/BASELINE/MorphologicalFilter.h>
#include <OpenMS/KERNEL/SpectrumHelper.h>
#include <OpenMS/ANALYSIS/OPENSWATH/SpectrumAddition.h>
#include <OpenMS/FILTERING/SMOOTHING/SavitzkyGolayFilter.h>

#include <SmartPeak/io/FileReader.h>
#include <SmartPeak/core/RawDataHandler.h>
#include <SmartPeak/core/RawDataProcessor.h>

#include <iostream>
#include <fstream>

using namespace SmartPeak;
using namespace std;

void store_spectum(const std::string & filename, const OpenMS::MSSpectrum & spectrum) {
    std::ofstream fullpickedfile;
    fullpickedfile.open(filename);
    fullpickedfile << "mz,int\n";
    for (auto it = spectrum.begin(); it != spectrum.end(); ++it) {
        // std::cout << it->getMZ() << std::endl;
        fullpickedfile << std::fixed << std::setprecision(10) << it->getMZ() << ","  << it->getIntensity() << "\n";
    }
    fullpickedfile.close();
}

OpenMS::MSSpectrum sliding_bin(const OpenMS::MSExperiment & experiment, float resolution) {
    std::vector<float> mzs {0, 50, 75, 150, 180, 200, 300, 500, 700, 2000};
    std::vector<float> bin_sizes;
    for (size_t i = 0; i < mzs.size() - 1; i++) {
        bin_sizes.push_back(mzs[i+1] / resolution / 6.0);
    }
    OpenMS::MSSpectrum result;
    for (size_t i = 1; i < mzs.size() - 1; i++) {
        OpenMS::MSSpectrum full_spectrum = OpenMS::SpectrumAddition::addUpSpectra(
            experiment.getSpectra(), bin_sizes[i], false
        );
        for (auto it = full_spectrum.begin(); it != full_spectrum.end(); ++it) {
            if (it->getMZ() > mzs[i+1]) break;
            if (it->getMZ() >= mzs[i]) result.push_back(*it);
        }
    }
    result.sortByPosition();
    return result;
}

BOOST_AUTO_TEST_SUITE(fiadataprocessor)

BOOST_AUTO_TEST_CASE(integrate_by_time)
{
    std::vector<std::string> resolutions {"120000", "60000", "30000"};
    std::vector<float> resolutions_float {120000.0, 60000.0, 30000.0};
    std::vector<std::string> dirs {
        "/Users/svegal/Documents/IDX_FIA-MS/Res120000/20190614_QCSerum_NoColumn/",
        "/Users/svegal/Documents/IDX_FIA-MS/Res60000/QCserum_NoColumn_Res60000/",
        "/Users/svegal/Documents/IDX_FIA-MS/Res30000/QCserum_NoColumn/",
    };
    std::vector<std::string> prefix {
        "20190614_metk_SerumQC",
        "20190619_metk_SerumQC",
        "20190621_metk_SerumQC"
    };
    std::vector<std::string> masses {"LowMass", "HighMass"};
    std::vector<std::string> charges {"POS", "NEG"};
    std::vector<std::vector<std::string>> replicas { {"1", "2", "3"}, {"4", "5", "6"}, {"7", "8", "9"} };

    // std::vector<float> bins = {0.0008, 0.0002, 0.0004};
    // std::vector<bool> bools = {false, false, false};

    std::string results_dir = "/Users/svegal/Documents/FIA_results/";

    for (size_t i = 0; i < 2; i++) {
        for (std::string mass: masses) {
            for (std::string charge: charges) {
                std::string f_end = prefix[i] + "_FS_NoColumn_" + mass + "_Res" + resolutions[i] + "_" + charge;

                std::cout << "STARTED " << f_end << std::endl;

                std::vector<OpenMS::MSSpectrum> summed_all;

                for (std::string replica: replicas[i]) {
                    std::string f = prefix[i] + replica + "_FS_NoColumn_" + mass + "_Res" + resolutions[i] + "_" + charge;

                    Filenames filenames;
                    filenames.parameters_csv_i = SMARTPEAK_GET_TEST_DATA_PATH("RawDataProcessor_params_1_core_tmpFix.csv");
                    filenames.mzML_i = dirs[i] + f + ".mzML";

                    map<string, vector<map<string, string>>> params;

                    RawDataHandler rawDataHandler;

                    LoadParameters loadParameters;
                    loadParameters.process(rawDataHandler, {}, filenames);
                    params = rawDataHandler.getParameters();

                    LoadRawData loadRawData;
                    loadRawData.process(rawDataHandler, params, filenames);

                    OpenMS::MSExperiment experiment = rawDataHandler.getExperiment();

                    OpenMS::MSSpectrum full_spectrum = sliding_bin(experiment, resolutions_float[i]);
                    OpenMS::SavitzkyGolayFilter filter;
                    filter.filter(full_spectrum);

                    OpenMS::MorphologicalFilter morph_filter;
                    morph_filter.filter(full_spectrum);
                    store_spectum(results_dir + f + "_full_spectrum_no_bin_float.csv", full_spectrum);

                    std::cout << "Before picking" << std::endl;

                    OpenMS::PeakPickerHiRes picker;
                    OpenMS::Param param = picker.getParameters();
                    picker.setParameters(param);

                    OpenMS::MSSpectrum picked_full_no_bin;
                    picker.pick(full_spectrum, picked_full_no_bin);
                    
                    summed_all.push_back(picked_full_no_bin);

                    store_spectum(results_dir + f + "_picked_spectrum_no_bin.csv", picked_full_no_bin);
                }
                OpenMS::MSSpectrum spectrum_def;
                spectrum_def.setMSLevel(1);
                for (auto s : summed_all) {
                    for (auto it = s.begin(); it != s.end(); ++it) {
                        spectrum_def.push_back(*it);
                    }
                }
                spectrum_def.sortByPosition();
                OpenMS::FeatureMap feature_map;
                std::string polarity;
                if (charge == "POS") {
                    polarity = "positive";
                } else {
                    polarity = "negative";
                }
                for (auto it = spectrum_def.begin(); it != spectrum_def.end(); ++it) {
                    OpenMS::Feature f;
                    f.setIntensity(it->getIntensity());
                    f.setMZ(it->getMZ());
                    feature_map.push_back(f);
                    feature_map[0].setMetaValue("scan_polarity", polarity);
                }

                OpenMS::Param ams_param;
                ams_param.setValue("ionization_mode", "auto");
                ams_param.setValue("mass_error_value", 10.0);
                ams_param.setValue("positive_adducts", SMARTPEAK_GET_TEST_DATA_PATH("PositiveAdductsSmall.tsv"));
                ams_param.setValue("negative_adducts", SMARTPEAK_GET_TEST_DATA_PATH("NegativeAdductsSmall.tsv"));


                OpenMS::AccurateMassSearchEngine ams;
                ams.setParameters(ams_param);
                ams.init();
                std::vector<OpenMS::AccurateMassSearchResult> hmdb_results;

                OpenMS::MzTab mztab_output;
                ams.run(feature_map, mztab_output);

                OpenMS::MzTabFile mztab_outfile;
                mztab_outfile.store(results_dir + f_end + ".mzTab", mztab_output);
                
                store_spectum(results_dir + f_end + "_full_centroids_picked.csv", spectrum_def);
            }
        }
    }
}

BOOST_AUTO_TEST_SUITE_END()
