######################### COPYRIGHT ###############################
#                                                                 #
#                             FlowVR                              #
#                       Configuration Parse                       #
#                                                                 #
#-----------------------------------------------------------------#
# COPYRIGHT (C) 2003/2004/2005 by                                 #
# Laboratoire Informatique et Distribution (UMR5132) and          #
# Laboratoire d'Informatique Fondamentale d'Orleans               #
# (FRE 2490) ALL RIGHTS RESERVED.                                 #
#                                                                 #
# This source is covered by the GNU LGPL, please refer to the     #
# COPYING file for further information.                           #
#                                                                 #
#-----------------------------------------------------------------#
#                                                                 #
#  Original Contributors:                                         #
#    Jeremie Allard,                                              #
#    Ronan Gaugne,                                                #
#    Valerie Gouranton,                                           #
#    Loick Lecointre,                                             #
#    Sebastien Limet,                                             #
#    Bruno Raffin,                                                #
#    Sophie Robert,                                               #
#    Emmanuel Melin.                                              #
#                                                                 # 
###################################################################

package FlowVR::XML;

use strict;
use warnings;
use POSIX 'ceil';

require Exporter;
#use AutoLoader qw(AUTOLOAD);

our @ISA = qw(Exporter);

# Items to export into callers namespace by default. Note: do not export
# names by default without a very good reason. Use EXPORT_OK instead.
# Do not simply export all your public functions/methods/constants.

# This allows declaration	use FlowVR::XML ':all';
# If you do not need this, moving things directly into @EXPORT or @EXPORT_OK
# will save memory.
our %EXPORT_TAGS = ( 'all' => [ qw(
parseInput
parseDoc
showHosts
showIds
printResult
printDoc
$doc
$root
$scriptLevel
%xmlnode
%hosts
getNb
getHosts
getHost
getHostNetwork
%subids
getSubIds
addSimpleConnection
addConnection
addSimpleConnectionStamps
addConnectionStamps
delRoutingRule
addRoutingRule
addRoutingRuleFull
addRoutingRuleStamps
showRules
getNetwork
$nodeModule
$nodeRoutingNode
$nodeRoutingNodeStamps
$nodeGreedySynchronizer
$nodeGreedySynchronizerMin1
$nodeGreedySynchronizerMax1
$nodeFilterIt
$nodeMergeIt
$nodeFilterMerge2D
nodeFilterMerge
nodeFilterScatter
nodeFilterRotateFilter
nodeFilterUnrotateFilter
$nodeFilterMaxBuffer
$nodeFilterMinIt
$nodeFilterIt2Start
$nodeFilterDiscard
$nodeFilterStop
nodeFilterFrequency
nodeMaxFrequencySynchronizor
nodeLogger
nodeFilterMultiplex
addObject
addObjects
addPortFull
addPortStamps
createElement
createTextNode
createTree
createNodeFilter
createNodeSynchronizer
addNodeParameters
addRecursiveGather
addRecursiveScatter
addRecursiveBroadcast
addP2PMergeAndBroadcast
addRoutingNodes
addRoutingNodeStamps
addGreedy
addCustomGreedy
addOutGreedy
addOutCustomGreedy
addTracetoModule				   
addTracetoFilterSync				   
removeTrace
addLoggersWriters
trace
moduleFrequency
moduleMaxFrequency
preSignal
preSignalStamps
preSignalGreedy
nodeFilterSignalAnd
andSignal
barrier
getOutputPorts
getInputPorts
hasOutputPort
hasInputPort
findObjectsWithInputPort
findObjectsWithOutputPort
fixSources
sendCommand
sendCommands
sendCommandsAddObject
sendCommandsAddRoute
sendCommandsStart
@style
) ] );

our @EXPORT_OK = ( @{ $EXPORT_TAGS{'all'} } );

our @EXPORT = qw(
	
);

our $VERSION = '0.01';


# Preloaded methods go here.

use XML::DOM;

our $parser;
our $doc;
our $root;
our $routingDoc;
our $scriptLevel = 0;

our @rules;     #list of routing rules : ('module','port','network','priority','msgtype')
our %xmlnode;    # hash of all xml nodes given their id
our %hosts;      # hash of array of hosts of all objects or sub-objects given an id
our %subids;     # hash of all sub-objects' id given an id
our %connections; # hash of connections given a destination id (object+port)
our %connectionstamps; # hash of connectionstamps given a destination id (object+port)
our $ccount = 0; # connections counter
our $rcount = 0; # routing nodes counter

sub getNb
{
    my ($id) = @_;
    return 0 if (!$subids{$id});
    return scalar(@{$subids{$id}});
}

sub getSubIds
{
    my ($id) = @_;
    return $subids{$id};
}

sub getHosts
{
    my ($id) = @_;
#    return @{$hosts{$id}}
    return $hosts{$id};
}

sub getHost
{
    my ($id) = @_;
    return $hosts{$id}->[0];
}

sub pushNewNode
  {
    my ($id, $host, $e) = @_;
    print STDERR 'New object: '.$id.' @ '.$host."\n" if (defined $ENV{'DEBUG'});
    $xmlnode{$id} = $e;
    push @{$hosts{$id}}, $host;
    push @{$subids{$id}}, $id;
    my $sid = $id;
    while ($sid =~ /^(.*)\/.*$/)
      {
	$sid = $1;
	push @{$hosts{$sid}},$host;
	push @{$subids{$sid}},$id;
      }
}

sub parseDoc
{
    no strict 'subs';             # Allow bare word 'STDIN'

    $parser = XML::DOM::Parser->new();
    #$doc = $parser->parse(STDIN);
    my $myxml = "";
    while (<STDIN>)
    {
	$myxml .= $_;
    }
    $doc = $parser->parsestring($myxml);
    $root = $doc->getDocumentElement();
    #$root->setNodeName("instanciationgraph");
    #my $doctype = $doc->getDoctype();
    #$doc->removeChild($doctype) if (defined $doctype);

    foreach my $e ($doc->getElementsByTagName('module'),
		   $doc->getElementsByTagName('filter'),
		   $doc->getElementsByTagName('synchronizer'),
		   $doc->getElementsByTagName('routingnode'),
		   $doc->getElementsByTagName('routingnodestamps')
		   )
      {
      my $id = $e->getAttribute('id');
      my $host = $e->getAttribute('host');
      #print STDERR $id . '@' . $host . "\n";
      &pushNewNode($id,$host,$e);
    }

    foreach my $c ($doc->getElementsByTagName('connection'))
    {
	my $connid = $c->getAttribute('id');
	if ($connid =~ /^_c([0-9]+)$/)
	{
	    my $count = $1;
	    if ($count >= $ccount)
	    {
		$ccount = $count+1;
	    }
	}
	my $destportid;
	my $dest = $c->getElementsByTagName('destination')->item(0)->getFirstChild();

	$destportid = $dest->getAttribute('id');
	$destportid .= ':'.$dest->getAttribute('port') if ($dest->getNodeName() !~ /^routingnode/);
	$connections{$destportid} = $c;
    }

    foreach my $c ($doc->getElementsByTagName('connectionstamps'))
    {
	my $connid = $c->getAttribute('id');
	if ($connid =~ /^_c([0-9]+)$/)
	{
	    my $count = $1;
	    if ($count >= $ccount)
	    {
		$ccount = $count+1;
	    }
	}
	my $destportid;
	my $dest = $c->getElementsByTagName('destinationstamps')->item(0)->getFirstChild();

	$destportid = $dest->getAttribute('id');
	$destportid .= ':'.$dest->getAttribute('port') if ($dest->getNodeName() !~ /^routingnode/);
	$connectionstamps{$destportid} = $c;
    }

    foreach my $r ($doc->getElementsByTagName('routingnode'))
    {
	my $rid = $r->getAttribute('id');
	if ($rid =~ /^_r([0-9]+)$/)
	{
	    my $count = $1;
	    if ($count >= $rcount)
	    {
		$rcount = $count+1;
	    }
	}
    }

    foreach my $r ($doc->getElementsByTagName('routingnodestamps'))
    {
	my $rid = $r->getAttribute('id');
	if ($rid =~ /^_r([0-9]+)$/)
	{
	    my $count = $1;
	    if ($count >= $rcount)
	    {
		$rcount = $count+1;
	    }
	}
    }


    #parse the routing file
    $routingDoc = $parser->parsefile($ENV{'FLOWVR_ROUTING'}) if (defined $ENV{'FLOWVR_ROUTING'});

	
	
}

sub showHosts
{
    foreach my $id (sort keys %hosts)
    {
	my @h = @{$hosts{$id}};
	print STDERR $id.' @ '.join(' ',@h)."\n";
    }
}

sub showIds
{
    foreach my $id (sort keys %hosts)
    {
	my @ids = @{$subids{$id}};
	print STDERR $id.' : '.join(' ',@ids)."\n";
    }
}

sub printDoc
{
    print $doc->toString if ($scriptLevel==1);
    --$scriptLevel;
}

