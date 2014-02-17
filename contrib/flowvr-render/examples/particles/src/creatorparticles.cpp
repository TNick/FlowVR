#include <iostream>
#include <flowvr/module.h>
#include <ftl/vec.h>

using namespace flowvr;
using namespace ftl;

typedef Vec3f Point3f;

int   nbparticles = 10000;
int   nbalive = 0;
float maxlife = 10;
float curtime = 0;
float lastbirth = 0;
float birthdt = 0.001f;
float gravity = 1.0f;
float minangle = 0;
float maxangle = 360;


inline int nrand( int nMin, int nMax )
{
	return nMin + (int)( (float)( nMax-nMin + 1 ) * ( rand() / (RAND_MAX + 1.0) ) );
}

inline float frand(float d = 1.0f)
{
  return d*rand()/(RAND_MAX+1.0f);
}

struct Particle
{
  Point3f pos;
  Vec3f   vit;
  Point3f color;
  float t0;
  float last_col;

  void init(float _t0)
  {
    pos.x() = frand(0.1); pos.y() = frand(0.1); pos.z() = 0.00;

    int angle = nrand(minangle, maxangle);
    float angle_rad = (float(angle)/180.0f)*3.14159265f;
    vit.x() = ::cos(angle_rad);
    vit.y() = ::sin(angle_rad);
    vit.z() = 0;

    //    vit.x() = frand(2)-1; vit.y() = frand(2)-1; vit.z() = 0;

    vit.normalize();
    vit.z() = 2+frand(2);
    color.x() = 0;
    color.y() = 0.5f+frand(0.5f);
    color.z() = 0.9f+frand(0.1f);
    t0 = _t0;
    last_col = t0;
  }

  /// Return false if the particle is dead
  bool advance(float dt)
  {
    if (curtime >= t0 + maxlife)
			return false;

    // gravity
	vit.z() -= gravity * dt;

	// update
	pos += vit * dt;

	// collision
	if (pos.z() < 0)
	{
		float dv = vit.z();

		pos.z() = 0;
		if (dv < 0)
		{
			vit.z() = -0.8f * vit.z();
			last_col = curtime; //-d;
		}
	}

	// color
	float c = curtime - last_col;

	if (c > 1)
		c = 1;

	color.x() = 1 - c;
	return true;
  }

};

Particle *part = NULL;

void partInit()
{
	part      = new Particle[nbparticles];
	nbalive   = 0;
	curtime   = 0;
	lastbirth = 0;
}

void partStep(float dt = 0.01f)
{
	curtime += dt;
	int i;
	i = 0;
	while (i < nbalive)
	{
		if (part[i].advance(dt))
			++i;
		else
		{
			--nbalive;
			if (nbalive > i)
				part[i] = part[nbalive];
		}
	}
	float nextbirth = lastbirth + birthdt;
	while (nbalive < nbparticles && (curtime >= nextbirth))
	{
		part[nbalive].init(nextbirth);
		lastbirth = nextbirth;
		if (part[nbalive].advance(curtime - lastbirth))
			++nbalive;
		nextbirth = lastbirth + birthdt;
	}
}

void partWrite(Point3f* points, Point3f* colors)
{
  for (int i=0;i<nbalive;i++)
  {
    points[i] = part[i].pos;
    colors[i] = part[i].color;

    if (colors[i].z()<colors[i].x())
    	colors[i].z()=colors[i].x();
    if (colors[i].y()<colors[i].x())
    	colors[i].y()=colors[i].x();
  }
}

int main(int argc, char**argv)
{
	OutputPort portPoints("points");
	BufferPool poolPoints;
	OutputPort portColors("colors");
	BufferPool poolColors;


	std::vector<Port*> ports;
	ports.push_back(&portPoints);
	ports.push_back(&portColors);

	ModuleAPI* module = flowvr::initModule(ports);
	if (!module)
		return 1;

	if( flowvr::Parallel::isParallel() )
	{
		int nbProcs = flowvr::Parallel::getNbProc();
		int nRank   = flowvr::Parallel::getRank();

		float partitionSize = float((360.0f/float(nbProcs)));
		minangle = float(nRank) * partitionSize;
		maxangle = minangle + partitionSize;

		nbparticles = nbparticles / nbProcs;
	}
	else
	{
		std:: cout << "non-parallel creatorparticles." << std::endl;
	}



	partInit();

	int it = 0;

	while (module->wait())
	{
		partStep();

		MessageWrite mpoints, mcolors;
		// Always alloc max size bufs
		mpoints.data = poolPoints.alloc(module->getAllocator(), nbparticles * sizeof(Point3f));
		mcolors.data = poolColors.alloc(module->getAllocator(), nbparticles	* sizeof(Point3f));
		// Then take subbuffers
		mpoints.data = BufferWrite(mpoints.data, 0, nbalive * sizeof(Point3f));
		mcolors.data = BufferWrite(mcolors.data, 0, nbalive * sizeof(Point3f));

		if (nbalive > 0)
			partWrite(mpoints.data.getWrite<Point3f> (0), mcolors.data.getWrite<Point3f> (0));

		module->put(&portPoints, mpoints);
		module->put(&portColors, mcolors);

		++it;
	}

  return 0;
}
