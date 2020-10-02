#include "ns3/core-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/network-module.h"
#include "ns3/applications-module.h"
#include "ns3/wifi-module.h"
#include "ns3/mobility-module.h"
#include "ns3/csma-module.h"
#include "ns3/internet-module.h"

/*
Wifi 10.1.3.0
*  *  *
|  |  |
n3 n4 n5      p2p (100Mbps, 2ms)             p2p(100Mbps, 2ms)
    A (n0)------------------------B(n1)---------------------------C(n2)
   (AP)            10.1.1.0                    1 0.1.2.0          (AP)
                               	         		                    n6  n7  n8
                                 	             		              |   |   |
                                   	        	  	              *   *   *
                                       	      	 		           Wifi 10.1.4.0
*/


using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("Lab3");

int 
main (int argc, char *argv[])
{
  bool verbose = true;
  bool tracing = true;
  uint32_t nWifi1 = 2;
  uint32_t nWifi2 = 2;

  CommandLine cmd;
  cmd.AddValue ("nCsma", "Number of wifi1 STA devices", nWifi1);
  cmd.AddValue ("nWifi", "Number of wifi2 STA devices", nWifi2);
  cmd.AddValue ("verbose", "Tell echo applications to log if true", verbose);
  cmd.AddValue ("tracing", "Enable pcap tracing", tracing);

  cmd.Parse (argc,argv);

  // The underlying restriction of 18 is due to the grid position
  // allocator's configuration; the grid layout will exceed the
  // bounding box if more than 18 nodes are provided.
  if (nWifi1 > 18 || nWifi2 > 18)
    {
      std::cout << "nWifi should be 18 or less; otherwise grid layout exceeds the bounding box" << std::endl;
      return 1;
    }

  if (verbose)
    {
      LogComponentEnable ("UdpEchoClientApplication", LOG_LEVEL_INFO);
      LogComponentEnable ("UdpEchoServerApplication", LOG_LEVEL_INFO);
    }

  // Create ppp channel
  // the first one
  NodeContainer p2pNodes1;
  p2pNodes1.Create (2);
  
  PointToPointHelper pointToPoint;
  pointToPoint.SetDeviceAttribute ("DataRate", StringValue ("100Mbps"));
  pointToPoint.SetChannelAttribute ("Delay", StringValue ("2ms"));
  
  NetDeviceContainer p2pDevices1 = pointToPoint.Install (p2pNodes1);

  // the second one
  NodeContainer p2pNodes2;
  p2pNodes2.Add (p2pNodes1.Get (1));
  p2pNodes2.Create (1);
  
  NetDeviceContainer p2pDevices2 = pointToPoint.Install (p2pNodes2);
  
  // Create the first wifi subnet
  NodeContainer wifiStaNodes1;
  wifiStaNodes1.Create (nWifi1);
  NodeContainer wifiApNode1 = p2pNodes1.Get (0);

  YansWifiChannelHelper channel1 = YansWifiChannelHelper::Default ();
  YansWifiPhyHelper phy1 = YansWifiPhyHelper::Default ();
  phy1.SetChannel (channel1.Create ());

  WifiHelper wifi;
  wifi.SetRemoteStationManager ("ns3::AarfWifiManager");

  WifiMacHelper mac1;

//Ssid ssid1 ("ns-3-ssid1");
  Ssid ssid1 ("ns-3-ssid");
  mac1.SetType ("ns3::StaWifiMac",
                "Ssid", SsidValue (ssid1),
                "ActiveProbing", BooleanValue (false));
  
  NetDeviceContainer staDevices1;
  staDevices1 = wifi.Install (phy1, mac1, wifiStaNodes1);

  mac1.SetType ("ns3::ApWifiMac",
                "Ssid", SsidValue (ssid1));

  NetDeviceContainer apDevices1;
  apDevices1 = wifi.Install (phy1, mac1, wifiApNode1);
  
  // Create the second wifi subnet
  NodeContainer wifiStaNodes2;
  wifiStaNodes2.Create (nWifi2);
  NodeContainer wifiApNode2 = p2pNodes2.Get (1);

  YansWifiChannelHelper channel2 = YansWifiChannelHelper::Default ();
  YansWifiPhyHelper phy2 = YansWifiPhyHelper::Default ();

  phy2.SetChannel (channel2.Create ());

  //Ssid ssid2 ("ns-3-ssid2");
  Ssid ssid2 ("ns-3-ssid");

  WifiMacHelper mac2;
  mac2.SetType ("ns3::StaWifiMac",
                "Ssid", SsidValue (ssid2),
                "ActiveProbing", BooleanValue (false));
  
  NetDeviceContainer staDevices2;
  staDevices2 = wifi.Install (phy2, mac2, wifiStaNodes2);

  mac2.SetType ("ns3::ApWifiMac",
                "Ssid", SsidValue (ssid2));

  NetDeviceContainer apDevices2;
  apDevices2 = wifi.Install (phy2, mac2, wifiApNode2);

  // specify how the mobile nodes will move around.
  // using mobility model
  MobilityHelper mobility;

  mobility.SetPositionAllocator ("ns3::GridPositionAllocator",
                                 "MinX", DoubleValue (0.0),
                                 "MinY", DoubleValue (0.0),
                                 "DeltaX", DoubleValue (5.0), /* 5.0 meters */
                                 "DeltaY", DoubleValue (10.0),
                                 "GridWidth", UintegerValue (3),
                                 "LayoutType", StringValue ("RowFirst"));

  mobility.SetMobilityModel ("ns3::RandomWalk2dMobilityModel",
                             "Bounds", RectangleValue (Rectangle (-50, 50, -50, 50)));
  mobility.Install (wifiStaNodes1);
  mobility.Install (wifiStaNodes2);

  mobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
  mobility.Install (wifiApNode1);
  mobility.Install (wifiApNode2);

  // install the internet protocol stack
  // note do not re-install on junction nodes
  InternetStackHelper stack;
  stack.Install (p2pNodes1.Get (0));
  stack.Install (p2pNodes2);
  stack.Install (wifiStaNodes1);
  stack.Install (wifiStaNodes2);

  // assign ipv4 address
  Ipv4AddressHelper address;

  address.SetBase ("10.1.1.0", "255.255.255.0");
  Ipv4InterfaceContainer p2pInterfaces1;
  p2pInterfaces1 = address.Assign (p2pDevices1);

  address.SetBase ("10.1.2.0", "255.255.255.0");
  Ipv4InterfaceContainer p2pInterfaces2;
  p2pInterfaces2 = address.Assign (p2pDevices2);

  address.SetBase ("10.1.3.0", "255.255.255.0");
  Ipv4InterfaceContainer wifiStaInterfaces1;
  address.Assign (staDevices1);
  address.Assign (apDevices1);

  address.SetBase ("10.1.4.0", "255.255.255.0");
  Ipv4InterfaceContainer wifiStaInterfaces2;
  wifiStaInterfaces2 = address.Assign (staDevices2);
  address.Assign (apDevices2);

  // create udpechoserver
  UdpEchoServerHelper echoServer (9);

  ApplicationContainer serverApps = echoServer.Install (wifiStaNodes2.Get (nWifi2 - 1));
  serverApps.Start (Seconds (1.0));
  serverApps.Stop (Seconds (10.0));

  // create udpechoclient
  UdpEchoClientHelper echoClient (wifiStaInterfaces2.GetAddress (nWifi2 - 1), 9);
  echoClient.SetAttribute ("MaxPackets", UintegerValue (2));
  echoClient.SetAttribute ("Interval", TimeValue (Seconds (1.0)));
  echoClient.SetAttribute ("PacketSize", UintegerValue (1024));

  ApplicationContainer clientApps = echoClient.Install (wifiStaNodes1.Get (nWifi1 - 1));
  clientApps.Start (Seconds (2.0));
  clientApps.Stop (Seconds (10.0));

  // enable routing
  Ipv4GlobalRoutingHelper::PopulateRoutingTables ();

  Simulator::Stop (Seconds (10.0));
  // .pcap log files on client and serevr
  pointToPoint.EnablePcapAll ("lab3");
  phy1.EnablePcap ("lab3", apDevices1.Get (0));
  phy2.EnablePcap ("lab3", apDevices2.Get (0));
  
  Simulator::Run ();
  Simulator::Destroy ();
  return 0;
}