sub parseInput
{
    ++$scriptLevel;
    return if ($scriptLevel>1);

    print STDERR "Parsing document...\n" if (defined $ENV{'DEBUG'});
    &parseDoc;
    if (defined $ENV{'DEBUG'})
    {
	print STDERR "Hosts:\n";
	&showHosts;
	print STDERR "Subids:\n";
	&showIds;
	print STDERR "Adding connections...\n";
    }
    createElements();
}

sub xmlNodeRef
{
    my ($id, $port) = @_;
    my $node = $xmlnode{$id};
    my $type = $node->getNodeName();
    my $newnode = $doc->createElement($type.'id');
    $newnode->setAttribute('id',$id);
    $newnode->setAttribute('port',$port) if ($type !~ /^routingnode/);
    return $newnode;
}

sub xmlNodeRefID
{
    my ($id, $port) = @_;
    my $node = $xmlnode{$id};
    my $type = $node->getNodeName();
    my $newnode = $doc->createElement($type.'id');
    $newnode->setAttribute('id',$id);
    $newnode->setAttribute('port',$port) if ($type !~ /^routingnode/);
    $_[2] = $id;
    $_[2] .= ':'.$port if ($type !~ /^routingnode/);
    return $newnode;
}

our @style = ();

sub addSimpleConnectionBase
{ # add a simple point2point connection
    my ($sourceid, $sourceport, $destid, $destport, @st) = @_;
    print STDERR "SimpleConnection $sourceid:$sourceport -> $destid:$destport\n" if (defined $ENV{'DEBUG'});
    # check valid source and dest
    die  "ERROR: Invalid source in connection $sourceid:$sourceport -> $destid:$destport\n" if (! $subids{$sourceid} );
    die  "ERROR: Invalid destination in connection $sourceid:$sourceport -> $destid:$destport\n" if (! $subids{$destid} );

    my $newnode = $doc->createElement('connection');
    my $id = '_c'.$ccount;
    my $destportid;
	$newnode->setAttribute('id',$id);
        $newnode->setAttribute('style',join(',',@style,@st)) if (@style || @st);
      my $source = $doc->createElement('source');
      $newnode->appendChild($source);
        $source->appendChild(&xmlNodeRef($sourceid,$sourceport));
      my $dest = $doc->createElement('destination');
      $newnode->appendChild($dest);
        $dest->appendChild(&xmlNodeRefID($destid,$destport,$destportid));
    ++$ccount;
    $root->appendChild($newnode);    $root->appendChild($doc->createTextNode("\n"));
    $connections{$destportid} = $newnode;
    return $id;
}



our $nodeModule;
our $nodeFilter;
our $nodeSynchronizer;
our $nodeRoutingNode;
our $nodeRoutingNodeStamps;

our $nodeGreedySynchronizer;
our $nodeGreedySynchronizerMin1;
our $nodeGreedySynchronizerMax1;
our $nodeFilterIt;
our $nodeMergeIt;
our $nodeFilterMerge2D;
our $nodeFilterMaxBuffer;
our $nodeFilterMinIt;
our $nodeFilterIt2Start;
our $nodeFilterDiscard;
our $nodeFilterStop;

sub addSimpleRoutingConnection
{
    my ($sourceid, $sourceport, $destid, $destport, $network, @st) = @_;
    my $destHostDefault = &getHost($destid);
    my $destHostNew = &getNameHostOnNetwork($destHostDefault,$network);
    my $sourceHost=&getHost($sourceid);
    
    if (defined($destHostNew) && !isSameHost($sourceHost,$destHostNew) && (&getNetwork($destHostDefault) ne $network))
      {
	if(defined $ENV{'DEBUG'})
	  {
	    print STDERR " Routing : add a routing node\n";
	  }

	my $id = '_r'.$rcount;
	&addObject("$id",$destHostNew,$nodeRoutingNode); #Add the routing node
	++$rcount;

	&addSimpleConnectionBase($sourceid,$sourceport,"$id",'in',@st);
	&addSimpleConnectionBase("$id",'out',$destid, $destport,@st);

      }
    else
      {
	if(defined $ENV{'DEBUG'})
	  {
	    print STDERR " Routing : BUT don't add routing node because ";
	    if(!defined($destHostNew))
	      {print STDERR "$destHostDefault don't found on $network (see your routing.xml)\n";}
	    elsif(isSameHost($sourceHost,$destHostNew))
	      {print STDERR "$sourceHost is the same host that $destHostNew\n";}
	    else{print STDERR "$destid already on $network\n";}
	  }
	&addSimpleConnectionBase($sourceid, $sourceport, $destid, $destport,@st);
      }

}

sub addRoutingRule
{
  my ($sourceid,$sourceport,$destid, $destport, $network) = @_;
  unshift(@rules,[$sourceid,$sourceport,$destid , $destport, $network, '']);
}

sub addRoutingRuleStamps
{
  my ($sourceid,$sourceport,$destid, $destport, $network) = @_;
  unshift(@rules,[$sourceid,$sourceport,$destid , $destport, $network, 'stamps']);
}

sub addRoutingRuleFull
{
  my ($sourceid,$sourceport,$destid, $destport, $network) = @_;
  unshift(@rules,[$sourceid,$sourceport,$destid , $destport, $network, 'stamps']);
}

sub delRoutingRule
  {
    shift(@rules);
  }

sub showRules
  {
    my $i =1;
    foreach my $tmp  (@rules)
      {
	my ($r_srcid, $r_srcport, $r_destid, $r_destport,$r_network,$r_msgtype)=@$tmp;
	print STDERR "Rules $i : $r_srcid | $r_srcport |$r_destid |$r_destport |$r_network |$r_msgtype\n";
	$i++;
      }
  }

#return the name of $id on $network or undef
sub getNameHostOnNetwork
  {
    my ($id,$network) = @_;

    if (defined($routingDoc))
      {
	#Search the Host with the given $id 
	foreach my $node ($routingDoc->getElementsByTagName( 'id' ))
	  {
	    
	    my $data = $node->getFirstChild->getData;
	    
	    #host found
	    if ($data eq "$id")
	      {
		my $theHost = $node->getParentNode;
		while ($theHost->getTagName ne "host")
		  {
		    $theHost = $theHost->getParentNode;
		  }

		#return the (first) name on $network
		foreach my $ni  ($theHost->getElementsByTagName( 'ni' ))
		  {
		    return $ni->getElementsByTagName( 'id' )->item(0)->getFirstChild->getData if($ni->getAttribute('netid') eq $network) ;
		  }
	      }
	  }

      }
    return undef;
  }



#return true if $host1 equal $host2
sub isSameHost
  {
    my ($host1,$host2) = @_;
    my ($dadHost1,$dadHost2);
    return ($host1 eq $host2) if (!defined $routingDoc);
    foreach my $node ($routingDoc->getElementsByTagName( 'id' ))
      {
	my $data = $node->getFirstChild->getData;
	if ($data eq $host1)
	  {
	    $dadHost1 = $node->getParentNode;
	    while ($dadHost1->getTagName ne "host")
	      {
		$dadHost1 = $dadHost1->getParentNode;
	      }
	  }
	if($data eq $host2)
	  {
	    $dadHost2 = $node->getParentNode;
	    while ($dadHost2->getTagName ne "host")
	      {
		$dadHost2 = $dadHost2->getParentNode;
	      }
	  }
      }
    return  if(!defined($dadHost1) || !defined($dadHost2)); 
    return($dadHost1 eq $dadHost2);
  }

#return the name of network $host
sub getNetwork()
  {
    my ($host) = @_;
    foreach my $node ($routingDoc->getElementsByTagName( 'id' ))
      {
	my $data = $node->getFirstChild->getData;
	if ($data eq $host)
	  {
	    if($node->getParentNode->getNodeName eq 'ni')
	      {
		return $node->getParentNode->getAttribute('netid');
	      }
	  }
      }

  }


sub isSameNetwork
  {
    my ($host1,$host2) = @_;
    return (&getNetwork($host1) eq &getNetwork($host2));
  }

#return the $network if a rules is find
sub getNetworkRoutingRuleFull
  {
    my ($sourceid, $sourceport, $destid, $destport) = @_;
    foreach my $tmp  (@rules)
      {
	my ($r_srcid, $r_srcport, $r_destid, $r_destport,$r_network,$r_msgtype)=@$tmp;
	if ($sourceid =~/^$r_srcid/ && $sourceport =~ /^$r_srcport/ && $destid =~ /^$r_destid/ && $destport =~ /^$r_destport/ && $r_msgtype ne 'stamps')
	  {
	    print STDERR "Routing : Rule ($r_srcid, $r_srcport, $r_destid, $r_destport,$r_network,$r_msgtype) found between $sourceid and $destid \n"  if(defined $ENV{'DEBUG'});
	    return($r_network);
	  }
     }
    return undef;
  }


#return the $network if a rules is find
sub getNetworkRoutingRuleStamps
  {
    my ($sourceid, $sourceport, $destid, $destport) = @_;
    foreach my $tmp  (@rules)
      {
	my ($r_srcid, $r_srcport, $r_destid, $r_destport,$r_network,$r_msgtype)=@$tmp;
	if ($sourceid =~/^$r_srcid/ && $sourceport =~ /^$r_srcport/ && $destid =~ /^$r_destid/ && $destport =~ /^$r_destport/ && $r_msgtype ne 'full')
	  {
	    print STDERR "Routing : Rule ($r_srcid, $r_srcport, $r_destid, $r_destport,$r_network,$r_msgtype) found between $sourceid and $destid \n"  if(defined $ENV{'DEBUG'});
	    return($r_network);
	  }
     }
    return undef;
  }



