#include <iostream>
#include "output.h"
#include "dump_particle_gz.h"
#include "update.h"
#include "domain.h"
#include "solid.h"
#include "mpmtype.h"
#include "mpm_math.h"
#include <gzstream.h>

using namespace std;
using namespace MPM_Math;


DumpParticleGz::DumpParticleGz(MPM *mpm, vector<string> args) : Dump(mpm, args)
{
  cout << "In DumpParticleGz::DumpParticleGz()" << endl;
}

DumpParticleGz::~DumpParticleGz()
{
}


void DumpParticleGz::write()
{
  // Open dump file:
  size_t pos_asterisk = filename.find('*');
  string fdump;

  if (pos_asterisk >= 0)
    {
      // Replace the asterisk by ntimestep:
      fdump = filename.substr(0, pos_asterisk)
	+ to_string(update->ntimestep);
      if (filename.size()-pos_asterisk-1 > 0)
	fdump += filename.substr(pos_asterisk+1, filename.size()-pos_asterisk-1);
    }
  else fdump = filename;

  // cout << "Filemame for dump: " << fdump << endl;

  // Open the file fdump:
  ogzstream dumpstream(fdump.c_str());

  dumpstream << "ITEM: TIMESTEP\n0\nITEM: NUMBER OF ATOMS\n";

  bigint total_np = 0;
  for (int isolid=0; isolid < domain->solids.size(); isolid++) total_np += domain->solids[isolid]->np;

  dumpstream << total_np << endl;
  dumpstream << "ITEM: BOX BOUNDS sm sm sm\n";
  dumpstream << domain->boxlo[0] << " " << domain->boxhi[0] << endl;
  dumpstream << domain->boxlo[1] << " " << domain->boxhi[1] << endl;
  dumpstream << domain->boxlo[2] << " " << domain->boxhi[2] << endl;
  dumpstream << "ITEM: ATOMS id type x y z x0 y0 z0 vx vy vz s11 s22 s33 s12 s13 s23 seq damage damage_init volume mass bx by bz ep epdot\n";

  bigint ID = 0;
  for (int isolid=0; isolid < domain->solids.size(); isolid++) {
    Solid *s = domain->solids[isolid];
    for (bigint i=0; i<s->np;i++) {
      ID++;
      dumpstream << ID << " ";
      dumpstream << isolid+1 << " ";
      dumpstream << s->x[i][0] << " ";
      dumpstream << s->x[i][1] << " ";
      dumpstream << s->x[i][2] << " ";
      dumpstream << s->x0[i][0] << " ";
      dumpstream << s->x0[i][1] << " ";
      dumpstream << s->x0[i][2] << " ";
      dumpstream << s->v[i][0] << " ";
      dumpstream << s->v[i][1] << " ";
      dumpstream << s->v[i][2] << " ";
      dumpstream << s->sigma[i](0,0) << " ";
      dumpstream << s->sigma[i](1,1) << " ";
      dumpstream << s->sigma[i](2,2) << " ";
      dumpstream << s->sigma[i](0,1) << " ";
      dumpstream << s->sigma[i](0,2) << " ";
      dumpstream << s->sigma[i](1,2) << " ";
      dumpstream << sqrt(3. / 2.) * Deviator(s->sigma[i]).norm() << " ";
      dumpstream << s->damage[i] << " ";
      dumpstream << s->damage_init[i] << " ";
      dumpstream << s->vol[i] << " ";
      dumpstream << s->mass[i] << " ";
      dumpstream << s->mb[i][0] << " ";
      dumpstream << s->mb[i][1] << " ";
      dumpstream << s->mb[i][2] << " ";
      dumpstream << s->eff_plastic_strain[i] << " ";
      dumpstream << s->eff_plastic_strain_rate[i] << endl;
    }
  }
  dumpstream.close();
}