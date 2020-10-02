/*
 *      192.168.10.0                       192.168.50.0
 n0 ------------------------ n1-------------------------------- n2
         point-to-point               point-to-point
          5Mbps, 2ms                    1Mbps, 2ms 
 *
 *
 */

#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"
#include "ns3/packet-sink.h"
#include "ns3/onoff-application.h"
#include "ns3/on-off-helper.h"
#include "ns3/packet-sink-helper.h"

using namespace ns3;

// Enable logging
NS_LOG_COMPONENT_DEFINE ("FirstScriptExample");

int
main (int argc, char *argv[])
{

  // use command line to turn log on/off
  bool verbose = true;
  CommandLine cmd;
  cmd.AddValue("verbose", "Tell application to log if ture", verbose);
  cmd.Parse (argc, argv);
  
  // Logging information
  if (verbose) {
    LogComponentEnable ("OnOffApplication", LOG_LEVEL_INFO);
    LogComponentEnable ("PacketSink", LOG_LEVEL_INFO);
  }

  // set resolution
  Time::SetResolution (Time::NS);

  // Add some custom logging information
  NS_LOG_INFO("Create Topology");

  // Container of Nodes
  NodeContainer pppNodes1, pppNodes2;
  pppNodes1.Create (2);
  pppNodes2.Add (pppNodes1.Get (1));
  pppNodes2.Create (1);

  // channel
  PointToPointHelper pointToPoint1, pointToPoint2;
  pointToPoint1.SetDeviceAttribute ("DataRate", StringValue ("5Mbps"));
  pointToPoint1.SetChannelAttribute ("Delay", StringValue ("2ms"));
  pointToPoint2.SetDeviceAttribute ("DataRate", StringValue ("1Mbps"));
  pointToPoint2.SetChannelAttribute ("Delay", StringValue ("2ms"));

  // install the device and IP protocol stack
  NetDeviceContainer devices1, devices2;
  devices1 = pointToPoint1.Install (pppNodes1);
  devices2 = pointToPoint2.Install (pppNodes2);

  InternetStackHelper stack1, stack2;
  stack1.Install (pppNodes1);
  stack2.Install (pppNodes2.Get (1));

  // Create an IP address pool and assign the IP addresses
  Ipv4AddressHelper address1, address2;
  address1.SetBase ("192.168.10.0", "255.255.255.0");
  address2.SetBase ("192.168.50.0", "255.255.255.0");
  uint16_t portNum = 12345;

  Ipv4InterfaceContainer interfaces1 = address1.Assign (devices1);
  Ipv4InterfaceContainer interfaces2 = address2.Assign (devices2);

  // enable routing
  Ipv4GlobalRoutingHelper::PopulateRoutingTables ();

  // install onoff application
  OnOffHelper onOffHelper ("ns3::TcpSocketFactory", InetSocketAddress(interfaces2.GetAddress (1), portNum));
  // incorrect onOffHelper construction
  // OnOffHelper onOffHelper ("ns3::TcpSocketFactory", interfaces2.GetAddress (1));
  onOffHelper.SetAttribute ("PacketSize", UintegerValue (512));
  onOffHelper.SetAttribute ("DataRate", StringValue ("50kbps"));
  onOffHelper.SetAttribute ("MaxBytes", UintegerValue (2000));
  onOffHelper.SetAttribute ("OnTime", StringValue ("ns3::ConstantRandomVariable[Constant=1]"));
  onOffHelper.SetAttribute ("OffTime", StringValue ("ns3::ConstantRandomVariable[Constant=0]"));

  ApplicationContainer clientApps = onOffHelper.Install (pppNodes1.Get (0));
  clientApps.Start (Seconds (2.0));
  clientApps.Stop (Seconds (3.0));

  // install packet sink application
  PacketSinkHelper packetSinkHelper ("ns3::TcpSocketFactory", InetSocketAddress(interfaces2.GetAddress (1), portNum));
  ApplicationContainer serverApps = packetSinkHelper.Install (pppNodes2.Get (1));
  serverApps.Start (Seconds (1.0));
  serverApps.Stop (Seconds (3.0));

  // add some pcap tracing
  pointToPoint2.EnablePcapAll ("Lab2");
  pointToPoint1.EnablePcapAll ("Lab2");
 
  Simulator::Run ();
  Simulator::Destroy ();
  return 0;
}