sub getHostNetwork
{
  my ($id,$network) = @_;
  my $host = getHost($id);
  if (defined $network)
  {
      my $newhost = &getNameHostOnNetwork($host,$network);
      $host = $newhost if (defined $newhost);
  }
  return $host;
}




sub addSimpleConnection
{ # add a simple point2point connection
  my ($sourceid, $sourceport, $destid, $destport, @st) = @_;

  my $net = getNetworkRoutingRuleFull($sourceid, $sourceport, $destid, $destport);
  if (defined($net))
    {
      addSimpleRoutingConnection($sourceid, $sourceport, $destid, $destport, $net, @st);
    }
    else
    {
      addSimpleConnectionBase($sourceid, $sourceport, $destid, $destport, @st);
    }
}



sub addConnection
{
    my ($sourceid, $sourceport, $destid, $destport, @st) = @_;
    print STDERR "Connection $sourceid:$sourceport -> $destid:$destport\n" if (defined $ENV{'DEBUG'});
    # check valid source and dest
    die  "ERROR: Invalid source in connection $sourceid:$sourceport -> $destid:$destport\n" if (! $subids{$sourceid} );
    die  "ERROR: Invalid destination in connection $sourceid:$sourceport -> $destid:$destport\n" if (! $subids{$destid} );
    # default connection: assume replication, find the nearest source for each destination
    my @sourceids = @{$subids{$sourceid}};
    my $nextsource = 0;
    LOOP1: foreach my $did ( @{$subids{$destid}} )
    {
	my $sid;
	if (&getHost($sourceids[$nextsource]) ne &getHost($did))
	{
	    foreach $sid ( @sourceids )
	    {
		if (&getHost($sid) eq &getHost($did))
		{
		    &addSimpleConnection($sid,$sourceport,$did,$destport,@st);
		    next LOOP1;
		}
	    }
	}
	$sid = $sourceids[$nextsource];
	&addSimpleConnection($sid,$sourceport,$did,$destport,@st);
	++$nextsource;
	$nextsource = 0 if ($nextsource == scalar(@sourceids));
    }
}

sub addSimpleConnectionStampsBase
{ # add a simple point2point connectionstamps
    my ($sourceid, $sourceport, $destid, $destport, @st) = @_;
    print STDERR "SimpleConnectionStamps $sourceid:$sourceport -> $destid:$destport\n" if (defined $ENV{'DEBUG'});
    my $newnode = $doc->createElement('connectionstamps');
    my $id = '_c'.$ccount;
    my $destportid;
	$newnode->setAttribute('id',$id);
        $newnode->setAttribute('style',join(',',@style,@st)) if (@style || @st);
      my $source = $doc->createElement('sourcestamps');
      $newnode->appendChild($source);
        $source->appendChild(&xmlNodeRef($sourceid,$sourceport));
      my $dest = $doc->createElement('destinationstamps');
      $newnode->appendChild($dest);
        $dest->appendChild(&xmlNodeRefID($destid,$destport,$destportid));
    ++$ccount;
    $root->appendChild($newnode);    $root->appendChild($doc->createTextNode("\n"));
    $connectionstamps{$destportid} = $newnode;
    return $id;
}

sub addSimpleRoutingConnectionStamps
  {
    my ($sourceid, $sourceport, $destid, $destport, $network, @st) = @_;
    my $destHostDefault = &getHost($destid);
    my $destHostNew = &getNameHostOnNetwork($destHostDefault,$network);
    my $sourceHost=&getHost($sourceid);
    
    if (defined($destHostNew) && !isSameHost($sourceHost,$destHostNew) && (&getNetwork($destHostDefault) ne $network))
      {
	if(defined $ENV{'DEBUG'})
	  {
	    print STDERR " Routing : add a routing node (stamps)\n";
	  }

	my $id = '_r'.$rcount;
	&addObject("$id",$destHostNew,$nodeRoutingNode); #Add the routing node
	++$rcount;

	&addSimpleConnectionStampsBase($sourceid,$sourceport,"$id",'in',@st);
	&addSimpleConnectionStampsBase("$id",'out',$destid, $destport,@st);

      }
    else
      {
	if(defined $ENV{'DEBUG'})
	  {
	    print STDERR " Routing : BUT don't add routing node stamps because ";
	    if(!defined($destHostNew))
	      {print STDERR "$destHostDefault don't found on $network (see your routing.xml)\n";}
	    elsif(isSameHost($sourceHost,$destHostNew))
	      {print STDERR "$sourceHost is the same host that $destHostNew\n";}
	    else{print STDERR "$destid already on $network\n";}
	  }
	&addSimpleConnectionStampsBase($sourceid, $sourceport, $destid, $destport,@st);
      }


  }

sub addSimpleConnectionStamps
  {# add a simple point2point connection
  my ($sourceid, $sourceport, $destid, $destport, @st) = @_;

  my $net = getNetworkRoutingRuleStamps($sourceid, $sourceport, $destid, $destport);
  if (defined($net))
    {
       
      &addSimpleRoutingConnectionStamps($sourceid, $sourceport, $destid, $destport, $net, @st);
    }
    else
    {
     &addSimpleConnectionStampsBase($sourceid, $sourceport, $destid, $destport, @st);
    }


  }
sub addConnectionStamps
{
    my ($sourceid, $sourceport, $destid, $destport, @st) = @_;
    print STDERR "ConnectionStamps $sourceid:$sourceport -> $destid:$destport\n" if (defined $ENV{'DEBUG'});
    # check valid source and dest
    die  "ERROR: Invalid source in connection $sourceid:$sourceport -> $destid:$destport\n" if (! $subids{$sourceid} );
    die  "ERROR: Invalid destination in connection $sourceid:$sourceport -> $destid:$destport\n" if (! $subids{$destid} );
    # default connection: assume replication, find the nearest source for each destination
    my @sourceids = @{$subids{$sourceid}};
    my $nextsource = 0;
    LOOP1: foreach my $did ( @{$subids{$destid}} )
    {
	my $sid;
	if (&getHost($sourceids[$nextsource]) ne &getHost($did))
	{
	    foreach $sid ( @sourceids )
	    {
		if (&getHost($sid) eq &getHost($did))
		{
		    &addSimpleConnectionStamps($sid,$sourceport,$did,$destport,@st);
		    next LOOP1;
		}
	    }
	}
	$sid = $sourceids[$nextsource];
	&addSimpleConnectionStamps($sid,$sourceport,$did,$destport,@st);
	++$nextsource;
	$nextsource = 0 if ($nextsource == scalar(@sourceids));
    }
}

sub addObject
{
    my ($id, $host, $template) = @_;
    my $newnode = $template->cloneNode(1);
    $newnode->setAttribute('id',$id);
    $newnode->setAttribute('host',$host);
    $newnode->setAttribute('style',join(',',@style)) if (@style);
    $root->appendChild($newnode);    $root->appendChild($doc->createTextNode("\n"));
    pushNewNode($id, $host, $newnode);
}

sub addObjects
{
    my ($id, $hosts, $template) = @_;
    my $count = 0;
    foreach my $host (@{$hosts})
    {
	&addObject($id.'/'.$count,$host,$template);
	++$count;
    }
}



sub createElement
{
    my ($name, $attr) = @_;
    my $node = $doc->createElement($name);
    if (defined $attr)
    {
	my %attrmap = %{$attr};
	foreach my $a (keys %attrmap)
	{
	    $node->setAttribute($a,$attrmap{$a});
	}
    }
    return $node;
}

sub createTextNode
{
    return $doc->createTextNode(join(' ',@_));
}

sub createTree
{
    my ($root, @children) = @_;
    foreach my $p (@children)
    {
	$root->appendChild($p);
    }
    return $root;
}

sub createNodeFilter
{
    my ($class, @params) = @_;
    my $newnode = $nodeFilter->cloneNode(1);
        my $classnode = $doc->createElement('filterclass');
        $newnode->appendChild($classnode);
            $classnode->appendChild($doc->createTextNode($class));
    foreach my $p (@params)
    {
	$newnode->appendChild($p);
    }
    return $newnode;
}

sub createNodeSynchronizer
{
    my ($class, @params) = @_;
    my $newnode = $nodeSynchronizer->cloneNode(1);
        my $classnode = $doc->createElement('synchronizerclass');
        $newnode->appendChild($classnode);
            $classnode->appendChild($doc->createTextNode($class));
    foreach my $p (@params)
    {
	$newnode->appendChild($p);
    }
    return $newnode;
}


sub addNodeParameters
{
    my ($node, @params) = @_;
    my $newnode = $node->cloneNode(1);
    my $prevparam = $newnode->getElementsByTagName('parameters');
    my $parameters;
    if ($prevparam->getLength>0)
    {
	$parameters = $prevparam->item(0);
    }
    else
    {
	$parameters = createElement('parameters');
    }
    foreach my $p (@params)
    {
	$parameters->appendChild($p);
    }
    if ($prevparam->getLength==0)
    {
	$newnode->appendChild($parameters);
    }
    return $newnode;
}


