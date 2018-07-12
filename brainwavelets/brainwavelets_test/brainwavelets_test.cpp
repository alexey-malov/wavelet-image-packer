// brainwavelets_test.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "../include/brainwavelets.h"

using namespace brainwavelets;

int main(int argc, char* argv[])
{
	CStaticWavelet2D wave;
	float inBuf[8] = {1, 3, 4, 5, 4, 2, 5, 77,};
	float outBuf[8] = {1, 3, 4, 5, 4, 2, 5, 77,};

	wave.Lift(inBuf, 8);
	wave.InverseLift(inBuf, 8);
	return 0;
}

// $Log: brainwavelets_test.cpp,v $
// Revision 1.2  2006/02/08 22:22:56  Vivid
// added log section
//
