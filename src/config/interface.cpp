//-----------------------------------------------------------------------------
//
// This source file is part of the VAMPIRE open source package under the
// GNU GPL (version 2) licence (see licence file for details).
//
// (c) R F L Evans 2015. All rights reserved.
//
//-----------------------------------------------------------------------------

// C++ standard library headers
#include <sstream>

// Vampire headers
#include "errors.hpp"
#include "config.hpp"
#include "vio.hpp"

// config headers
#include "internal.hpp"

namespace config{

   //-----------------------------------------------------------------------------
   // Function to process input file parameters for ltmp settings
   //-----------------------------------------------------------------------------
   bool match_input_parameter(std::string const key, std::string const word, std::string const value, std::string const unit, int const line){

      // Check for valid key, if no match return false
      std::string prefix="config";
      if(key!=prefix) return false;

      //----------------------------------
      // Now test for all valid options
      //----------------------------------
      std::string test="atoms";
      if(word==test){
         config::internal::output_atoms = true;
         config::internal::output_meta = true;
         return true;
      }
      //-----------------------------------------
      test="coordinates";
      if(word==test){
         config::internal::output_coords = true;
         return true;
      }
      //-----------------------------------------
      test="output-rate";
      if(word==test){
         int i=atoi(value.c_str());
         vin::check_for_valid_int(i, word, line, prefix, 1, 1000000,"input","1 - 1,000,000");
         config::internal::output_rate=i;
         return true;
      }
      //--------------------------------------------------------------------
      test="atoms-minimum-x";
      if(word==test){
         double x=atof(value.c_str());
         vin::check_for_valid_value(x, word, line, prefix, "", "none", 0.0, 1.0,"input","0.0 - 1.0");
         config::internal::atoms_output_min[0]=x;
         return true;
      }
      //--------------------------------------------------------------------
      test="atoms-minimum-y";
      if(word==test){
         double y=atof(value.c_str());
         vin::check_for_valid_value(y, word, line, prefix, "", "none", 0.0, 1.0,"input","0.0 - 1.0");
         config::internal::atoms_output_min[1]=y;
         return true;
      }
      //--------------------------------------------------------------------
      test="atoms-minimum-z";
      if(word==test){
         double z=atof(value.c_str());
         vin::check_for_valid_value(z, word, line, prefix, "", "none", 0.0, 1.0,"input","0.0 - 1.0");
         config::internal::atoms_output_min[2]=z;
         return true;
      }
      //--------------------------------------------------------------------
      test="atoms-maximum-x";
      if(word==test){
         double x=atof(value.c_str());
         vin::check_for_valid_value(x, word, line, prefix, "", "none", 0.0, 1.0,"input","0.0 - 1.0");
         config::internal::atoms_output_max[0]=x;
         return true;
      }
      //--------------------------------------------------------------------
      test="atoms-maximum-y";
      if(word==test){
         double y=atof(value.c_str());
         vin::check_for_valid_value(y, word, line, prefix, "", "none", 0.0, 1.0,"input","0.0 - 1.0");
         config::internal::atoms_output_max[1]=y;
         return true;
      }
      //--------------------------------------------------------------------
      test="atoms-maximum-z";
      if(word==test){
         double z=atof(value.c_str());
         vin::check_for_valid_value(z, word, line, prefix, "", "none", 0.0, 1.0,"input","0.0 - 1.0");
         config::internal::atoms_output_max[2]=z;
         return true;
      }
      //--------------------------------------------------------------------
      test="macro-cells";
      if(word==test){
         config::internal::output_cells=true;
         return true;
      }
      //--------------------------------------------------------------------
      test="format";
      if(word==test){
         //----------------------------------------------
         test="binary";
         if(value==test){
            config::internal::output_data_format = config::internal::binary;
         }
         //----------------------------------------------
         test="text";
         if(value==test){
            config::internal::output_data_format = config::internal::text;
         }
         return true;
      }
      //-------------------------------------------------------------------
      //test="identify-surface-atoms";
      //if(word==test){
      //   sim::identify_surface_atoms=true;
      //   return EXIT_SUCCESS;
      //}
      //-----------------------------------------
      
      // config:output-processes-per-node = n
      // config:output-non-magnetic-atoms = true/false
      // mpi:ppn

      return false;

   }
} // end of namespace config