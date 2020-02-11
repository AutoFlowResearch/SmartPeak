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
#include <OpenMS/TRANSFORMATIONS/FEATUREFINDER/FeatureFinder.h>
#include <OpenMS/FORMAT/MzTabFile.h>
#include <OpenMS/FORMAT/MzTab.h>
#include <OpenMS/FILTERING/NOISEESTIMATION/SignalToNoiseEstimatorMedianRapid.h>
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


std::vector<OpenMS::MSSpectrum> time_spectra(const OpenMS::MSExperiment & experiment, float n_seconds) {
    std::vector<OpenMS::MSSpectrum> result;
    for (auto s : experiment.getSpectra()) {
        if (s.getRT() < n_seconds) result.push_back(s);
    }
    return result;
}


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

OpenMS::MSSpectrum sliding_bin(const OpenMS::MSExperiment & experiment, float resolution, float n_seconds) {
    std::vector<float> mzs; 
    std::vector<float> bin_sizes;
    for (size_t i = 0; i < 58; i++) {
        mzs.push_back(i*20 + 50);
        bin_sizes.push_back(mzs[i] / (resolution*4.0));
    }
    OpenMS::MSSpectrum result;
    std::vector<OpenMS::MSSpectrum> spectra = time_spectra(experiment, n_seconds); //experiment.getSpectra()
    for (size_t i = 1; i < mzs.size() - 1; i++) {
        OpenMS::MSSpectrum full_spectrum = OpenMS::SpectrumAddition::addUpSpectra(
            spectra, bin_sizes[i], false
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
    std::vector<float> resolutions_float {
120000.0,
120000.0,
120000.0,
120000.0,
120000.0,
120000.0,
120000.0,
120000.0,
120000.0,
120000.0,
120000.0,
120000.0,
120000.0,
120000.0,
120000.0,
120000.0,
120000.0,
120000.0,
120000.0,
120000.0,
120000.0,
120000.0,
120000.0,
120000.0,
120000.0,
120000.0,
120000.0,
120000.0,
120000.0,
120000.0,
120000.0,
120000.0,
120000.0,
120000.0,
120000.0,
120000.0,
120000.0,
120000.0,
120000.0,
120000.0,
120000.0,
120000.0,
120000.0,
120000.0,
120000.0,
120000.0,
120000.0,
120000.0,
120000.0,
120000.0,
120000.0,
120000.0,
120000.0,
120000.0,
120000.0,
120000.0,
120000.0,
120000.0,
120000.0,
120000.0,
120000.0,
120000.0,
120000.0,
120000.0,
120000.0,
120000.0,
120000.0,
120000.0,
120000.0,
120000.0,
120000.0,
120000.0,
120000.0,
120000.0,
120000.0,
120000.0,
120000.0,
120000.0,
120000.0,
120000.0,
120000.0,
120000.0,
120000.0,
120000.0,
120000.0,
120000.0,
120000.0,
120000.0,
120000.0,
120000.0,
120000.0,
120000.0,
120000.0,
120000.0,
120000.0,
120000.0,
120000.0,
120000.0,
120000.0,
120000.0,
120000.0,
120000.0,
120000.0,
120000.0,
120000.0,
120000.0,
120000.0,
120000.0,
120000.0,
120000.0,
120000.0,
120000.0,
120000.0,
120000.0,
120000.0,
120000.0,
120000.0,
120000.0,
120000.0,
120000.0,
120000.0,
120000.0,
120000.0,
120000.0,
120000.0,
120000.0,
120000.0,
120000.0,
120000.0,
120000.0,
120000.0,
120000.0,
120000.0,
120000.0,
120000.0,
120000.0,
120000.0,
120000.0,
120000.0,
    };

    std::vector<std::string> charges {
"POS",
"POS",
"POS",
"NEG",
"NEG",
"NEG",
"POS",
"POS",
"POS",
"NEG",
"NEG",
"NEG",
    };
    std::vector<std::string> filenames {
"20191120_metk_Serum_FS_50uLflow_10uLinject_HighMass_POS_2",
"20191120_metk_Serum_FS_50uLflow_10uLinject_HighMass_POS_3",
"20191120_metk_Serum_FS_50uLflow_10uLinject_HighMass_POS_4",
"20191120_metk_Serum_FS_50uLflow_10uLinject_HighMass_NEG_2",
"20191120_metk_Serum_FS_50uLflow_10uLinject_HighMass_NEG_3",
"20191120_metk_Serum_FS_50uLflow_10uLinject_HighMass_NEG_4",
"20191120_metk_Serum_FS_50uLflow_10uLinject_LowMass_POS_2",
"20191120_metk_Serum_FS_50uLflow_10uLinject_LowMass_POS_3",
"20191120_metk_Serum_FS_50uLflow_10uLinject_LowMass_POS_4",
"20191120_metk_Serum_FS_50uLflow_10uLinject_LowMass_NEG_2",
"20191120_metk_Serum_FS_50uLflow_10uLinject_LowMass_NEG_3",
"20191120_metk_Serum_FS_50uLflow_10uLinject_LowMass_NEG_4",
    };

    std::string results_dir = "/Users/svegal/Documents/FIA_results_slow/";
    // std::string results_dir = "/Users/svegal/Documents/FIA_results_ff/";
    std::string dir = "/Users/svegal/Documents/FIA_experiments_23102019/";
    // std::string dir = "/Volumes/CFB/Scientific dep/009_Experimental data/023_LC-MS/Douglas/20191009_Methodtest_QCserum/mzML/";
    std::vector<float> seconds {5, 10, 30, 60, 90, 180};
    std::vector<std::string> seconds_str {"5", "10", "30", "60", "90", "180"};

    for (size_t i = 0; i < filenames.size(); i++) {
        std::string f = filenames[i];
        std::cout << f << std::endl;
        for (size_t j_sec = 0; j_sec < seconds.size(); j_sec++) {
            std::cout << seconds_str[j_sec] << " sec" << std::endl;
            Filenames filenames;
            filenames.parameters_csv_i = SMARTPEAK_GET_TEST_DATA_PATH("RawDataProcessor_params_1_core_tmpFix.csv");
            filenames.mzML_i = dir + f + ".mzML";

            map<string, vector<map<string, string>>> params;

            RawDataHandler rawDataHandler;

            LoadParameters loadParameters;
            loadParameters.process(rawDataHandler, {}, filenames);
            params = rawDataHandler.getParameters();

            LoadRawData loadRawData;
            loadRawData.process(rawDataHandler, params, filenames);

            OpenMS::MSExperiment experiment = rawDataHandler.getExperiment();

            // OpenMS::MSSpectrum spectrum_concat;
            // spectrum_concat.setMSLevel(1);
            // std::vector<OpenMS::MSSpectrum> spectra = time_spectra(experiment, seconds[j_sec]); //experiment.getSpectra()
            // for (auto s : spectra) {
            //     for (auto it = s.begin(); it != s.end(); ++it) {
            //         spectrum_concat.push_back(*it);
            //     }
            // }
            // spectrum_concat.sortByPosition();

            OpenMS::MSSpectrum full_spectrum = sliding_bin(experiment, resolutions_float[i], seconds[j_sec]);
            OpenMS::SavitzkyGolayFilter filter;
            filter.filter(full_spectrum);

            OpenMS::MorphologicalFilter morph_filter;
            morph_filter.filter(full_spectrum);
            // store_spectum(results_dir + f + "_full_spectrum_" + seconds_str[j_sec] + ".csv", full_spectrum);
            // store_spectum(results_dir + f + "_full_spectrum_concat_" + seconds_str[j_sec] + ".csv", spectrum_concat);

            std::cout << "Before picking" << std::endl;

            OpenMS::PeakPickerHiRes picker;
            OpenMS::Param param = picker.getParameters();
            picker.setParameters(param);

            OpenMS::MSSpectrum picked_full_no_bin;
            picker.pick(full_spectrum, picked_full_no_bin);

            store_spectum(results_dir + f + "_picked_spectrum_" + seconds_str[j_sec] + ".csv", picked_full_no_bin);

            OpenMS::SignalToNoiseEstimatorMedianRapid sne(50);

            std::vector<double> mzs, intensities;

            for (auto it = full_spectrum.begin(); it != full_spectrum.end(); ++it)
            {
                mzs.push_back(it->getMZ());
                intensities.push_back(it->getIntensity());
            }

            OpenMS::SignalToNoiseEstimatorMedianRapid::NoiseEstimator e = sne.estimateNoise(mzs, intensities);

            OpenMS::MSSpectrum spectrum_sn;
            for (auto it = picked_full_no_bin.begin(); it != picked_full_no_bin.end(); ++it)
            {
                OpenMS::Peak1D peak;
                peak.setMZ(it->getMZ());
                peak.setIntensity(e.get_noise_value(it->getMZ()));
                spectrum_sn.push_back(peak);
            }

            store_spectum(results_dir + f + "_signal_to_noise_" + seconds_str[j_sec] + ".csv", spectrum_sn);

            OpenMS::MSSpectrum spectrum_def = picked_full_no_bin;
            OpenMS::FeatureMap feature_map;
            std::string polarity;
            if (charges[i] == "POS") {
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
            // OpenMS::MSExperiment picked_map;
            // picked_full_no_bin.setMSLevel(1);
            // picked_map.addSpectrum(picked_full_no_bin);
            // picked_map.updateRanges(1);

            // // experiment.updateRanges();

            // OpenMS::FeatureFinder ff;
            // OpenMS::FeatureMap feature_map;
            // ff.run("centroided", picked_map, feature_map, OpenMS::Param(), OpenMS::FeatureMap());

            OpenMS::Param ams_param;
            ams_param.setValue("ionization_mode", "auto");
            ams_param.setValue("mass_error_value", 1e+06 / (resolutions_float[i]*2));
            ams_param.setValue("positive_adducts", SMARTPEAK_GET_TEST_DATA_PATH("PositiveAdductsSmall.tsv"));
            ams_param.setValue("negative_adducts", SMARTPEAK_GET_TEST_DATA_PATH("NegativeAdductsSmall.tsv"));

            OpenMS::AccurateMassSearchEngine ams;
            ams.setParameters(ams_param);
            ams.init();
            std::vector<OpenMS::AccurateMassSearchResult> hmdb_results;

            OpenMS::MzTab mztab_output;
            ams.run(feature_map, mztab_output);

            OpenMS::MzTabFile mztab_outfile;
            mztab_outfile.store(results_dir + f + "_all_adducts_low_" + seconds_str[j_sec] + ".mzTab", mztab_output);
        }
    }
}

BOOST_AUTO_TEST_SUITE_END()
