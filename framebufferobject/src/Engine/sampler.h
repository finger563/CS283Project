#ifndef SAMPLER_H
#define SAMPLER_H

#include <math.h>
#include <random>
#include <vector>

using namespace std;

#include "point2d.h"

class sampler
{
public:
	sampler(){num_samples=num_sets=0;}
	sampler(int _numsamp, int _numset){num_samples=_numsamp;num_sets=_numset;}
	~sampler(){}
	
	virtual void generate_samples(void)=0;
	void setup_shuffled_indices(void);
	void shuffle_samples(void);
	POINT2D sample_unit_square(void);

	int num_samples;	// the number of sample points on a pattern
	int num_sets;		// the number of sample sets (patterns) stored
	vector<POINT2D> samples;	// sample points on a unit square
	vector<int> shuffled_indices;	// shuffled samples array indicies
	unsigned long count;	// the current number of sample points used
	int jump;				// random index jump
};


class jittered : public sampler
{
private:
	virtual void generate_samples(void)
	{
		int n = (int)sqrt((double)num_samples);

		for (int p = 0; p < num_sets; p++)
			for (int j = 0; j< n; j++)
				for (int k = 0; k < n; k++)
				{
					POINT2D sp((k + ((float)rand()/(float)RAND_MAX)) / n, (j + ((float)rand()/(float)RAND_MAX))/n );
					samples.push_back(sp);
				}
	}
};

#endif