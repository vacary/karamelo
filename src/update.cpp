/* ----------------------------------------------------------------------
 *
 *                    ***       Karamelo       ***
 *               Parallel Material Point Method Simulator
 * 
 * Copyright (2019) Alban de Vaucorbeil, alban.devaucorbeil@monash.edu
 * Materials Science and Engineering, Monash University
 * Clayton VIC 3800, Australia

 * This software is distributed under the GNU General Public License.
 *
 * ----------------------------------------------------------------------- */

#include <iostream>
#include "update.h"
#include "scheme.h"
#include "method.h"
#include "input.h"
#include "var.h"
#include "style_scheme.h"
#include "style_method.h"
#include <vector>
#include "error.h"

using namespace std;

Update::Update(MPM *mpm) : Pointers(mpm)
{
  // cout << "In Update::Update()" << endl;

  atime = 0;
  atimestep = 0;
  ntimestep = 0; // set the current timestep to 0
  firststep = laststep = 0;
  beginstep = endstep = 0;
  first_update = 0;
  dt = 1e-16;
  dt_constant = false;
  dt_factor = 0.9;

  // Default scheme is MUSL:
  vector<string> scheme_args;
  scheme_args.push_back("musl");
  create_scheme(scheme_args);

  method = NULL;
  //vector<string> method_args;
  //method_args.push_back("tlmpm");
  //create_method(method_args);
}

Update::~Update()
{
  delete scheme;
  delete method;
}

void Update::set_dt_factor(vector<string> args){
  if (args.size()!=1) {
    error->all(FLERR, "Illegal dt_factor command: not enough arguments or too many arguments.\n");
  }
  dt_factor = input->parsev(args[0]);
}

void Update::set_dt(vector<string> args){
  if (args.size()!=1) {
    error->all(FLERR, "Illegal dt_factor command: not enough arguments or too many arguments.\n");
  }
  dt = input->parsev(args[0]);
  dt_constant = true;
  (*input->vars)["dt"] = Var("dt", dt);
}


/*! This function is the C++ equivalent to the scheme() user function.\n
 * Syntax: scheme(type)\n
 * It points the pointer Update::scheme to the desired Scheme type selected from style_scheme.h
 */
void Update::create_scheme(vector<string> args){
  if (args.size() < 1) {
    error->all(FLERR, "Illegal scheme command: not enough arguments.\n");
  }

  scheme_style = args[0];

  if (0) return;

#define SCHEME_CLASS
#define SchemeStyle(key,Class) \
  else if (scheme_style.compare(#key) == 0) scheme = new Class(mpm,args);
#include "style_scheme.h"
#undef SchemeStyle
#undef SCHEME_CLASS

  else {
    error->all(FLERR, "Illegal scheme style.\n");
  }
}

/*! This function is the C++ equivalent to the method() user function.\n
 * Syntax: method(type, type specific arguments)\n
 * It points the pointer Update::method to the desired Method type selected from style_method.h
 */
void Update::create_method(vector<string> args){
  if (args.size() < 3) {
    error->all(FLERR, "Illegal method command: not enough arguments.\n");
  }

  method_type = args[0];
  method_shape_function = args[2];

  if (0) return;

#define METHOD_CLASS
#define MethodStyle(key,Class) \
  else if (method_type.compare(#key) == 0) method = new Class(mpm,args);
#include "style_method.h"
#undef MethodStyle
#undef METHOD_CLASS

  else {
    error->all(FLERR, "Illegal method style.\n");
  }

  method->setup(args);
}

/*! Update elapsed simulation time.
 *  Called at end of runs or when timestep size changes.
 */
void Update::update_time()
{
  atime += (ntimestep-atimestep) * dt;
  atimestep = ntimestep;
  (*input->vars)["time"] = Var("time", atime);
}


/*! Update simulation timestep.
 */
int Update::update_timestep()
{
  update->ntimestep++;
  (*input->vars)["timestep"] = Var("timestep", ntimestep);
  return update->ntimestep;
}
