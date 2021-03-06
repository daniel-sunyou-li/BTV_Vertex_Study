#include <iostream>
#include <fstream>
#include "CommonTools/RecoAlgos/interface/PrimaryVertexSorting.h"
#include "DataFormats/VertexReco/interface/Vertex.h"
#include "TrackingTools/TransientTrack/interface/TransientTrackBuilder.h"
#include "DataFormats/Math/interface/deltaR.h"
#include "TrackingTools/IPTools/interface/IPTools.h"
#include <fastjet/internal/base.hh>
#include "fastjet/PseudoJet.hh"
#include "fastjet/JetDefinition.hh"
#include "fastjet/ClusterSequence.hh"
#include "fastjet/Selector.hh"
#include "fastjet/PseudoJet.hh"
#include "FWCore/Utilities/interface/isFinite.h"

using namespace fastjet;
using namespace std;

float PrimaryVertexSorting::score(const reco::Vertex & pv,const  std::vector<const reco::Candidate *> & cands, bool useMet ) const {
  typedef math::XYZTLorentzVector LorentzVector;
  float sumPt2=0;
  float sumEt=0;
  float sumPt2_lep=0;
  float sumPt2_met=0;
  float sumPt2_jet=0;

// check if output .txt file exists
  FILE *file;
  const char * filename = "particle_ID.txt";
  int exists;
  file = fopen( filename, "r" );
  if ( file == NULL ) exists = 0;
  else { exists = 1; fclose( file ); }

  if ( exists == 1 ) { file = fopen( filename, "r+b" ); }
  else { file = fopen( filename, "w+b" ); }

  if ( file != NULL ){ fclose( file ); }

  std::ifstream check_file( "particle_ID.txt" );
  std::ofstream data_file;
  data_file.open( "particle_ID.txt", std::ios_base::app );
  char columns[100];
  int n = sprintf( columns, "%9s, %9s, %9s, %9s, %9s\n", "TOT", "JET", "MET", "LEP", "PDGID" );
  if ( check_file.peek() == std::ifstream::traits_type::eof() ){ data_file << columns; }
  
  LorentzVector met;
  std::vector<fastjet::PseudoJet> fjInputs_;
  fjInputs_.clear();
  size_t countScale0 = 0;
  std::string particles = "";
  for (size_t i = 0 ; i < cands.size(); i++) {
    const reco::Candidate * c= cands[i];
    float scale=1.;
    if(c->bestTrack() != nullptr)
      {
        if(c->pt()!=0) {
                 scale=(c->pt()-c->bestTrack()->ptError())/c->pt();
        }
        if(edm::isNotFinite(scale)) {
                edm::LogWarning("PrimaryVertexSorting") << "Scaling is NAN ignoring this candidate/track" << std::endl;
                scale=0;
        }
        if(scale<0){
          scale=0;
          countScale0++;
        }
      }
    
    int absId=abs(c->pdgId());
    particles.append( std::to_string( absId ).append( " " ) );
    if(absId==13 or absId == 11) {
      float pt =c->pt()*scale;
      sumPt2+=pt*pt;
      met+=c->p4()*scale;
      sumEt+=c->pt()*scale;
      sumPt2_lep+=pt*pt;
    } else {
      if (scale != 0){ // otherwise, what is the point to cluster zeroes
        fjInputs_.push_back(fastjet::PseudoJet(c->px()*scale,c->py()*scale,c->pz()*scale,c->p4().E()*scale));
        //      fjInputs_.back().set_user_index(i);
      }
    }
  }
  fastjet::ClusterSequence sequence( fjInputs_, JetDefinition(antikt_algorithm, 0.4));
  auto jets = fastjet::sorted_by_pt(sequence.inclusive_jets(0));
  for (const auto & pj : jets) {
    auto p4 = LorentzVector( pj.px(), pj.py(), pj.pz(), pj.e() ) ;
    sumPt2+=(p4.pt()*p4.pt())*0.8*0.8;
    met+=p4;
    sumEt+=p4.pt();
    sumPt2_jet+=(p4.pt()*p4.pt())*0.8*0.8;
  }
  float metAbove = met.pt() - 2*sqrt(sumEt);
  if(metAbove > 0 and useMet) {
    sumPt2+=metAbove*metAbove;
    sumPt2_met+=metAbove*metAbove;
  }
  if (countScale0 == cands.size()) sumPt2 = countScale0*0.01; //leave some epsilon value to sort vertices with unknown pt
//  std::cout << "TOT: " << sumPt2 << " / JET: " << sumPt2_jet << " / MET: " << sumPt2_met << " / LEP: " << sumPt2_lep << std::endl;

  char buffer[500];
  int m = sprintf( buffer, "%8.3f, %8.3f, %8.3f, %8.3f, %s\n", sumPt2, sumPt2_jet, sumPt2_met, sumPt2_lep, particles.c_str() );
  data_file << buffer;
  data_file.close();

  return sumPt2;
}