# add a data port to list of existing ones
sub addPortFull
{
    my ($node,$portype,$id) = @_;
    my @portlist = $node->getElementsByTagName($portype);
    my $port;
    if ($#portlist < 0)
      {
	$port = createElement($portype);
	$node->appendChild($port);
      }
    else
      {
	$port = $portlist[0];
      }
    my $newport=createTree(createElement('port',{'id' => $id}),createElement('datatype'));
    $port->appendChild($newport);
    return $node;
}


# add a stamps port to list of existing ones 
sub addPortStamps
{
    my ($node,$portype,$id) = @_;
    my @portlist = $node->getElementsByTagName($portype);
    my $port;
    if ($#portlist < 0){
      $port = createElement($portype);
      $node->appendChild($port);
    }
    else
      {
	$port = $portlist[0];
      }
    my $newport=createElement('port',{'id' => $id});
    $port->appendChild($newport);

    return $node;
}


sub createElements
{
$nodeModule = $doc->createElement('module');
$nodeFilter = $doc->createElement('filter');
$nodeSynchronizer = $doc->createElement('synchronizer');
$nodeRoutingNode = $doc->createElement('routingnode');
$nodeRoutingNodeStamps = $doc->createElement('routingnodestamps');

$nodeGreedySynchronizer = &createNodeSynchronizer('flowvr.plugins.GreedySynchronizor');
$nodeGreedySynchronizer = &addPortStamps($nodeGreedySynchronizer,'input','stamps');
$nodeGreedySynchronizer = &addPortStamps($nodeGreedySynchronizer,'input','endIt');
$nodeGreedySynchronizer = &addPortStamps($nodeGreedySynchronizer,'output','order');


$nodeGreedySynchronizerMin1 =
    &addNodeParameters($nodeGreedySynchronizer, createElement('diff',{'min' => '1'}));

$nodeGreedySynchronizerMax1 =
    &addNodeParameters($nodeGreedySynchronizer, createElement('diff',{'max' => '1'}));

$nodeFilterIt = &createNodeFilter('flowvr.plugins.FilterIt');
$nodeFilterIt = &addPortFull($nodeFilterIt,'input','in');
$nodeFilterIt = &addPortStamps($nodeFilterIt,'input','order');
$nodeFilterIt = &addPortFull($nodeFilterIt,'output','out');

$nodeMergeIt = &createNodeFilter('flowvr.plugins.MergeIt');
$nodeMergeIt = &addPortFull($nodeMergeIt,'input','in');
$nodeMergeIt = &addPortStamps($nodeMergeIt,'input','order');
$nodeMergeIt = &addPortFull($nodeMergeIt,'output','out');

$nodeFilterMerge2D = &createNodeFilter('flowvr.plugins.Merge2D');
$nodeFilterMerge2D = &addPortFull($nodeFilterMerge2D,'input','in0');
$nodeFilterMerge2D = &addPortFull($nodeFilterMerge2D,'input','in1');
$nodeFilterMerge2D = &addPortFull($nodeFilterMerge2D,'output','out');

$nodeFilterMaxBuffer =&createNodeFilter('flowvr.plugins.MaxBuffer');
$nodeFilterMaxBuffer =&addPortStamps($nodeFilterMaxBuffer,'input','portIt');
$nodeFilterMaxBuffer =&addPortStamps($nodeFilterMaxBuffer,'input','signal');
$nodeFilterMaxBuffer =&addPortStamps($nodeFilterMaxBuffer,'output','out');

$nodeFilterMinIt = &createNodeFilter('flowvr.plugins.MinIt');
$nodeFilterMinIt = &addPortStamps($nodeFilterMinIt,'input','in0');
$nodeFilterMinIt = &addPortStamps($nodeFilterMinIt,'input','in1');
$nodeFilterMinIt = &addPortStamps($nodeFilterMinIt,'output','out');

$nodeFilterIt2Start = &createNodeFilter('flowvr.plugins.It2Start');
$nodeFilterIt2Start = &addPortStamps($nodeFilterIt2Start,'input','it');
$nodeFilterIt2Start = &addPortStamps($nodeFilterIt2Start,'output','out');

$nodeFilterDiscard =  &createNodeFilter('flowvr.plugins.Discard');
$nodeFilterDiscard =  &addPortFull($nodeFilterDiscard,'input','in'); 
$nodeFilterDiscard =  &addPortFull($nodeFilterDiscard,'input','open'); 
$nodeFilterDiscard =  &addPortFull($nodeFilterDiscard,'output','out'); 

$nodeFilterStop =    &createNodeFilter('flowvr.plugins.Stop');
$nodeFilterStop = &addPortFull($nodeFilterStop,'input','in');
$nodeFilterStop = &addPortFull($nodeFilterStop,'input','open');
$nodeFilterStop = &addPortFull($nodeFilterStop,'output','out');
}

sub nodeFilterMerge
{
  my ($nbports) = @_;
  my $node =&createNodeFilter('flowvr.plugins.Merge',createTree(createElement('parameters'),createTree(createElement('nb'),createTextNode($nbports))));
  for (my $i = 0; $i < $nbports; $i++)
  {
    $node = &addPortFull($node,'input','in'.$i);
  }
  $node = &addPortFull($node,'output','out');
  return $node;
}


sub nodeFilterScatter
{
  my ($nbports, $elementsize) = @_;

  my $node = &createNodeFilter('flowvr.plugins.Scatter',createTree(createElement('parameters'),
								   createTree(createElement('nb'),createTextNode($nbports)),
								   createTree(createElement('elementsize'),
									      createTextNode($elementsize))));
  for (my $i = 0; $i < $nbports; $i++) {
    $node = &addPortFull($node,'output','out'.$i);
  }
  $node = &addPortFull($node,'input','in');
  return $node
}


sub nodeFilterRotateFilter
{
  my ($nbports) = @_;

  my $node = &createNodeFilter('flowvr.plugins.RotateFilter',createTree(createElement('parameters'),createTree(createElement('nb'),createTextNode($nbports))));

  for (my $i = 0; $i < $nbports; $i++) {
    $node = &addPortFull($node,'output','out'.$i);
  }
  $node = &addPortFull($node,'input','in');
  return $node
}


sub nodeFilterUnrotateFilter
{
  my ($nbports) = @_;

  my $node = &createNodeFilter('flowvr.plugins.UnrotateFilter',createTree(createElement('parameters'),createTree(createElement('nb'),createTextNode($nbports))));

  for (my $i = 0; $i < $nbports; $i++) {
    $node = &addPortFull($node,'input','in'.$i);
  }
  $node = &addPortFull($node,'output','out');
  return $node
}


sub nodeFilterFrequency
{
  my ($freq) = @_;
  my $node =  createNodeFilter('flowvr.plugins.FrequencyFilter',
			       createTree(createElement('parameters'),
					  createTree(createElement('freq'),
						     createTextNode($freq))));
  $node = &addPortFull($node,'output','out');
  return  $node;
}


sub nodeMaxFrequencySynchronizor
{
  my ($freq) = @_;
  my $node = createNodeSynchronizer('flowvr.plugins.MaxFrequencySynchronizor',
				    createTree(createElement('parameters'),
					       createTree(createElement('freq'),
							  createTextNode($freq))));

  $node = &addPortStamps($node,'input','endIt');
  $node = &addPortStamps($node,'output','out');
  return $node;
}


sub nodeLogger
{

  my ($buffernb,$buffersize,$period) = @_;

  my $node =  createNodeFilter('flowvr.plugins.DefaultLogger',
			  createTree(createElement('parameters'),
				     createTree(createElement('buffer',{'nb' => $buffernb,'size' => $buffersize})),
				     createTree(createElement('period'),createTextNode($period))));

  $node = &addPortFull($node,'output','log');
}


sub nodeFilterMultiplex
{
  my ($nbports) = @_;
  my $node =&createNodeFilter('flowvr.plugins.MultiplexFilter',createTree(createElement('parameters'),createTree(createElement('nb'),createTextNode($nbports))));
  for (my $i = 0; $i < $nbports; $i++)
  {
    $node = &addPortFull($node,'input','in'.$i);
  }
  $node = &addPortFull($node,'output','out');
  return $node;
}


sub addRecursiveGather
{
    my ($sourceid, $sourceport, $destid, $destport, $node, $arity, $id) = @_;
    print STDERR "RecursiveGather $sourceid:$sourceport -> $destid:$destport\n" if (defined $ENV{'DEBUG'});
    my $net = getNetworkRoutingRuleFull($sourceid, $sourceport, $destid, $destport);
    my $host = &getHostNetwork($destid,$net);



    my @sourceids = @{$subids{$sourceid}};
    my $count = 1;
    while (scalar(@sourceids) > 1)
    {
	my @newids = ();
	do
	{
	    my $n;
	    #$count = 0 if (scalar(@sourceids)<=$arity && scalar(@newids)==0);
	    my $newid = $id.'/'.$count;
	    ++$count;
	    my $newhost =&getHostNetwork($sourceids[0],$net);


	    if (scalar(@sourceids)<=$arity && scalar(@newids)==0)
	    {
		$newhost=$host;
	    }
	    else
	    {
		for($n=0; $n<$arity && $n<scalar(@sourceids) ; $n++)
		{
		    my $h = &getHostNetwork($sourceids[$n],$net);
		    $newhost = $h if ($h eq $host); # place output to final host if possible
		}
	    }
	    if (scalar(@sourceids)>1)
	    {
		&addObject($newid,$newhost,$node);
	    }
	    else
	    {
		&addObject($newid,$newhost,$nodeRoutingNode);
	    }

	    for($n=0; $n<$arity && scalar(@sourceids)>0 ; $n++)
	    {
		my $sid = shift @sourceids;
	
		&addSimpleConnection($sid,$sourceport,$newid,'in'.$n);
		  
	    }
	    push @newids, $newid;
	}
	while (scalar(@sourceids) > 0);
	@sourceids = @newids;
	$sourceport = 'out';
    }
    &addConnection($sourceids[0],$sourceport,$destid,$destport);
    return $sourceids[0];
}




sub addRecursiveScatter
{
    my ($sourceid, $sourceport, $destid, $destport, $node, $arity, $id) = @_;
    print STDERR "RecursiveScatter $sourceid:$sourceport -> $destid:$destport\n" if (defined $ENV{'DEBUG'});
    my $net = &getNetworkRoutingRuleFull($sourceid, $sourceport, $destid, $destport);
    my $host = &getHostNetwork($sourceid,$net);

    my @destids = @{$subids{$destid}};
    my $count = 1;
    while (scalar(@destids) > 1)
    {
	my @newids = ();
	do
	{
	  my $n;
	  #$count = 0 if (scalar(@destids)<=$arity && scalar(@newids)==0);
	  my $newid = $id.'/'.$count;
	  ++$count;
	  my $newhost = &getHostNetwork($destids[0],$net);
	  if (scalar(@destids)<=$arity && scalar(@newids)==0)
	    {
	      $newhost=$host;
	    }
	    else
	      {
		for($n=0; $n<$arity && $n<scalar(@destids) ; $n++)
		  {
		    my $h =&getHostNetwork($destids[$n],$net);
		    $newhost = $h if ($h eq $host); # place output to final host if possible
		  }
	      }
	  if (scalar(@destids)>1)
	    {
	      &addObject($newid,$newhost,$node);
	    }
	  else
	    {
	      &addObject($newid,$newhost,$nodeRoutingNode);
	    }
	  
	  for($n=0; $n<$arity && scalar(@destids)>0 ; $n++)
	    {
	      my $did = shift @destids;
	      if (defined($net))
		{
		  &addSimpleRoutingConnection($newid,'out'.$n,$did,$destport,$net);
		}
	      else {
		&addSimpleConnection($newid,'out'.$n,$did,$destport);
	      }
	    }
	    push @newids, $newid;
	}
	  while (scalar(@destids) > 0);
	@destids = @newids;
	$destport = 'in';
      }

    &addConnection($sourceid,$sourceport,$destids[0],$destport);
 
    return $destids[0];
  }


sub addRecursiveBroadcast
{
    my ($sourceid, $sourceport, $destid, $destport, $arity, $id) = @_;
    &addRecursiveScatter($sourceid, $sourceport, $destid, $destport, $nodeRoutingNode, $arity, $id);
}

sub addP2PMergeAndBroadcast
{
    my ($sourceid, $sourceport, $destid, $destport, $id) = @_;
    my $nbSrc = &getNb($sourceid);
    my $nbDest = &getNb($destid);
    &addObjects("$id/Merge",&getHosts($destid),&nodeFilterMerge($nbDest));
    &addRoutingNodes("$id/Diffuse",&getHosts($destid));
    my @dests = @{&getSubIds($destid)};
    my @srcs = @{&getSubIds($sourceid)};
    my $nbScatter = ceil($nbDest/$nbSrc);
    my $desti=0;
    my $srcp=0;
    my $srci=0;
    foreach my $dest (@dests)
    {
	if ($srcp == 0) {
	    my $nbS = $nbScatter;
	    $nbS = $nbDest-$desti if $nbDest-$desti < $nbScatter;
	    &addObject("$id/Scatter/".$srci,&getHost($srcs[$srci]),&nodeFilterScatter($nbS,8));
	    my $sid = $srcs[$srci];
	    &addSimpleConnection($sid, $sourceport, "$id/Scatter/".$srci, "in");
	}
	&addConnection("$id/Scatter/".$srci,"out".$srcp,"$id/Diffuse/".$desti,"in");
#	&addConnection("$id/Diffuse/".$desti,"out","$id/Merge","in".$desti);
	foreach my $mid (@{&getSubIds("$id/Merge")})
	{
	    if (!isSameHost(&getHost($mid),&getHost("$id/Scatter/".$srci)))
	    {
		&addSimpleConnection("$id/Diffuse/".$desti,"out",$mid,"in".$desti);
	    }
	    else
	    {
		&addSimpleConnection("$id/Scatter/".$srci,"out".$srcp,$mid,"in".$desti);
	    }
	}
	$desti++;
	$srcp++;
	if ($srcp == $nbScatter) {
	    $srci++;
	    $srcp = 0;
	}
    }
    &addConnection("$id/Merge","out",$destid,$destport);
}

sub addRoutingNodes
{
    my ($id, $hosts) = @_;
    &addObjects($id,$hosts,$nodeRoutingNode);
}

sub addRoutingNodeStamps
{
    my ($id, $hosts) = @_;
    &addObjects($id,$hosts,$nodeRoutingNodeStamps);
}

sub addCustomGreedy
{
    my ($sourceid, $sourceport, $destid, $destport, $filthosts, $synchosts, $destmodule, $id, $filtxml, $syncxml, $endit) = @_;
    $endit = 'endIt' if (!defined $endit);
    my $syncid = $id.'/sync';
    my $filterid = $id.'/filter';
    &addObjects($syncid, $synchosts, $syncxml);
    &addObjects($filterid, $filthosts, $filtxml);

    # NEW: Use routing nodes for input data to be sure it is really available on the dest hosts
    # NEW: Replace routing nodes by NOP filters (i.e. presignal with nb=0 to remove multisend and stamps and infinite loops
    my $inid = $id.'/in';
    #&addRoutingNodes($inid,$filthosts);
    #&addConnection($sourceid,$sourceport,$inid,'in');
    #&addConnection($inid,'out',$filterid,'in');
    &preSignal($sourceid,$sourceport,$filterid,'in',$inid,0);
    &addConnection($filterid,'out',$destid,$destport);

    if (&getNb($destmodule) > 1 && &getNb($syncid) == 1)
    {
	&andSignal($destmodule,$endit,$syncid,'endIt',$id.'/andit');
    }
    else
    {
      #&addConnectionStamps($destmodule,$endit,$syncid,'endIt','constraint=false');
       &addConnectionStamps($destmodule,$endit,$syncid,'endIt','');
    }
    if (&getNb($inid) > 1 && &getNb($syncid) == 1)
    {
	&andSignal($inid,'out',$syncid,'stamps',$id.'/and');
    }
    else
    {
	&addConnectionStamps($inid,'out',$syncid,'stamps');
    }
    #&addConnectionStamps($syncid,'order',$filterid,'order','constraint=false');
	  &addConnectionStamps($syncid,'order',$filterid,'order','');

    return $id;
}

sub addGreedy
{
    my ($sourceid, $sourceport, $destid, $destport, $filthosts, $synchosts, $destmodule, $id, $endit) = @_;
    $endit = 'endIt' if (!defined $endit);
    return &addCustomGreedy($sourceid, $sourceport, $destid, $destport, $filthosts, $synchosts, $destmodule, $id, $nodeFilterIt, $nodeGreedySynchronizer, $endit);
}

sub addOutCustomGreedy
{
    my ($sourceid, $sourceport, $destid, $destport, $filthosts, $synchosts, $destmodule, $id, $filtxml, $syncxml) = @_;
    return &addCustomGreedy($sourceid, $sourceport, $destid, $destport, $filthosts, $synchosts, $destmodule, $id, $filtxml, $syncxml, 'out');
}

sub addOutGreedy
{
    my ($sourceid, $sourceport, $destid, $destport, $filthosts, $synchosts, $destmodule, $id) = @_;
    return &addOutCustomGreedy($sourceid, $sourceport, $destid, $destport, $filthosts, $synchosts, $destmodule, $id, $nodeFilterIt, $nodeGreedySynchronizer);
}

# Add default traces to modules if  not already present 
# if $modules argument empty c onsider all modules
sub addTracetoModule
{
  my ($modules) = @_;

  my @modulelist; 

  # Take all modules if $module empty
  if (!$modules) 
    {
    @modulelist =  $doc->getElementsByTagName('module');
  }else{
    @modulelist = map{$xmlnode{$_}} @{$subids{$modules}}
  }
      
  foreach my $id  (@modulelist)
    {
      if ($id->getNodeName()  eq 'module')
	{

	  my @tracelistarray = $id->getElementsByTagName('tracelist');
	  my $tracelist;
	  # create tracelist if missing
	  if ( scalar(@tracelistarray) <= 0){
	    $tracelist  =&createElement('tracelist');
	    $id->appendChild($tracelist);
	  }else{
	    $tracelist = $tracelistarray[0];
	  }


	  my @ports = map{$_->getAttribute('id')} $id->getElementsByTagName('port');
#	  my @defaulttraces=('beginIt','endIt','waitBegin','waitEnd');
	  my @defaulttraces=('waitBegin','waitEnd');
	  foreach my  $i (@ports)
	    {
	      my $alreadyin = 0;
	      foreach my $j  (@defaulttraces)
		{
		  if ($i eq $j) 
		    {
		      $alreadyin = 1;
		    }
		}
	      if (!$alreadyin){@defaulttraces = (@defaulttraces,$i);}
	    }
	  
	  my @existingtraces = map{$_->getAttribute('id')}  $id->getElementsByTagName('trace');
	  foreach my  $i (@defaulttraces)
	    {
	      my $alreadyin = 0;
	      foreach my $j  (@existingtraces)
		{
		  if ($i eq $j) 
		    {
		      $alreadyin = 1;
		    }
		}

	      if (!$alreadyin)
		{
		  my $newtrace=createTree(createElement('trace',{'id' => $i}),
					  createTree(createElement('data'),
						     createElement('int')));
		  $tracelist->appendChild($newtrace);
		  if (defined $ENV{'DEBUG'})
		    {
		      my $modid = $id->getAttribute('id');
		      print STDERR "Add trace $i to  module $modid\n" 
		    }
		}
	    }
	}
    }
}


# Add default traces to synchronizers and filters  if  not already present 
# if $filter argument empty c onsider all filters and syncrhonizers
sub addTracetoFilterSync
{
  my ($filter) = @_;

  my @filterlist; 

  # Take all filters and sync  if $filter empty
  if (!$filter) 
    {
    @filterlist =  ($doc->getElementsByTagName('filter'),
		    $doc->getElementsByTagName('synchronizer'));
  }else{
    @filterlist = map{$xmlnode{$_}} @{$subids{$filter}}
  }
      
  foreach my $id  (@filterlist)
    {
      if ($id->getNodeName()  eq 'filter' ||
	  $id->getNodeName()  eq 'synchronizer' )
	  {
	    my @tracelistarray = $id->getElementsByTagName('tracelist');
	    my $tracelist;
	    # create tracelist if lacking
	    if ( scalar(@tracelistarray) <= 0){
	      $tracelist  = createElement('tracelist');
	      $id->appendChild($tracelist);
	    }else{
	      $tracelist = $tracelistarray[0];
	    }
	    
	    my @existingtraces =  $id->getElementsByTagName('trace');
	    my @tracestoadd = map{$_->getAttribute('id')} $id->getElementsByTagName('port');
	    foreach my  $i (@tracestoadd)
	      {
		my $alreadyin = 0;
		foreach my $j  (@existingtraces)
		{
		  if ($i eq $j) 
		    {
		      $alreadyin = 1;
		    }
		}
		
	      if (!$alreadyin)
		{
		  my $newtrace=createTree(createElement('trace',{'id' => $i}),
					  createTree(createElement('data'),
						     createElement('int')));
		  $tracelist->appendChild($newtrace);
		  if (defined $ENV{'DEBUG'})
		    {
		      my $modid = $id->getAttribute('id');
		      print STDERR "Add trace $i to  filter/sync  $id\n";
		    }
		}
	      }
	  }
    }
}

# Remove traces  from all modules, filters and sycnhronizers with prefix $object
sub removeTrace
{
  my ($object) = @_;

  foreach my $id  (map{$xmlnode{$_}} @{$subids{$object}})
    {
      if ($id->getNodeName()  eq 'module' ||
	  $id->getNodeName()  eq 'filter' ||
	  $id->getNodeName()  eq 'synchronizer' )
	{
	  my @tracelistarray = $id->getElementsByTagName('tracelist');
	  foreach my $i (@tracelistarray)
	    {
	      if (defined $ENV{'DEBUG'})
		{
		  my $did = $id->getAttribute('id');
		  print STDERR "Remove tracelist from  $did\n" 
		}
	      $id->removeChild($i);
	    }
	}
    }
}



#  Parse the input file, look for logger names in tracelists. For each logger, check that a fwrite module with id $fwrite is present on the same host,  create the logger and connect it this module. 

sub addLoggersWriters
{
  my ($fwrite,$buffernb,$buffersize,$period) = @_;

  
  my $id;
  # go for each fwrite module
  foreach $id (keys %xmlnode)
    {
      my $fwritenode = $xmlnode{$id};
      if ( $fwritenode->getNodeName()  eq 'module' &&  $fwritenode->getAttribute('id') =~ /^$fwrite/){
	my $fwritehost = $fwritenode->getAttribute('host');
	my $fwriteconnected = 0;

	# go for each module 
	foreach my  $i (keys %xmlnode)
	  {
	    my $node = $xmlnode{$i};
	    if ( ($node->getNodeName()  eq 'module' && $node->getAttribute('host') eq  $fwritehost) 
		 ||
		 ($node->getNodeName()  eq 'filter' && $node->getAttribute('host') eq  $fwritehost) 
		 ||
		 ($node->getNodeName()  eq 'synchronizer' && $node->getAttribute('host') eq  $fwritehost) 
	       )
	      {
		my @tracelist = $node->getElementsByTagName('trace');
		if ( $#tracelist >= 0 && 	 !$fwriteconnected )
		  {
		    # If changing  log/logger do it symmetrically into flowvr-gltrace
		    my $logger = 'log/logger/'.$fwritehost;
		    &addObject($logger,$fwritehost,&nodeLogger($buffernb,$buffersize,$period));
		    &addSimpleConnection($logger,'log',$fwritenode->getAttribute('id'),'in');
		    $fwriteconnected = 1;
		    print STDERR "Create logger $logger and connect it  to module  $id on host $fwritehost\n" if (defined $ENV{'DEBUG'});
		  }			  
	      }
	  }
	print STDERR "Warning: module $id useless because no logger connected to it  because nothing to trace on host $fwritehost\n" if(!$fwriteconnected);
      }
    }
}

# Set everything for  traces 
sub trace
{
    my ($fwrite,$buffernb,$buffersize,$period) = @_;

    # add default traces to all modules
    &addTracetoModule('');
    # add default traces to all filters and synchronizers
    &addTracetoFilterSync('');
    # remove traces from  all filters, synchronizers and modules with prefix log 
    # to avoid tracing the log/fwrite modules  and log/logger filters
    &removeTrace('log');

    # add  loggers 
    &addLoggersWriters($fwrite,$buffernb,$buffersize,$period);

    # Compute the source when using routing nodes 
    &fixSources;
}


sub moduleFrequency
{
    my ($moduleid,$freq) = @_;
    &addObject('FilterFrequency/'.$moduleid,&getHost($moduleid),&nodeFilterFrequency($freq));
    &addConnectionStamps('FilterFrequency/'.$moduleid,'out',$moduleid,'beginIt');
}

sub moduleMaxFrequency
{
    my ($moduleid,$freq) = @_;
    &addObject('MaxFrequencySynchronizor/'.$moduleid,&getHost($moduleid),&nodeMaxFrequencySynchronizor($freq));
    #&addConnectionStamps($moduleid,'endIt','MaxFrequencySynchronizor/'.$moduleid,'endIt','constraint=false');
	  &addConnectionStamps($moduleid,'endIt','MaxFrequencySynchronizor/'.$moduleid,'endIt','');
    &addConnectionStamps('MaxFrequencySynchronizor/'.$moduleid,'out',$moduleid,'beginIt');
}

sub preSignal
{
    my ($sourceid, $sourceport, $destid, $destport, $id, $nb) = @_;
    $nb = 1 if (!defined $nb);
    my $nodePreSignal = &createNodeFilter('flowvr.plugins.PreSignal',
					  createTree(createElement('parameters'),
						     createTree(createElement('nb'),
								createTextNode("$nb"))),
					  createTree(createElement('input'),
						     createTree(createElement('port',{'id' => 'in'}),createElement('datatype'))),
					  createTree(createElement('output'),
						     createTree(createElement('port',{'id' => 'out'}),createElement('datatype'))));
    &addObjects($id, &getHosts($destid), $nodePreSignal);
    #&addConnection($sourceid, $sourceport, $id, 'in', 'constraint=false');
	  &addConnection($sourceid, $sourceport, $id, 'in', '');
    &addConnection($id, 'out', $destid, $destport);
}

sub preSignalStamps
{
    my ($sourceid, $sourceport, $destid, $destport, $id, $nb) = @_;
    $nb = 1 if (!defined $nb);
    my $nodePreSignal = &createNodeFilter('flowvr.plugins.PreSignal',
					  createTree(createElement('parameters'),
						     createTree(createElement('nb'),
								createTextNode("$nb"))),
					  createTree(createElement('input'),
						     createElement('port',{'id' => 'in'})),
					  createTree(createElement('output'),
						     createElement('port',{'id' => 'out'})));
    &addObjects($id, &getHosts($destid), $nodePreSignal);
    #&addConnectionStamps($sourceid, $sourceport, $id, 'in', 'constraint=false');
     &addConnectionStamps($sourceid, $sourceport, $id, 'in', '');
    &addConnectionStamps($id, 'out', $destid, $destport);
}

sub preSignalGreedy
{
    my ($sourceid, $sourceport, $destid, $destport, $filthosts, $synchosts, $destmodule, $id) = @_;
    my $nb = 1;
    my $preid = $id.'/pre';
    my $nodePreSignal = &createNodeFilter('flowvr.plugins.PreSignal',
					  createTree(createElement('parameters'),
						     createTree(createElement('nb'),
								createTextNode("$nb"))),
					  createTree(createElement('input'),
						     createTree(createElement('port',{'id' => 'in'}),createElement('datatype'))),
					  createTree(createElement('output'),
						     createTree(createElement('port',{'id' => 'out'}),createElement('datatype'))));
    &addObjects($preid, $filthosts, $nodePreSignal);
    #&addConnection($sourceid, $sourceport, $preid, 'in', 'constraint=false');
	  &addConnection($sourceid, $sourceport, $preid, 'in', '');
    &addGreedy($preid, 'out', $destid, $destport, $filthosts, $synchosts, $destmodule, $id);
}

sub nodeFilterSignalAnd
{
  my ($nbports) = @_;
  my $node =&createNodeFilter('flowvr.plugins.SignalAnd',createTree(createElement('parameters'),createTree(createElement('nb'),createTextNode($nbports))));
  for (my $i = 0; $i < $nbports; $i++)
  {
    $node = &addPortStamps($node,'input','in'.$i);
  }
  $node = &addPortStamps($node,'output','out');
  return $node;
}

sub andSignal
{
    my ($sourceid, $sourceport, $destid, $destport, $id) = @_;
    my $nb = &getNb($sourceid);
    &addObjects($id, &getHosts($destid), &nodeFilterSignalAnd($nb));

    my @srcs = @{&getSubIds($sourceid)};
    my $i = 0;
    foreach my $src (@srcs) {
	&addConnectionStamps($src,$sourceport, $id, 'in'.$i);
	++$i;
    }
    &addConnectionStamps($id, 'out', $destid, $destport);
}

sub barrier
{
    my ($sourceid, $sourceport, $destid, $destport, $id, $nb) = @_;
    $nb = 0 if (!defined $nb);

    my @srcs = @{&getSubIds($sourceid)};

    my $nbin = &getNb($sourceid);

    &addObject($id.'/And', &getHost($destid), &nodeFilterSignalAnd($nbin));

    my $i = 0;
    foreach my $src (@srcs) {
	&addConnectionStamps($src,$sourceport, $id.'/And', 'in'.$i);
	++$i;
    }
    if ($nb>0) {
	&preSignalStamps($id.'/And','out',$destid,$destport,$id.'/Pre',$nb);
    } else {
	&addConnectionStamps($id.'/And','out',$destid,$destport);
    }
}

sub getOutputPorts
{
    my ($id) = @_;
    my @result = ();
    my $node = $xmlnode{$id};
    if ($node) # && $node->getNodeName() eq "module")
    {
	foreach my $inputs ($node->getElementsByTagName('output'))
	{
	    foreach my $port ($inputs->getElementsByTagName('port'))
	    {
		push @result, $port->getAttribute('id');
	    }
	}
    }
    return @result;
}

sub getInputPorts
{
    my ($id) = @_;
    my @result = ();
    my $node = $xmlnode{$id};
    if ($node) # && $node->getNodeName() eq "module")
    {
	foreach my $inputs ($node->getElementsByTagName('input'))
	{
	    foreach my $port ($inputs->getElementsByTagName('port'))
	    {
		push @result, $port->getAttribute('id');
	    }
	}
    }
    return @result;
}

sub hasOutputPort
{
    my ($id, $pname) = @_;
    my $node = $xmlnode{$id};
    if ($node) # && $node->getNodeName() eq "module")
    {
	foreach my $outputs ($node->getElementsByTagName('output'))
	{
	    foreach my $port ($outputs->getElementsByTagName('port'))
	    {
		if ($port->getAttribute('id') eq $pname)
		{
		    return 1;
		}
	    }
	}
    }
    return 0;
}

sub hasInputPort
{
    my ($id, $pname) = @_;
    my $node = $xmlnode{$id};
    if ($node) # && $node->getNodeName() eq "module")
    {
	foreach my $inputs ($node->getElementsByTagName('input'))
	{
	    foreach my $port ($inputs->getElementsByTagName('port'))
	    {
		if ($port->getAttribute('id') eq $pname)
		{
		    return 1;
		}
	    }
	}
    }
    return 0;
}

sub findObjectsWithOutputPort
{
    my ($pname) = @_;
    my @result = ();

    foreach my $id (keys %xmlnode)
    {
	if (hasOutputPort($id,$pname))
	{
	    push @result, $id;
	}
    }
    return @result;
}

sub findObjectsWithInputPort
{
    my ($pname) = @_;
    my @result = ();

    foreach my $id (keys %xmlnode)
    {
	if (hasInputPort($id,$pname))
	{
	    push @result, $id;
	}
    }
    return @result;
}

my %routingnodesource = ();

sub getFirstSource
{
    my ($routingnodeid) = @_;
    return $routingnodesource{$routingnodeid} if (defined $routingnodesource{$routingnodeid});

    my $conn = $connections{$routingnodeid};
    return undef if (!defined $conn);

    my $source = $conn->getElementsByTagName('source')->item(0)->getFirstChild();
    if ($source->getNodeName() eq 'routingnodeid')
    {
	my $newid = $source->getAttribute('id');
	$source = getFirstSource($newid);
    }
    my $id = $source->getAttribute('id');
    my $port = $source->getAttribute('port');
    print STDERR "$routingnodeid <- $id:$port\n" if (defined $ENV{'DEBUG'});
    $routingnodesource{$routingnodeid}=$source;
    return $source;
}

sub getFirstSourceStamps
{
    my ($routingnodeid) = @_;
    return $routingnodesource{$routingnodeid} if (defined $routingnodesource{$routingnodeid});
 
    my $conn = $connectionstamps{$routingnodeid};
    return &getFirstSource($routingnodeid) if (!defined $conn);

    my $source = $conn->getElementsByTagName('sourcestamps')->item(0)->getFirstChild();
    if ($source->getNodeName() eq 'routingnodeid')
    {
	my $newid = $source->getAttribute('id');
	$source = &getFirstSource($newid);
    }
    elsif ($source->getNodeName() eq 'routingnodestampsid')
    {
	my $newid = $source->getAttribute('id');
	$source = &getFirstSourceStamps($newid);
    }
    my $id = $source->getAttribute('id');
    my $port = $source->getAttribute('port');
    print STDERR "$routingnodeid <- $id:$port\n" if (defined $ENV{'DEBUG'});
    $routingnodesource{$routingnodeid}=$source;
    return $source;
}

sub fixSources
{
    foreach my $c (keys %connections)
    {
	my $source = $connections{$c}->getElementsByTagName('source')->item(0)->getFirstChild();
	my $id = $source->getAttribute('id');
	print STDERR "> ".$source->getNodeName()." $id\n" if (defined $ENV{'DEBUG'});
	if ($source->getNodeName() eq 'routingnodeid' && !defined $source->getFirstChild())
	{
	    print STDERR "? $id\n" if (defined $ENV{'DEBUG'});
	    my $first = &getFirstSource($id);
	    $source->appendChild($first->cloneNode(1)) if (defined $first);
	}
    }

    foreach my $c (keys %connectionstamps)
    {
	my $source = $connectionstamps{$c}->getElementsByTagName('sourcestamps')->item(0)->getFirstChild();
	my $id = $source->getAttribute('id');
	print STDERR "> ".$source->getNodeName()." $id\n" if (defined $ENV{'DEBUG'});
	if ($source->getNodeName() eq 'routingnodeid' && !defined $source->getFirstChild())
	{
	    print STDERR "? $id\n" if (defined $ENV{'DEBUG'});
	    my $first = &getFirstSource($id);
	    $source->appendChild($first->cloneNode(1)) if (defined $first);
	}
	elsif ($source->getNodeName() eq 'routingnodestampsid' && !defined $source->getFirstChild())
	{
	    print STDERR "? $id\n" if (defined $ENV{'DEBUG'});
	    my $first = &getFirstSourceStamps($id);
	    $source->appendChild($first->cloneNode(1)) if (defined $first);
	}
    }
}


# First find all objects and launch them

my $lastdest = "127.0.0.1";

sub sendCommand
{
    my ($dest, @cmds) = @_;
    if (($dest ne "") && ($dest ne $lastdest))
    {
	my $cmd = &createTree(&createElement('dest'),&createTextNode($dest));
	print $cmd->toString."\n";
	$lastdest = $dest;
    }
    foreach my $c (@cmds)
    {
	print $c->toString."\n";
    }
}

my %networkroutes=();

sub compareID
{
    $a->getAttribute('id') cmp $b->getAttribute('id');
}

sub sendCommandsAddObject
{
    my $id;
    foreach $id (keys %xmlnode)
    {
	my $node = $xmlnode{$id};
	my $type = $node->getNodeName();
	next if ($type eq 'routingnode');
	next if ($type eq 'routingnodestamps');
	my $host = $node->getAttribute('host');
	if ($type eq 'module')
	{
	    &sendCommand($host, &createElement('addobject',{'id'=>$id,'class'=>'flowvr.plugins.Regulator'}));
	}
	elsif ($type eq 'filter')
	{
	    my $class = $node->getElementsByTagName('filterclass')->item(0)->getFirstChild()->getNodeValue();
	    my $cmd = &createElement('addobject',{'id'=>$id,'class'=>$class});
	    foreach my $parameters ($node->getElementsByTagName('parameters'))
	    {
		foreach my $param ($parameters->getChildNodes())
		{
		    $cmd->appendChild($param->cloneNode(1));
		}
	    }
	    &sendCommand($host, $cmd);
	}
	elsif ($type eq 'synchronizer')
	{
	    my $class = $node->getElementsByTagName('synchronizerclass')->item(0)->getFirstChild()->getNodeValue();#->getTextContent();
	    my $cmd = &createElement('addobject',{'id'=>$id,'class'=>$class});
	    foreach my $parameters ($node->getElementsByTagName('parameters'))
	    {
		foreach my $param ($parameters->getChildNodes())
		{
		    $cmd->appendChild($param->cloneNode(1));
		}
	    }
	    &sendCommand($host, $cmd);
	}
    }
    
    &sendCommand("",&createElement('flush'));
}

sub sendCommandsAddRoute
{
    my $conn;
    #foreach $conn (sort(compareID,values %connections))
    foreach $conn (values %connections)
    {
	my $cmd = &createElement('addroute',{'id'=>$conn->getAttribute('id')});
	my $source = $conn->getElementsByTagName('source')->item(0)->getFirstChild();
	my $sourcetype = $source->getNodeName();
	my $s0 = $source;
	if ($sourcetype eq 'routingnodeid')
	{
	    $s0 = &getFirstSource($source->getAttribute('id')) 
	}
	elsif ($sourcetype eq 'routingnodestampsid')
	{
	    $s0 = &getFirstSourceStamps($source->getAttribute('id')) 
	}
	my $s0id = $s0->getAttribute('id');
	my $s0port = $s0->getAttribute('port');
	my $sourcehost = $xmlnode{$source->getAttribute('id')}->getAttribute('host');
	my $cmdsource = createElement('source',{ 'id'=>$s0id, 'port'=>$s0port});
	$cmd->appendChild($cmdsource);
	my $dest = $conn->getElementsByTagName('destination')->item(0)->getFirstChild();
	my $desttype = $dest->getNodeName();
	my $destid = $dest->getAttribute('id');
	my $destport = $dest->getAttribute('port');
	my $destnode = $xmlnode{$destid};
	my $desthost = $destnode->getAttribute('host');
	if ($desthost ne $sourcehost)
	{
	    my $rid = $sourcehost.'/NET'.'/full/full/'.$desthost.$s0id.':'.$s0port;
	    if (!defined $networkroutes{$rid})
	    {
		$networkroutes{$rid} = $conn->getAttribute('id');
		my $cmd0 = $cmd->cloneNode(1);
		$cmd0->appendChild(&createTree(&createElement('action',{'id'=>'/NET'}),&createTree(&createElement('dest'),&createTextNode($desthost))));
		&sendCommand($sourcehost, $cmd0);
	    }
	}
	if ($desttype !~ /^routingnode/)
	{
	    $cmd->appendChild(&createTree(&createElement('action',{'id'=>$destid}),&createTree(&createElement('port'),&createTextNode($destport))));
	    &sendCommand($desthost, $cmd);
	}
    }

    #foreach $conn (sort(compareID,values %connectionstamps))
    foreach $conn (values %connectionstamps)
    {
	my $cmd = &createElement('addroute',{'id'=>$conn->getAttribute('id')});
	my $source = $conn->getElementsByTagName('sourcestamps')->item(0)->getFirstChild();
	my $sourcetype = $source->getNodeName();
	my $s0 = $source;
	if ($sourcetype eq 'routingnodeid')
	{
	    $s0 = &getFirstSource($source->getAttribute('id')) 
	}
	elsif ($sourcetype eq 'routingnodestampsid')
	{
	    $s0 = &getFirstSourceStamps($source->getAttribute('id')) 
	}
	my $s0id = $s0->getAttribute('id');
	my $s0port = $s0->getAttribute('port');
	my $msgtype = 'stamps';
	my $sourcehost = $xmlnode{$source->getAttribute('id')}->getAttribute('host');
	if ($sourcetype eq 'routingnodeid')
	{ # a routing node always deliver full messages
	    $msgtype = 'full';
	}
	elsif ($sourcetype eq 'moduleid' || $sourcetype eq 'filterid')
	{ # only modules and filters are ambigous
	    my $s0node = $xmlnode{$s0id};
	    foreach my $p ($s0node->getElementsByTagName('port'))
	    {
		if ($p->getAttribute('id') eq $s0port)
		{
		    $msgtype = 'full' if ($p->getElementsByTagName('datatype')->getLength()>0);
		    last; # port found
		}
	    }
	}
	my $cmdsource = createElement('source');#,{ 'id'=>$s0id, 'port'=>$s0port, 'messagetype'=>$msgtype});
	$cmdsource->setAttribute('id',$s0id);
	$cmdsource->setAttribute('port',$s0port);
	$cmdsource->setAttribute('messagetype',$msgtype);

	$cmd->appendChild($cmdsource);
	my $dest = $conn->getElementsByTagName('destinationstamps')->item(0)->getFirstChild();
	my $desttype = $dest->getNodeName();
	my $destid = $dest->getAttribute('id');
	my $destport = $dest->getAttribute('port');
	my $destnode = $xmlnode{$destid};
	my $desthost = $destnode->getAttribute('host');
	if ($desthost ne $sourcehost)
	{
	    my $rid = $sourcehost.'/NET'.'/'.$msgtype.'/stamps/'.$desthost.$s0id.':'.$s0port;
	    if (!defined $networkroutes{$rid})
	    {
		$networkroutes{$rid} = $conn->getAttribute('id');
		my $cmd0 = $cmd->cloneNode(1);
		$cmd0->appendChild(&createTree(&createElement('action',{'id'=>'/NET', 'messagetype'=>'stamps'}),&createTree(&createElement('dest'),&createTextNode($desthost))));
		&sendCommand($sourcehost, $cmd0);
	    }
	    $cmdsource->setAttribute('messagetype','stamps'); # on the destination node only stamps are received
	}
	if ($desttype !~ /^routingnode/)
	{
	    $cmd->appendChild(&createTree(&createElement('action',{'id'=>$destid, 'messagetype'=>'stamps'}),&createTree(&createElement('port'),&createTextNode($destport))));
	    &sendCommand($desthost, $cmd);
	}
    }
    
    &sendCommand("",&createElement('flush'));
}

sub sendCommandsStart
{
    foreach my $id (keys %xmlnode)
    {
	my $node = $xmlnode{$id};
	my $type = $node->getNodeName();
	next if ($type eq 'routingnode');
	next if ($type eq 'routingnodestamps');
	my $host = $node->getAttribute('host');
	&sendCommand($host, &createTree(&createElement('action',{'id'=>$id}),&createElement('start')));
    }    
    &sendCommand("",&createElement('flush'));
}

sub sendCommands
{
    print '<?xml version="1.0" encoding="ISO-8859-1"?>'."\n";
    print '<!DOCTYPE commands SYSTEM "http://flowvr.sf.net/flowvr-parse/dtd/commands.dtd" >'."\n";
    print '<commands>'."\n";

    &sendCommandsAddObject;
    
# Then create connections
    &sendCommandsAddRoute;
    
# Finally start all objects
    &sendCommandsStart;

    print '</commands>'."\n";
}

sub printResult
{
    #&fixSources;
    if ((scalar(@ARGV)>0) && ($ARGV[-1] eq 'COMMANDS'))
    {
	&sendCommands;
    }
    else
    {
	&printDoc;
    }
}

# Autoload methods go after =cut, and are processed by the autosplit program.

1;
__END__
# Below is stub documentation for your module. You'd better edit it!

=head1 NAME

FlowVR::XML - Perl extension for blah blah blah

=head1 SYNOPSIS

  use FlowVR::XML;
  blah blah blah

=head1 DESCRIPTION

Stub documentation for FlowVR::XML, created by h2xs. It looks like the
author of the extension was negligent enough to leave the stub
unedited.

Blah blah blah.

=head2 EXPORT

None by default.



=head1 SEE ALSO

Mention other useful documentation such as the documentation of
related modules or operating system documentation (such as man pages
in UNIX), or any relevant external documentation such as RFCs or
standards.

If you have a mailing list set up for your module, mention it here.

If you have a web site set up for your module, mention it here.

=head1 AUTHOR

Jérémie Allard (Jeremie.Allard@imag.fr), E<lt>allardj@localdomainE<gt>

=head1 COPYRIGHT AND LICENSE

Copyright 2004 by Jérémie Allard (Jeremie.Allard@imag.fr)

This library is free software; you can redistribute it and/or modify
it under the same terms as Perl itself. 

=cut
