#!/usr/bin/perl -w
use strict;
use FlowVR::XML ':all';
&parseInput;

&addRoutingNodes('VisuDensity',&getHosts('Visu'));
&addRecursiveGather('Simu','density','VisuDensity','in',$nodeFilterMerge2D,2,'f');
&addRecursiveBroadcast('VisuDensity','out','Visu','density',2,'B3');

&preSignal ('Visu', 'endIt', 'Simu', 'beginIt', 'preSigVisu');

&printResult;