float PrimaryVertexSorting::score_lep(const reco::Vertex & pv, const std::vector<const reco::Candidate *> & cands ) const {
  float sumPt2_lep=0;
  size_t countScale0 = 0;
  for (size_t i = 0; i < cands.size(); i++){
    const reco::Candidate * c= cands[i];
    float scale=1.;
    if(c->bestTrack() != nullptr){
      if(c->pt()!=0){
        scale = ( c->pt() - c->bestTrack()->ptError() ) / c->pt();
      }
      if(edm::isNotFinite(scale)){
        scale=0;
      }
      if(scale<0){
        scale=0;
        countScale0++;
      }
    }
    int absId=abs(c->pdgId());
    if( absId==13 or absId==11 ) {
      float pt =c->pt()*scale;
      sumPt2_lep+=pt*pt;
    }
  }
  return sumPt2_lep;
}

float PrimaryVertexSorting::score_jet(const reco::Vertex & pv, const std::vector<const reco::Candidate *> & cands ) const {
  typedef math::XYZTLorentzVector LorentzVector;
  float sumPt2_jet=0;
  std::vector<fastjet::PseudoJet> fjInputs_;
  fjInputs_.clear();
  size_t countScale0 = 0;
  for (size_t i = 0; i < cands.size(); i++){
    const reco::Candidate * c = cands[i];
    float scale=1.;
    if( c->bestTrack() != nullptr ){
      if( c->pt() != 0 ){
        scale = ( c->pt() - c->bestTrack()->ptError() ) / c->pt();
      }
      if( edm::isNotFinite( scale ) ){
        scale = 0;
      }
      if( scale < 0 ){
        scale = 0;
        countScale0++;
      }
    }
    if ( scale != 0 ){
      fjInputs_.push_back( fastjet::PseudoJet( c->px()*scale, c->py()*scale, c->pz()*scale, c->p4().E()*scale ) );
    }
  }
  fastjet::ClusterSequence sequence( fjInputs_, JetDefinition(antikt_algorithm, 0.4));
  auto jets = fastjet::sorted_by_pt(sequence.inclusive_jets(0));
  for (const auto & pj: jets){
    auto p4 = LorentzVector( pj.px(), pj.py(), pj.pz(), pj.e() );
    sumPt2_jet+=p4.pt()*p4.pt()*0.8*0.8;
  }
  if ( countScale0 == cands.size() ) sumPt2_jet = countScale0*0.01;
  return sumPt2_jet;
}

float PrimaryVertexSorting::score_met(const reco::Vertex & pv, const std::vector<const reco::Candidate *> & cands ) const {
  float sumPt2_met=0;
  typedef math::XYZTLorentzVector LorentzVector;
  float sumEt=0;
  LorentzVector met;
  std::vector<fastjet::PseudoJet> fjInputs_;
  fjInputs_.clear();
  size_t countScale0 = 0;
  for (size_t i = 0; i < cands.size(); i++) {
    const reco::Candidate * c = cands[i];
    float scale=1.;
    if( c->bestTrack() != nullptr ){
      if( c->pt() != 0 ){
        scale = ( c->pt() - c->bestTrack()->ptError() ) / c->pt();
      }
      if( edm::isNotFinite( scale ) ){
        scale = 0;
      }
      if( scale < 0 ){
        scale = 0;
        countScale0++;
      }
    }
    int absId=abs(c->pdgId());
    if( absId==13 or absId==11 ){
      met+=c->p4()*scale;
      sumEt+=c->pt()*scale;
    } else {
      if( scale != 0 ){
        fjInputs_.push_back( fastjet::PseudoJet( c->px()*scale, c->py()*scale, c->pz()*scale, c->p4().E()*scale ) );
      }
    }
  }
  fastjet::ClusterSequence sequence( fjInputs_, JetDefinition(antikt_algorithm, 0.4) );
  auto jets = fastjet::sorted_by_pt(sequence.inclusive_jets(0));
  for (const auto & pj : jets) {
    auto p4 = LorentzVector( pj.px(), pj.py(), pj.pz(), pj.e() );
    met+=p4;
    sumEt+=p4.pt();
  }
  float metAbove = met.pt() - 2*sqrt(sumEt);
  if( metAbove > 0 ){
    sumPt2_met+=metAbove*metAbove;
  }
  if( countScale0 == cands.size() ) sumPt2_met = 0;
  return sumPt2_met;
}
  
  
  
  
  
  
  
  
