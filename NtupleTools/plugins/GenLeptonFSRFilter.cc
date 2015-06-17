// -*- C++ -*-
//
// Package:    GenLeptonFSRFilter
// Class:      GenLeptonFSRFilter
// 
/**\class GenLeptonFSRFilter GenLeptonFSRFilter.cc FinalStateAnalysis/GenLeptonFSRFilter/src/GenLeptonFSRFilter.cc

 Description: [one line class summary]

 Implementation:
     [Notes on implementation]
*/
//
// Original Author:  Nick Smith
//         Created:  Fri Jun 12 02:15:46 CDT 2015
// $Id$
//
//


// system include files
#include <memory>

// user include files
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDFilter.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include "DataFormats/Math/interface/deltaR.h"

#include "DataFormats/HepMCCandidate/interface/GenParticle.h"

//
// class declaration
//

class GenLeptonFSRFilter : public edm::EDFilter {
   public:
      explicit GenLeptonFSRFilter(const edm::ParameterSet&);
      ~GenLeptonFSRFilter();

      static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);

   private:
      virtual void beginJob() ;
      virtual bool filter(edm::Event&, const edm::EventSetup&);
      virtual void endJob() ;
      
      virtual bool beginRun(edm::Run&, edm::EventSetup const&);
      virtual bool endRun(edm::Run&, edm::EventSetup const&);
      virtual bool beginLuminosityBlock(edm::LuminosityBlock&, edm::EventSetup const&);
      virtual bool endLuminosityBlock(edm::LuminosityBlock&, edm::EventSetup const&);

      // ----------member data ---------------------------

   edm::InputTag GenParticleTag_;
};

//
// constants, enums and typedefs
//

//
// static data member definitions
//

//
// constructors and destructor
//
GenLeptonFSRFilter::GenLeptonFSRFilter(const edm::ParameterSet& iConfig) :
      GenParticleTag_(iConfig.getUntrackedParameter<edm::InputTag> ("GenTag", edm::InputTag("genParticles")))
{

}


GenLeptonFSRFilter::~GenLeptonFSRFilter()
{
 
   // do anything here that needs to be done at desctruction time
   // (e.g. close files, deallocate resources etc.)

}


//
// member functions
//

// ------------ method called on each new Event  ------------
bool
GenLeptonFSRFilter::filter(edm::Event& iEvent, const edm::EventSetup& iSetup)
{
   using namespace edm;

   Handle<reco::GenParticleCollection> genParticles;
   iEvent.getByLabel(GenParticleTag_, genParticles);

   std::vector<reco::GenParticle> leptons;
   for ( const auto& particle : *genParticles )
   {
      if ( particle.status() == 1 
            && ( abs(particle.pdgId()) == 11 || abs(particle.pdgId()) == 13 || abs(particle.pdgId()) == 15 )
         )
      {
         leptons.push_back(particle);
      }
   }
   if ( leptons.size() == 0 ) return true;

   for ( const auto& genParticle : *genParticles )
   {
      int absmom = 0;
      int absgmom = absmom;
      if ( genParticle.mother(0) != nullptr )
      {
         absmom = genParticle.mother(0)->pdgId();
         if ( genParticle.mother(0)->mother(0) != nullptr )
            absgmom = genParticle.mother(0)->mother(0)->pdgId();
      }
      if ( genParticle.status() == 1 
            && genParticle.pdgId() == 22 
            && absmom > 0
            && ( absmom < 7 || absmom == 11 || absmom == 13 || absmom == 15 )
            && ( absgmom == absmom || absgmom == 22 || absgmom == 23 ) 
            && genParticle.pt() > 10.
         )
      {
         double smallestdR = reco::deltaR(genParticle, leptons[0]);
         for ( auto& lepton : leptons ) 
            if ( reco::deltaR(genParticle, lepton) < smallestdR )
               smallestdR = reco::deltaR(genParticle, lepton);
         
         if ( smallestdR > 0.4 ) return false;
      }
   }

   return true;
}

// ------------ method called once each job just before starting event loop  ------------
void 
GenLeptonFSRFilter::beginJob()
{
}

// ------------ method called once each job just after ending the event loop  ------------
void 
GenLeptonFSRFilter::endJob() {
}

// ------------ method called when starting to processes a run  ------------
bool 
GenLeptonFSRFilter::beginRun(edm::Run&, edm::EventSetup const&)
{ 
  return true;
}

// ------------ method called when ending the processing of a run  ------------
bool 
GenLeptonFSRFilter::endRun(edm::Run&, edm::EventSetup const&)
{
  return true;
}

// ------------ method called when starting to processes a luminosity block  ------------
bool 
GenLeptonFSRFilter::beginLuminosityBlock(edm::LuminosityBlock&, edm::EventSetup const&)
{
  return true;
}

// ------------ method called when ending the processing of a luminosity block  ------------
bool 
GenLeptonFSRFilter::endLuminosityBlock(edm::LuminosityBlock&, edm::EventSetup const&)
{
  return true;
}

// ------------ method fills 'descriptions' with the allowed parameters for the module  ------------
void
GenLeptonFSRFilter::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  //The following says we do not know what parameters are allowed so do no validation
  // Please change this to state exactly what you do use, even if it is no parameters
  edm::ParameterSetDescription desc;
  desc.setUnknown();
  descriptions.addDefault(desc);
}
//define this as a plug-in
DEFINE_FWK_MODULE(GenLeptonFSRFilter);
