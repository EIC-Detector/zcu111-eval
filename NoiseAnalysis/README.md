# ZCU111 data analysis

## Description
Reads and process the ZCU111 data files which are in LabView Measurement file format. The analysis contains three scripts, a python script, and two ROOT scripts.

The ADC input is terminated at 50Ohm to measure the intrinsic noise in the example files.

## Prerequisities
* CERN Root (tested on 6.18/00)
* Linux or MacOS (should work on Windows, not tested)
* Python
* [lvm_read](https://pypi.org/project/lvm_read/#files) module for Python.
* CMake

## Building
Source the appropriate file for ROOT before. (e.g. source /opt/sphenix/core/bin/sphenix_setup.sh)
* Clone this repository
```
git clone https://github.com/EIC-Detector/zcu111-eval.git
```
* Make a build directory (you can use these for example).
```
cd zcu111-eval
mkdir NoiseAnalysis/build && cd NoiseAnalysis/build
cmake ../
cp ../serialize_lvm.py .
make -jN (where N is the number of jobs to run simultaneously)
```

~~Before running the scripts, you need to copy the serialize_lvm.py file and if you want to use, the run.sh file to the build directory.~~
The CMake script will copy the necessary files. Maybe you need to adjust permissions:
```
chmod +x serialize_lvm.py
chmod +x run.sh
```

After a successful building there will be two executable. You need to run these in the following order:
* read_data filename_without_extension(lvm)
* read_pedestals filename_without_extension(txt)

You can use the run.sh script to do the analysis with one command. It will run the scripts in batch mode.

## How to use
### run.sh
This basic bash script will call the read_data and read_pedestals scripts in the right order to do the analysis. The output files are the same as if you run the scripts one-by-one.
Specify the file name without extension in the first argument. For example for data.lvm:
```
./run.sh data
```

### serialize_lvm.py
You do not need to run this code alone, but it works for creating only the serialized files. The first argument is the file name with extension (lvm).
```
python serialize_lvm.py input_filename.extension
```
If the serialized files are exists, this script does nothing. If you have any trouble with the previously generated files, remove them first. 
The generated files:
* input_filename_metadata.txt
* input_filename_serialized.txt

### read_data
The script requires the file name as argument without extension. It will call the python script to serialize the data, then open these files to analyse the ADCs output data.
The pedestal is defined using an average of 1000 ADC samples. The whole waveform and all of the fitted histograms are saved into a root file.
You can disable the python script with wp argument and can run it in batch mode with b argument. The file name must be the first argument!
For example:
```
./read_data filename_without_extension b //for batch mode
./read_data filename_without_estension wp //run without python script (this will only work if you have the serialized files)
./read_data filename_without_extension b wp //run in batch mode without python script
```
It has two output files:
* input_filename.txt (this contains the pedestal and peak values)
* input_filename.root (waveform and fitted histograms)

### read_pedestals
Requires one argument for the input file name without extension. It reads the pedestals and peaks from the previously generated text file and creates a TGraph with all of the pedestal values, and two histograms in a root file (peaks and pedestals):
* input_filename_pedestals.root

The output histograms contain the means and standard deviations of the peaks and pedestals.
The first argument must be the file name. You can run it in batch mode with b argument:
```
./read_pedestals filename_without_extension b
```

## Summary
Both of read_data and read_pedestals shows the histograms while they runs if you have X11 enabled, which can slow down the calculations. In that case you can interact with them, or just close the applications by pressing Ctrl + Z.
In batch mode, every histogram will be closed at the end of run, but will show up anyway.
Every ROOT file can be opened by TBrowser.