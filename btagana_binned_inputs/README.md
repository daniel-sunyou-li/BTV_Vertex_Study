# Histogram and 2D KS p-value plots for various b-tag analyzer inputs

## Instructions
1. Run the [b-tag analyzer ntuplizer](https://github.com/cms-btv-pog/RecoBTag-PerformanceMeasurements) with the following options/modifications (lxpus recommended):
> * Create a new [default](https://github.com/cms-btv-pog/RecoBTag-PerformanceMeasurements/tree/9_4_X/python/defaults) configuration for 2016/2017/2018 with the corresponding miniAOD sample you would like to run
> * Create a new [variable group](https://github.com/cms-btv-pog/RecoBTag-PerformanceMeasurements/blob/9_4_X/python/varGroups_cfi.py) with all relevant inputs desired (i.e. TagVarCSV_\*, PV_\*, SV_\*, GenPVz)
2. Run with the options:
> * `runOnData=False`
> * `defaults=2017_UltraLegacy`
> * `runEventInfo=True`
3. Upload the output ntuple `ROOT` file to [SWAN](swan.cern.ch) and run the three notebooks (as needed)
> * Methods are defined in `.py` files