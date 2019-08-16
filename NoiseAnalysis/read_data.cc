#include <stdio.h>
#include <iostream>
#include <string>
#include <vector>
#include "TH1.h"
#include "TCanvas.h"
#include "TApplication.h"
#include "TGraph.h"
#include "TFile.h"
#include "TSystem.h"
#include <fstream>
#include "TVirtualFitter.h"
#include "TH1F.h"

//#define RUN_PYTHON 0
#include "PROTOTYPE4_FEM.h"

using namespace std;

int main(int argc, char **argv)
{
    TApplication App("tapp", &argc, argv);
    TVirtualFitter::SetDefaultFitter("Minuit2");
    bool run_python = true;
    bool batch_mode = false;
    /*
     * The first argument is the file name without extensions,
     * so if you have a file called Data.lvm, then it should be just only Data
     */
    string filename = "";
    string cmd = "python serialize_lvm.py ";
    cout << "Arguments count: " << argc << endl;
    for (int i = 0; i < argc; i++)
    {
        cout << "Agrument[" << i << "] = " << argv[i] << endl;
        if (!strcmp("b", argv[i]))
        {
            batch_mode = true;
            cout << "Batch mode enabled" << endl;
        }
        else if(!strcmp("wp", argv[i])) //particle energy
        {
            run_python = false;
            cout << "Python script disabled" << endl;
        }
    }
    if (argc < 2)
    {
        cout << "Must give the file name without type (lvm)" << endl;
        cout << "USAGE: " << argv[0] << " filename_without_extension " << "[wp and/or b]" << endl
            << "Where b is for enable batch mode and wp for disable the python script." << endl;
        return -1;
    }
    else
    {
        filename = argv[1];
        cout << "File to open: " << filename << endl;
        cmd = cmd + filename + ".lvm";
    }

    /*
     * Creating the serialized files
     * The Python code opens the input file and extract the data to another two text files for easier reading
     */

    if (run_python)
    {
        if (system(cmd.c_str()))
        {
            cout << "Error while executing python script. Check files, permissions." << endl;
            return -1;
        }
    }
    //int lastindex = filename.find_last_of(".");
    //string stripped_filename = filename.substr(0, lastindex);
    string root_filename = filename + ".root";
    string data_filename = filename + ".txt";

    //Reading and storing in a TGraph
    TFile f(root_filename.c_str(), "RECREATE");
    ifstream data_file(filename + "_serialized.txt", ios::in);
    ofstream data_out;
    data_out.open(data_filename, ios::trunc); //delete its content if exists
    if (!data_out.is_open())
    {
        cout << "Output file can not be opened" << endl;
        return -1;
    }

    if (!data_file.good())
    {
        cout << "File can not be opened" << endl;
        return -1;
    }

    //Storing the data from the file
    double num;
    vector<double> data;
    vector<double> x_axis;

    //Read and store the iterator also for TGraph
    double i = 0;
    if (data_file.is_open())
    {
        while (data_file >> num)
        {
            data.push_back(num);
            x_axis.push_back(i);
            i++;
        }
    }
    auto n = data.size();
    const double *y = &data[0];
    const double *x = &x_axis[0];
    cout << "Data converted. Points: " << n << endl;
    TCanvas *c1 = new TCanvas("c1", "Pulses", 200, 10, 500, 300);
    TGraph *gr = new TGraph(n, x, y);
    gr->GetXaxis()->SetTitle("Time (unit)");
    gr->GetYaxis()->SetTitle("ADC");
    gr->Draw("ACP");

    //Write it into the file
    gr->Write("ADC samples");

    //--------
    //vector<double> &samples

    /*
     *  fits.SetParNames("Amplitude", "Sample Start", "Power", "Peak Time 1", "Pedestal", "Amplitude ratio", "Peak Time 2");
     */

    double peak = 0;
    double peak_sample = 0;
    double pedestal = 0;
    std::map<int, double> parameters_io;
    //parameters_io.insert(pair<int, double>(0, 30));
    parameters_io.insert(pair<int, double>(1, 100));
    parameters_io.insert(pair<int, double>(2, 2));   //power, 2
    parameters_io.insert(pair<int, double>(3, 120)); //peak time, 40ns
    //parameters_io.insert(pair<int, double>(4, 30));
    parameters_io.insert(pair<int, double>(5, 0)); //amplitude ratio, 0
    parameters_io.insert(pair<int, double>(6, 120));
    const int verbosity = 1;

    int number_of_samples = 1000;
    vector<double> ref_data; // = data;
    unsigned long num_of_retries = data.size() / number_of_samples;
    for (unsigned long j = 0; j < num_of_retries; j++)
    {
        ref_data.clear();
        for (int i = 0; i < number_of_samples; i++)
        {
            ref_data.push_back(data[(j * number_of_samples) + i]);
        }

        //cout << "SampleFit_PowerLawDoubleExp just about to start" << endl;
        PROTOTYPE4_FEM::SampleFit_PowerLawDoubleExp(ref_data, peak, peak_sample, pedestal, parameters_io, verbosity, &f, j);

        //Writing into the file
        std::string space = " ";
        std::string new_line = "\n";
        data_out << to_string(j) << space << to_string(pedestal) << space << to_string(peak) << space << new_line;
        data_out.flush();
        //data_file -> write(d.c_str(), d.size());
        //data_file -> flush();

        //gSystem->ProcessEvents();
    }
    data_out.close();

    TCanvas *adc_canvas = new TCanvas("adc_canvas", "ADC", 200, 10, 500, 300);
    TH1F *hADC = new TH1F("hADC", "ADC", 1000, -500.5, 499.5);
    hADC->GetXaxis()->SetTitle("ADC values");
    hADC->GetYaxis()->SetTitle("Number of ADC Samples per bin");
    hADC->SetCanExtend(TH1::kAllAxes);

    for (int i = 0; i < n; i++)
    {
        hADC->Fill(data[i]);
    }
    hADC->Draw("HIST");
    hADC->Write();
    f.Close();

    if(batch_mode)
    {
        return 0;
    }
    cout << "Done. Press Ctrl + C for exit. You can check the histograms if you run this code with X11 enabled." << endl;
    while (!batch_mode)
    {
        gSystem->ProcessEvents();
    }
    //App.Run();
    return 0;
}
