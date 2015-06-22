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
   std::vector<reco::GenParticle> radiatedPhotons;
   for ( const auto& particle : *genParticles )
   {
      if ( particle.status() == 1 
            && ( abs(particle.pdgId()) == 11 || abs(particle.pdgId()) == 13 || abs(particle.pdgId()) == 15 )
         )
      {
         leptons.push_back(particle);
      }
      else if ( particle.status() == 1
            && particle.pdgId() == 22 and abs(particle.mother(0)->pdgId()) < 25
         )
      {
         radiatedPhotons.push_back(particle);
      }
   }
   if ( leptons.size() == 0 || radiatedPhotons.size() == 0 ) return true;

   auto fsrDefinition = [](int mother, int grandmother) {
      if ( (abs(mother)==11 || abs(mother)==13 || abs(mother)==15 || abs(mother)==22)
          && (abs(grandmother)==22 || abs(grandmother)==23 || grandmother==mother) )
      {
         return true;
      }
      return false;
   };

   auto isrDefinition = [](int mother, int grandmother) {
      if ( (abs(mother)<7 || abs(mother)==21)
          || ( abs(mother)==22 && (abs(grandmother)<7 || abs(grandmother)==21) ) )
      {
         return true;
      }
      return false;
   };

   for ( auto& photon : radiatedPhotons )
   {
      int mother = photon.mother(0)->pdgId();
      int grandmother = 0;
      if ( photon.mother(0)->mother(0) != nullptr )
         grandmother = photon.mother(0)->mother(0)->pdgId();

      if ( fsrDefinition(mother, grandmother) || isrDefinition(mother, grandmother) )
      {
         double smallestdR = reco::deltaR(photon, leptons[0]);
         for ( auto& lepton : leptons ) 
            if ( reco::deltaR(photon, lepton) < smallestdR )
               smallestdR = reco::deltaR(photon, lepton);
         
         if ( photon.pt() > 10 && smallestdR > 0.4 ) return false;
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
