/// \file AmBe/src/DetectorConstruction.cc
/// \brief Implementation of the AmBeStack::DetectorConstruction class

#include "DetectorConstruction.hh"
// also include headers related to 
// sensitive detector logic
#include "ScintillatorSD.hh"
#include "G4SDManager.hh"

#include "G4Box.hh"
#include "G4Cons.hh"
#include "G4LogicalVolume.hh"
#include "G4NistManager.hh"
#include "G4PVPlacement.hh"
#include "G4SystemOfUnits.hh"
#include "G4Trd.hh"
#include "G4Tubs.hh"
#include "G4SubtractionSolid.hh"
#include "G4ProductionCuts.hh"
#include "globals.hh"
#include "G4RotationMatrix.hh"




// include G4GDMLParser for visualisation of geometry
// #include "G4GDMLParser.hh"
namespace AmBeStack // changed from B1 to AmBe since this is the AmBe simulation model
{

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4VPhysicalVolume* DetectorConstruction::Construct()
{    
     // Going to try and organise things with {} scopes
     // NOTE: when things are defined with "new" they can be looked up in a different scope
     // based on their name
             // !!!!!!!!!!!!! for debugging !!!!!!!!!!
        //G4cout << "Coming from Construct" << G4endl;


     // declare essential physical and logical volumes
     // at top level so they can be accessed across each block
     // for each component of the geometry below
     G4VPhysicalVolume* physWorld = nullptr;
     G4LogicalVolume* logicWorld = nullptr;
     G4LogicalVolume* logicRoom = nullptr;

     // Construct custom materials and add to global table (can then look up later)
     // in this case just the C6D6 for detector 
     // and AmBe for source itself
     {
          // Get nist material manager
          G4NistManager* nist = G4NistManager::Instance();


          // Start with making C6D6 for detector
          // need to initialise various properties of the material
          // atomic number, atomic mass, nucleon number, mass_density
          G4double z, a, n, mass_density;
          // name and symbol for the molecule
          G4String name, symbol;
          // number of components (isotopes) and number of atoms per molecule
          G4int ncomponents, natoms;
          // avogadros constant for use
          G4double avogadro = 6.02214076e23 / mole;

          // make C6D6
          G4double a_D = 2.0141 *g/mole;
          G4double a_H = 1.0078 *g/mole;
          G4double a_C = 12.01 *g/mole; // natural carbon
          // NOTE: n is total nucleon number rather than neutron number
          G4Isotope* iso_D = new G4Isotope(
               "Deuterium", // name  
               1, // atomic number
               2, // nucleon number
               a_D // atomic mass
          );
          G4Isotope* iso_H = new G4Isotope(
               "Hydrogen", // name
               1, // atomic number
               1, // nucleon number 
               a_H // atomic mass
          );
          // initialise the deuterium element (with the hydrogenous impurities stated)
          // (last argument is how many components (isotopes) belong to it)
          G4Element* el_D_imp = new G4Element(
               "Deuterium_with_H_imp", // name 
               "D", // symbol 
               2 // number of components (isotopes)
          );
          // add the components (isotopes) to the element
          el_D_imp -> AddIsotope(iso_D, 0.984); // 98.4% pure deuterium
          el_D_imp -> AddIsotope(iso_H, 1-0.984);
          // get the (premade) natural carbon element from nist
          G4Element* el_C = nist -> FindOrBuildElement("C");
          // now go ahead and define atomic densities
          G4double n_D = 4.1e22 /cm3;
          G4double n_H = 0.0287e20 /cm3;
          G4double n_C = 4.1e22 /cm3;
          // define macroscopic density of material using values (and units) above 
          mass_density = (((a_D * n_D) + (a_H * n_H) + (a_C * n_C)) / avogadro);
          // initialise full C6D6 material for active volume of detector
          G4Material* deut_benz = new G4Material(
               "Deuterated_Benzene", // name 
               mass_density, // mass density
               2 // number of components (elements)
          ); 
          // add the elemental components (el_C and el_D) defined above to this material
          deut_benz -> AddElement(el_C, 6); // elemental components (and how many atoms of each)
          deut_benz -> AddElement(el_D_imp, 6);
          // print to make sure correct
          G4cout << "mass density of deut_benz: "
               << deut_benz ->GetDensity() / (g/cm3)
               << " g/cm3" << G4endl;
          G4cout << "mass density of deut_benz: "
               << mass_density / (g/cm3)
               << " g/cm3" << G4endl;
          G4cout << "Total atoms per volume in deut_benz: "
               << deut_benz ->GetTotNbOfAtomsPerVolume()*cm3/mm3
               << " atoms/cm3" << G4endl;


          // Make AmBe mixture for source itself
          // first need Americium dioxide 
          // assume pure 241Am (same as in Filippo's paper)
          G4double a_Am241 = 241.0568 *g/mole;
          G4double a_O16 = 15.9949 *g/mole;
          G4Isotope* iso_Am = new G4Isotope(
               "Americium-241", // name
               95, // atomic number 
               241, // nucleon number
               a_Am241 // atomic mass
          );
          G4Isotope* iso_O = new G4Isotope(
               "Oxygen-16", // name
               8, // atomic number
               16, // nucleon number
               a_O16 // atomic mass
          );
          // add these isotopes to an element for each
          G4Element* el_Am = new G4Element(
               "Americium", // name
               "Am", // symbol
               1 // no. of components
          );
          el_Am->AddIsotope(iso_Am, 1.0);
          G4Element* el_O = new G4Element(
               "Oxygen",
               "O",
               1
          );
          el_O->AddIsotope(iso_O, 1);
          // can just define AmO2 mass density 
          // with set value
          // will be overwritten anyway to accomodate
          // rhoAmBe below
          G4double rho_AmO2 = 11.68 *g/cm3; 
          G4Material* mat_AmO2 = new G4Material(
               "Americium_Dioxide", // name
               rho_AmO2, // mass density
               2 // number of components (isotopes)
          );
          mat_AmO2 -> AddElement(el_Am, 1); // 1 atom of Am per molecule
          mat_AmO2 -> AddElement(el_O, 2); // 2 atoms of O per molecule
          // now define beryllium-9 material
          G4Element* el_Be = nist -> FindOrBuildElement("Be");
          // define mass density of mixture 
          G4double rho_AmBe = 2.927 *g/cm3;
          G4Material* AmBe = new G4Material(
               "AmBe", // name
               rho_AmBe, // mass density
               2 // number of components (elements)
          );
          AmBe -> AddMaterial(mat_AmO2, 10.97 *perCent); // fractional mass of AmO2 in mixture (from Filippo's paper)
          AmBe -> AddElement(el_Be, 89.03 *perCent); // fractional mass of Be in mixture
          // print to make sure correct
          G4cout << "mass density of AmBe :"
               << AmBe -> GetDensity() / (g/cm3)
               << " g/cm3" <<G4endl;


     }

     // World  and room volume
     {
          // make surrounding room volume full of air say 2.5 x 2.5 x 2.5 m3
          // should be just slightly bigger than the stack
          // this will define the envelope where things are measured
          G4double room_length = 2.5 *m; 
          // world just slightly larger
          G4double world_length = 1.1 * room_length;

          // make solid volume for world
          // NOTE: Geant defines boxe sizes using half-lengths
          auto solidWorld = new G4Box(
               "World_solid", // name
               0.5 * world_length, // half length in x
               0.5 * world_length, // half length in y
               0.5 * world_length // half length in z
          );

          // assign material to world volume (will be air)
          G4NistManager* nist = G4NistManager::Instance();
          G4Material* air = nist -> FindOrBuildMaterial("G4_AIR");

          // make logical volume for world
          // no auto here as already declared at top of function
          logicWorld = new G4LogicalVolume(
               solidWorld,         // associated solid volume 
               air,                // material
               "World_logic"       // name
          );

          // now make physical volume for the world
          // again no auto here as already declared at top of function
          physWorld = new G4PVPlacement(
               nullptr,            // no rotation
               G4ThreeVector(),    // at origin (0,0,0)
               logicWorld,         // logical volume
               "World_phys",       // name
               nullptr,            // its mother volume
               false,              // not part of a Boolean volume
               0,                  // copy number (for referencing)
               true                // overlaps check turned on
          );


          // Room volumes
          // solid 
          auto solidRoom = new G4Box(
               "Room_solid", // name
               0.5 * room_length, // half length in x
               0.5 * room_length, // half length in y
               0.5 * room_length // half length in z
          );

          // logical
          // again no auto here as already declared at top of function
          logicRoom = new G4LogicalVolume(
               solidRoom, // associated solid volume
               air, // material
               "Room_logic" // name
          );

          // physical
          auto physRoom = new G4PVPlacement(
               nullptr, // no rotation applied
               G4ThreeVector(), // position is origin, no offset needed
               logicRoom, // associate logical volume
               "Room_phys", // name
               logicWorld, // mother logical volume
               false, // not part of boolean         
               0, // copy number
               true // check overlaps
          );

     }

     // Stack volumes
     {
          // define graphite material
          // will just use nist default for now as has same density (1.7g/cm3) as i set in openmc
          // can manually define later if like
          G4NistManager* nist = G4NistManager::Instance();
          G4Material* graphite = nist -> FindOrBuildMaterial("G4_GRAPHITE_POROUS");

          // define dimensions
          G4double stack_widthY = 1.59 *m;
          G4double stack_heightX = 2.38 *m;
          G4double stack_depthZ = 1.42 *m; 
          G4double hole_radius = 3.5 *cm; // to match openmc, can change later
          
          // solid
          // make box first
          auto stack_box = new G4Box(
               "Stack_box",
               0.5 * stack_heightX, 0.5 * stack_widthY, 0.5 * stack_depthZ
          );
          // make cylinder for source detector placement
          // make it slightly larger than box depth to ensure clean 'cut' when we subtract
          auto stack_hole = new G4Tubs(
               "Stack_hole",
               0.,                 // inner radius
               hole_radius,        // outer radius
               (stack_depthZ + 0.2*cm)/2, // half length
               0. *deg,            // starting angle
               360. *deg           // spanning angle
          );
          // subtract hole from box to get graphite solid region
          auto solidStack = new G4SubtractionSolid(
               "Stack_solid", // name
               stack_box, // solid to subtract from
               stack_hole // solid to subtract
               // no need to offset as both solids are centred at same point
          );
          
          // logical
          auto logicStack = new G4LogicalVolume(
               solidStack, // associated solid volume
               graphite, // material
               "Stack_logic" // name
          );

          // physical
          auto physStack = new G4PVPlacement(
               nullptr, // no rotation applied
               G4ThreeVector(), // position is origin of volume, so no offset needed 
               logicStack, // associated logical volume
               "Stack_phys", // name
               logicRoom, // mother logical volume
               false, // not part of a boolean
               0, // copy number
               true // check overlaps
          );

     }

     // Detector and shielding volumes
     {
          // look up C6D6 material made earlier from table
          G4Material* deut_benz = G4Material::GetMaterial("Deuterated_Benzene");
          // get aluminium for casing
          G4NistManager* nist = G4NistManager::Instance();
          G4Material* aluminium = nist -> FindOrBuildMaterial("G4_Al");
          // get cadmium for shield
          G4Material* cadmium = nist -> FindOrBuildMaterial("G4_Cd");
          
          // define dimensions
          // worth measuring physical detector 
          // when next get chance, capsule dimensions are 5cm 
          // but surrounding components might be slightly larger
          G4double det_case_thickness = 1.5 *mm;
          G4double det_tot_diameter = 5 *cm;
          G4double det_tot_length = 5 *cm;
          G4double det_active_diameter = det_tot_diameter - (2 * det_case_thickness);
          G4double det_active_length = det_tot_length - det_case_thickness;
          // will make shield fit snug with detector
          G4double shield_thickness = 0.5 *cm;
          G4double shield_diameter = det_tot_diameter + (2 * shield_thickness);
          G4double shield_length = det_tot_length + shield_thickness;

          // define position
          // NOTE: position for G4PVPlacement is defined as the centre of the volume (see below)
          // first detector
          G4ThreeVector det_face_pos_1 = G4ThreeVector(0, 0, 75 *cm);

          // second detector will be outside stack and to the side
          G4ThreeVector det_face_pos_2 = G4ThreeVector(0, 50 *cm, 100 *cm); 

          // get a vector for displacement between detectors
          G4ThreeVector det_displacement = det_face_pos_2 - det_face_pos_1;
          // second detector also rotated so will define rotation matrix here
          // detector needs to be rotate relative to first
          // angle should be based on displacement between two detectors
          // det 1 is at (0,0,5) 
          // det 2 is at (0,50,100)
          // angle is therefore arctan(50/95)
          // make rotation matrix
          G4RotationMatrix* rotation = new G4RotationMatrix();
          // this is defined around the central axis of the solid volume
          // so need to rotate around x axis
          rotation -> rotateX(std::atan(det_displacement[1]/det_displacement[2]) *radian);// default is clockwise, negative to get correct CCW rotation     

          // create active detector volume 
          // solid
          auto solidDetActive = new G4Tubs(
               "Det_Active_solid",
               0., // inner radius
               0.5 * det_active_diameter, // outer radius
               0.5 * det_active_length, // half length
               0. *deg, // starting angle
               360. *deg // spanning angle
          );
          
          // logical
          auto logicDetActive = new G4LogicalVolume(
               solidDetActive, // associated solid volume
               deut_benz, // material
               "Det_Active_logic" // name
          );

          // physical
          auto physDetActive_1= new G4PVPlacement(
               nullptr, // no rotation applied
               // position is center of volume, so offset by half the active length in z
               // to get the face of detector at position defined above
               // add thickness as det_face_pos is position of face (including casing)
               det_face_pos_1 + G4ThreeVector(0, 0, (0.5 *det_active_length) + det_case_thickness),
               logicDetActive, // associated logical volume
               "Det_Active_phys_1", // name
               logicRoom, // mother logical volume 
               false, // not part of a boolean
               0, // copy number
               true // check overlaps
          );

          // create detector casing
          // will need to do subtraction with active volume above
          // solid
          // make outer cylinder
          auto solidDetCaseOuter = new G4Tubs(
               "Det_Case_outer", // name
               0., // inner radius
               0.5 * det_tot_diameter, // outer radius
               0.5 * det_tot_length, // half length
               0. *deg, // starting angle
               360. *deg // spanning angle
          );

          // must offset the subtraction since Geant takes the subtraction from the
          // centre of the solids 
          // (we want a 1.5mm window at the front between the face and the active part)
          // (and no window at the back of the detector)
          // the offset tells the SubtractionSolid function how much to shift the inner solid wrt the outer
          auto solidDetCase = new G4SubtractionSolid(
               "Det_Case_solid", // name
               solidDetCaseOuter, // solid to subtract from
               solidDetActive, // solid to subtract
               0, // no rotation 
               G4ThreeVector(0,0, (0.5 * det_case_thickness)) // shift active volume forward by half the case thickness for subtraction
               // (otherwise both solids centered at same point and would get window at both front and back)
          );

          // logical
          auto logicDetCase = new G4LogicalVolume(
               solidDetCase, // associated solid volume
               aluminium, // material
               "Det_Case_logic" // name
          );

          // physical
          auto physDetCase_1 = new G4PVPlacement(
               nullptr, // no rotation applied
               // position is the centre of the volume, so offset by half the total length in z 
               // to get face of detector at det_face_pos defined above
               det_face_pos_1 + G4ThreeVector(0, 0, 0.5 * det_tot_length),
               logicDetCase, // associated logical volume
               "Det_Case_phys_1", // name
               logicRoom, //mother logical volume
               false, // not part of a boolean
               0, // copy number
               true // check overlaps
          );

          // create detector shield (will subtract solidCaseOuter)
          auto solidShieldOuter = new G4Tubs(
               "Shield_outer", // name
               0., // inner radius
               0.5 * shield_diameter, // outer radius
               0.5 * shield_length, // half length
               0. *deg, // starting angle
               360. *deg // spanning angle
          );

          // similarly to what we did above with the casing offset,
          // shift the shield so it covers the front but leaves the rear of the detector 
          // (i'm leaving the rear incase i implement a pmt geometry later)
          auto solidShield = new G4SubtractionSolid(
               "Shield_solid", // name
               solidShieldOuter, // solid to subtract from
               solidDetCaseOuter, // solid to subtract (use case outer as want to shield whole detector including casing)
               0, // no rotation
               G4ThreeVector(0,0, (0.5 * shield_thickness)) // shift casing volume forward by half the shield thickness for subtraction
               // (otherwise both solids centred at same point and shield wouldn't be flush with front of detector)
          );

          // logical
          auto logicShield = new G4LogicalVolume(
               solidShield, // associated solid volume
               cadmium, // material
               "Shield_logic" // name
          );

          // physical
          auto physShield_1 = new G4PVPlacement(
               nullptr, // no rotation applied
               // position is centre of volume, so offset by half shield length in z
               // to get front of shield flush with front of detector at det_face_pos
               det_face_pos_1 + G4ThreeVector(0, 0, (0.5 * shield_length) - shield_thickness),
               logicShield, // associated logical volume
               "Shield_phys_1", // name
               logicRoom, // mother logical volume
               false, // not part of a boolean
               0, // copy number
               true // check overlaps
          );


          // now for second detector, solid and logical volumes are the same,
          // but need new copy of physical vol at new position
          
          // physical
          auto physDetActive_2 = new G4PVPlacement(
               // include rotation defined above for second detector
               rotation,
               // position is center of volume, need to offset by half active length
               det_face_pos_2 + G4ThreeVector(0, 0, (0.5 *det_active_length) + det_case_thickness),
               logicDetActive, // associated logical volume
               "Det_Active_phys_2", // name
               logicRoom, // mother logical volume
               false, // not part of a boolean
               1, // copy number (set to 1 to differentiate from first detector)
               true // check overlaps
          );

          // physical detector casing volume now in same way
          auto physDetCase_2 = new G4PVPlacement(
               rotation, // include rotation defined above
               // position is center of volume, so offset by half the total length in z
               det_face_pos_2 + G4ThreeVector(0, 0, 0.5 * det_tot_length),
               logicDetCase, // associated logical volume
               "Det_Case_phys_2", // name
               logicRoom, // mother logical volume
               false, // not part of boolean
               1, // copy number (set to 1 to differentiate)
               true // check overlaps
          );

          // physical shield volume now in same way
          auto physShield_2 = new G4PVPlacement(
               rotation, // include rotation defined above
               // position is centre of volume, so offset by half shield length in z
               det_face_pos_2 + G4ThreeVector(0, 0, (0.5 * shield_length) - shield_thickness),
               logicShield, // associated logical volume
               "Shield_phys_2", // name
               logicRoom, // mother logical volume
               false, // not part of boolean
               1, // copy number (set to 1 to differentiate)
               true // check overlaps
          );

          // assign the scoring volume as the detector active region
          fScoringVolume = logicDetActive;

     }

     // AmBe source and casing volumes
     {
          // look up materials from earlier
          G4Material* AmBe = G4Material::GetMaterial("AmBe");
          // grab stainless steel from nist manager
          G4NistManager* nist = G4NistManager::Instance();
          G4Material* stainless_steel = nist -> FindOrBuildMaterial("G4_STAINLESS-STEEL");

          // define dimensions
          G4double ambe_active_diameter = 17.4 *mm;
          G4double ambe_active_length = 17.6 *mm;
          G4double ambe_case_diameter = 22.475 *mm;
          G4double ambe_case_length = 31.20 *mm;
          // thicknesses just for sanity checking really
          // pretty sure steel is flush against the active volume 
          // so should just be a standard subtraction
          G4double curved_surface_thickness = 2.5 *mm;
          G4double end_surface_thickness = 6.8 *mm;

          // create active AmBe volume
          // solid volume 
          auto solidSourceActive = new G4Tubs(
               "Source_Active_solid", // name
               0., // inner radius 
               0.5 * ambe_active_diameter, // outer radius
               0.5 * ambe_active_length, // half length
               0. *deg, // starting angle
               360. *deg // spanning angle
          );

          // logical
          auto logicSourceActive = new G4LogicalVolume(
               solidSourceActive, // associated solid volume
               AmBe, // material
               "Source_Active_logic" // name
          );

          // physical
          auto physSourceActive = new G4PVPlacement(
               nullptr, // no rotation applied
               // position is origin of volume (for now), so can just plate at origin
               G4ThreeVector(), 
               logicSourceActive, // assocated logical volume
               "Source_Active_phys", // name
               logicRoom, // mother logical volume
               false, // not part of a boolean
               0, // copy number
               true // check overlaps
          );

          // create steel casing
          // define outer cylinder to subtract the active solid volume from
          auto solidCaseOuter = new G4Tubs(
               "Case_outer", // name
               0., // inner radius
               0.5 * ambe_case_diameter, // outer radius
               0.5 * ambe_case_length, // half length
               0. *deg, // starting angle
               360. *deg // spanning angle
          );

          // subtract
          auto solidSourceCase = new G4SubtractionSolid(
               "Source_Case_solid", // name
               solidCaseOuter, // solid to subtract from
               solidSourceActive // solid to subtract
               // no need to offset as we want steel to be flush with active volume
               // and both volumes are symmetrical about same point
          );

          // logical
          auto logicSourceCase = new G4LogicalVolume(
               solidSourceCase, // associated solid volume
               stainless_steel, // material
               "Source_Case_logic" // name
          );

          auto physSourceCase = new G4PVPlacement(
               nullptr, // no rotation applied
               G4ThreeVector(), // position is origin once more, so just place at empty threevector
               logicSourceCase, // associated logical volume
               "Source_Case_phys", // name
               logicRoom, // mother logical volume
               false, // not part of a boolean
               0, // copy number
               true // check overlaps
          );


     }
     
     // return the physWorld (everything within it can be looked up)
     return physWorld;


}

// constructSD member function implementation
// will actually assign the logical volumes defined in construct
// to our sensitive detector hit logic
void DetectorConstruction::ConstructSDandField()
{
     // get an instance of the sensitive detector manager 
     G4SDManager* sdManager = G4SDManager::GetSDMpointer();

     // make instance of the ScintillatorSD class we've made
     // requires 1 argument, the detector name
     G4String sdName = "OrganicSD";
     ScintillatorSD* scintillatorSD = new ScintillatorSD(sdName);

     // once we have this, pass it to the SDManager
     // to register the detector 
     sdManager->AddNewDetector(scintillatorSD);

     // finally, attach this sensitive detector to the logical volume
     // we made in construct() member function
     SetSensitiveDetector(fScoringVolume, scintillatorSD);
}

}