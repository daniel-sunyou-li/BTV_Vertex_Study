# copy the analysis scripts to the /test/ directory
echo Copying analysis scripts study_track_sorting.py, track_contents.py to CMSSW_10_6_16/src/CommonTools/RecoAlgos/test/
cp study_track_sorting.py ../../../
cp track_contents.py ../../../

# replace the pvSorting.py script
echo Updating CMSSW_10_6_16/src/CommonTools/RecoAlgos/test/pvSorting.py
cp pvSorting_track.py ../../../

# replace the PrimaryVertexSorter.h script
echo Updating CMSSW_10_6_16/src/CommonTools/RecoAlgos/plugins/PrimaryVertexSorter.h
rm ../../../../plugins/PrimaryVertexSorter.h
cp PrimaryVertexSorter.h ../../../../plugins/

# replace the sortedPrimaryVertices_cfi.py script
echo Updating CMSSW_10_6_16/src/CommonTools/RecoAlgos/python/sortedPrimaryVertices_cfi.py
rm ../../../../python/sortedPrimaryVertices_cfi.py
cp sortedPrimaryVertices_cfi.py ../../../../python/

# replace the PrimaryVertexSorting.cc script
echo Updating CMSSW_10_6_16/src/CommonTools/RecoAlgos/src/PrimaryVertexSorting.cc
rm ../../../../src/PrimaryVertexSorting.cc
cp PrimaryVertexSorting.cc ../../../../src/

# replace the PrimaryVertexSorting.h script
echo Updating CMSSW_10_6_16/src/CommonTools/RecoAlgos/interface/PrimaryVertexSorting.h
rm ../../../../interface/PrimaryVertexSorting.h
cp PrimaryVertexSorting.h ../../../../interface/

echo Recompiling the Primary Vertex Sorting scripts
cd ../../../../
scram b -j4
