#include <stdio.h>
#include <iostream>
#include <fstream>
#include "TH1F.h"
#include "TGraph.h"
#include "TFile.h"
#include "TCanvas.h"
#include "TApplication.h"
#include "TSystem.h"

using namespace std;

struct CalculatedValues
{
    unsigned long id;
    double pedestal;
    double peakval;
    double peakpos;
};

std::ostream& operator << (std::ostream& os, const CalculatedValues& c)
{
    os << "ID: " << c.id << std::endl
        << "Pedestal: " << c.pedestal << std::endl
        << "Peak value: " << c.peakval << std::endl;
        //<< "Peak position: " << c.peakpos << std::endl;
    return os;
}

std::istream& operator >> (std::istream& is, CalculatedValues& c)
{
    is >> c.id >> c.pedestal >> c.peakval; //>> c.peakpos;
    return is;
}

int main(int argc, char** argv)
{
    bool batch_mode = false;
    TApplication App("tapp", &argc, argv);
    std::string filename = "";
    cout << "Arguments count: " << argc << endl;
    for(int i = 0; i < argc; i++)
    {
        cout << "Agrument[" << i << "] = " << argv[i] << endl;
        if (!strcmp("b", argv[i]))
        {
            batch_mode = true;
            cout << "Batch mode enabled" << endl;
        }
    }
    if(argc < 2)
    {
        cout << "Must give the file name without type (txt) [OPTIONAL: b for batch mode]" << endl;
        return -1;
    }
    else
    {
        filename = argv[1];
        cout << "File to open: " << filename << endl;
    }

    //int lastindex = filename.find_last_of("."); 
    //string stripped_filename = filename.substr(0, lastindex); 
    string root_filename = filename + "_pedestals.root";
    string data_filename = filename + ".txt";
    ifstream data_file(data_filename, ios::in);

    if(!data_file.good())
    {
        cout << "File can not be opened" << endl;
        return -1;
    }

    //Reading and storing in a TGraph
    TFile f(root_filename.c_str(), "RECREATE");
    CalculatedValues c;
    std::vector<double> cv;

    //TH1D *histo = new TH1D("histo", "Pedestals", 1000, 0, 1000);
    //histo -> SetCanExtend(TH1::kAllAxes);

    TH1F *histo = new TH1F("histo", "Pedestals", 2000, 0, 50);
    histo -> GetXaxis() -> SetTitle("Pedestal values");
    histo -> GetYaxis() -> SetTitle("Number of Pedestals");
    histo -> SetCanExtend(TH1::kAllAxes);

    TH1F *histo2 = new TH1F("histo2", "Peaks", 2000, 0, 50);
    histo2 -> GetXaxis() -> SetTitle("Peak values");
    histo2 -> GetYaxis() -> SetTitle("Number of Peaks");
    histo2 -> SetCanExtend(TH1::kAllAxes);

    std::vector<double> x_axis;
    int i = 0;

    if(data_file.is_open())
    {
        while (data_file >> c)
        {
            std::cout << c;
            cv.push_back(c.pedestal);
            x_axis.push_back(i);
            histo -> Fill(c.pedestal);
            histo2 -> Fill(c.peakval);
            i++;
        }
    }
    else
    {
        cout << "File cannot be opened. Check if it exists." << endl;
        return -1;
    }
    

    auto n = cv.size();
    const double *y = &cv[0];
    const double *x = &x_axis[0];
    cout << "Data converted. Points: " << n << endl;
    TCanvas *c1 = new TCanvas("c1", "Pulses", 200, 10, 500, 300);
    TGraph *gr = new TGraph(n, x, y);
    gr->GetXaxis()->SetTitle("Time (ns)");
    gr->GetYaxis()->SetTitle("ADC");
    gr->Draw("AP");

    //Write it into the file
    gr -> Write("Pedestal");
    TCanvas *canvas = new TCanvas("canvas", "Pedestals and peaks", 200, 10, 500, 300);
    canvas -> Divide(2, 1);
    canvas -> cd(1);
    histo -> Draw("HIST");
    canvas -> cd(2);
    histo2 -> Draw("HIST");
    f.Write();
    if (batch_mode)
    {
        return 0;
    }
    cout << "Done. Press Ctrl + C for exit. You can check the histograms if you run this code with X11 enabled." << endl;
    while(!batch_mode)
    {
        gSystem -> ProcessEvents();
    }
    //App.Run();
}