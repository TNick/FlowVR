#!/usr/bin/perl -w
use strict;
use FlowVR::XML ':all';
&parseInput;

&addConnection('FluidDistRead', 'out', 'VTK', 'in');
&preSignal('VTK', 'endIt','FluidDistRead', 'beginIt', 'Fluid/Presignal', 4);

&printResult;
