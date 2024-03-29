#include <cstdlib>
#include<time.h>
#include <stdio.h>
#include <string>
#include <fstream>

#include "ns3/core-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/network-module.h"
#include "ns3/applications-module.h"
#include "ns3/mobility-module.h"
#include "ns3/csma-module.h"
#include "ns3/internet-module.h"
#include "ns3/yans-wifi-helper.h"
#include "ns3/ssid.h"
#include "ns3/core-module.h"
#include "ns3/internet-module.h"
#include "ns3/applications-module.h"
#include "ns3/network-module.h"
#include "ns3/packet-sink.h"
#include "ns3/error-model.h"
#include "ns3/udp-header.h"
#include "ns3/enum.h"
#include "ns3/event-id.h"
#include "ns3/flow-monitor-helper.h"
#include "ns3/ipv4-global-routing-helper.h"
#include "ns3/traffic-control-module.h"
#include "ns3/flow-monitor-module.h"

#include <map>
#include <vector>

using namespace ns3;
using namespace std;
NS_LOG_COMPONENT_DEFINE ("WifiTopology");

std::map<int, char> mapper1_mapping = {{0, 'a'},{1, 'b'},{2, 'c'},{3, 'd'},{4, 'e'},{5, 'f'},{6, 'g'},{7, 'h'},{8, 'i'}};
std::map<int, char> mapper2_mapping = {{9, 'j'},{10, 'k'},{11, 'l'},{12, 'm'},{13, 'n'},{14, 'o'},{15, 'p'},{16, 'q'},{17, 'r'}};
std::map<int, char> mapper3_mapping = {{18, 's'},{19, 't'},{20, 'u'},{21, 'v'},{22, 'w'},{23, 'x'},{24, 'y'},{25, 'z'},{26, ' '},{27, '-'}};

void
ThroughputMonitor (FlowMonitorHelper *fmhelper, Ptr<FlowMonitor> flowMon, double em)
{
    uint16_t i = 0;

    std::map<FlowId, FlowMonitor::FlowStats> flowStats = flowMon->GetFlowStats ();

    Ptr<Ipv4FlowClassifier> classing = DynamicCast<Ipv4FlowClassifier> (fmhelper->GetClassifier ());
    for (std::map<FlowId, FlowMonitor::FlowStats>::const_iterator stats = flowStats.begin (); stats != flowStats.end (); ++stats)
    {
        Ipv4FlowClassifier::FiveTuple fiveTuple = classing->FindFlow (stats->first);

        std::cout << "Flow ID			: "<< stats->first << " ; " << fiveTuple.sourceAddress << " -----> " << fiveTuple.destinationAddress << std::endl;
        std::cout << "Tx Packets = " << stats->second.txPackets << std::endl;
        std::cout << "Rx Packets = " << stats->second.rxPackets << std::endl;
        std::cout << "Duration		: "<< (stats->second.timeLastRxPacket.GetSeconds () - stats->second.timeFirstTxPacket.GetSeconds ()) << std::endl;
        std::cout << "Last Received Packet	: "<< stats->second.timeLastRxPacket.GetSeconds () << " Seconds" << std::endl;
        std::cout << "Throughput: " << stats->second.rxBytes * 8.0 / (stats->second.timeLastRxPacket.GetSeconds () - stats->second.timeFirstTxPacket.GetSeconds ()) / 1024 / 1024  << " Mbps" << std::endl;
    
        i++;

        std::cout << "---------------------------------------------------------------------------" << std::endl;
    }

    Simulator::Schedule (Seconds (1.0),&ThroughputMonitor, fmhelper, flowMon, em);
}

void
AverageDelayMonitor (FlowMonitorHelper *fmhelper, Ptr<FlowMonitor> flowMon, double em)
{
    uint16_t i = 0;

    std::map<FlowId, FlowMonitor::FlowStats> flowStats = flowMon->GetFlowStats ();
    Ptr<Ipv4FlowClassifier> classing = DynamicCast<Ipv4FlowClassifier> (fmhelper->GetClassifier ());
    for (std::map<FlowId, FlowMonitor::FlowStats>::const_iterator stats = flowStats.begin (); stats != flowStats.end (); ++stats)
    {
        Ipv4FlowClassifier::FiveTuple fiveTuple = classing->FindFlow (stats->first);
        std::cout << "Flow ID			: "<< stats->first << " ; " << fiveTuple.sourceAddress << " -----> " << fiveTuple.destinationAddress << std::endl;
        std::cout << "Tx Packets = " << stats->second.txPackets << std::endl;
        std::cout << "Rx Packets = " << stats->second.rxPackets << std::endl;
        std::cout << "Duration		: "<< (stats->second.timeLastRxPacket.GetSeconds () - stats->second.timeFirstTxPacket.GetSeconds ()) << std::endl;
        std::cout << "Last Received Packet	: "<< stats->second.timeLastRxPacket.GetSeconds () << " Seconds" << std::endl;
        std::cout << "Sum of e2e Delay: " << stats->second.delaySum.GetSeconds () << " s" << std::endl;
        std::cout << "Average of e2e Delay: " << stats->second.delaySum.GetSeconds () / stats->second.rxPackets << " s" << std::endl;
    
        i++;

        std::cout << "---------------------------------------------------------------------------" << std::endl;
    }

    Simulator::Schedule (Seconds (1.0),&AverageDelayMonitor, fmhelper, flowMon, em);
}

class MyHeader : public Header 
{
public:
    MyHeader ();
    virtual ~MyHeader ();
    void SetData (uint16_t data);
    uint16_t GetData (void) const;
    static TypeId GetTypeId (void);
    virtual TypeId GetInstanceTypeId (void) const;
    virtual void Print (std::ostream &os) const;
    virtual void Serialize (Buffer::Iterator start) const;
    virtual uint32_t Deserialize (Buffer::Iterator start);
    virtual uint32_t GetSerializedSize (void) const;
    void SetIp (Ipv4Address ip);
    void SetPort (uint16_t port);
    Ipv4Address GetIp (void) const;
    uint16_t GetPort (void) const;
    
private:
    uint16_t m_data;
    Ipv4Address m_ip;
    uint16_t m_port;
};

MyHeader::MyHeader ()
{
}

MyHeader::~MyHeader ()
{
}

TypeId
MyHeader::GetTypeId (void)
{
    static TypeId tid = TypeId ("ns3::MyHeader")
        .SetParent<Header> ()
        .AddConstructor<MyHeader> ()
    ;
    return tid;
}

TypeId
MyHeader::GetInstanceTypeId (void) const
{
    return GetTypeId ();
}

void
MyHeader::Print (std::ostream &os) const
{
    os << "data = " << m_data << endl;
}

uint32_t
MyHeader::GetSerializedSize (void) const
{
    return 8;
}

void
MyHeader::Serialize (Buffer::Iterator start) const
{
    start.WriteHtonU16 (m_data);
    start.WriteHtonU32(m_ip.Get());
    start.WriteHtonU16(m_port);
}

uint32_t
MyHeader::Deserialize (Buffer::Iterator start)
{
    m_data = start.ReadNtohU16 ();
    m_ip.Set(start.ReadNtohU32());
    m_port = start.ReadNtohU16();
    return 8;
}

void 
MyHeader::SetData (uint16_t data)
{
    m_data = data;
}

uint16_t 
MyHeader::GetData (void) const
{
    return m_data;
}

void
MyHeader::SetIp (Ipv4Address ip)
{
    m_ip = ip;
}

void
MyHeader::SetPort (uint16_t port)
{
    m_port = port;
}

Ipv4Address
MyHeader::GetIp (void) const
{
    return m_ip;
}

uint16_t
MyHeader::GetPort (void) const
{
    return m_port;
}

class master : public Application
{
public:
    master (uint16_t port, Ipv4InterfaceContainer& ip ,
            uint16_t mapper_1_port, uint16_t mapper_2_port, uint16_t mapper_3_port,
            Ipv4InterfaceContainer& mapper_ip);
    virtual ~master ();
private:
    virtual void StartApplication (void);
    void HandleRead (Ptr<Socket> socket);

    uint16_t port;
    Ipv4InterfaceContainer ip;
    uint16_t mapper_1_port;
    uint16_t mapper_2_port;
    uint16_t mapper_3_port;
    Ipv4InterfaceContainer mapper_ip;
    Ptr<Socket> socket;
    Ptr<Socket> mapper_1_socket;
    Ptr<Socket> mapper_2_socket;
    Ptr<Socket> mapper_3_socket;
};


class client : public Application
{
public:
    client (uint16_t port, Ipv4InterfaceContainer& ip, uint16_t master_port, Ipv4InterfaceContainer& master_ip);
    virtual ~client ();

private:
    virtual void StartApplication (void);
    void HandleRead (Ptr<Socket> socket);

    uint16_t port;
    Ipv4InterfaceContainer ip;
    uint16_t master_port;
    Ipv4InterfaceContainer master_ip;
    std::string result = "";
    std::vector<uint16_t> input = {2,14,12,15,20,19,4,17,27,13,4,19,22,14,17,10,26};
    uint16_t indx = 0;
};

class mapper : public Application
{
public:
    mapper (uint16_t port, Ipv4InterfaceContainer& ip, std::map<int, char> map_set, int i);
    virtual ~mapper ();

private:
    virtual void StartApplication (void);
    void HandleRead (Ptr<Socket> socket);
    void HandleAccept (Ptr<Socket> socket, const Address& from);
    
    uint16_t port;
    Ptr<Socket> socket;
    Ptr<Socket> client_socket;
    Ipv4InterfaceContainer ip;
    std::map<int, char> map_set;
    int i;
};


int
main (int argc, char *argv[])
{
    double error = 0.000001;
    string bandwidth = "1Mbps";
    bool verbose = true;
    double duration = 60.0;
    bool tracing = false;

    srand(time(NULL));

    CommandLine cmd (__FILE__);
    cmd.AddValue ("verbose", "Tell echo applications to log if true", verbose);
    cmd.AddValue ("tracing", "Enable pcap tracing", tracing);

    cmd.Parse (argc,argv);

    if (verbose)
    {
        LogComponentEnable ("UdpEchoClientApplication", LOG_LEVEL_INFO);
        LogComponentEnable ("UdpEchoServerApplication", LOG_LEVEL_INFO);
    }

    NodeContainer wifiStaNodeClient;
    wifiStaNodeClient.Create (1);

    NodeContainer wifiStaNodeMaster;
    wifiStaNodeMaster.Create (1);
    
    NodeContainer wifiStaNodeMapper;
    wifiStaNodeMapper.Create (3);

    YansWifiChannelHelper channel = YansWifiChannelHelper::Default ();

    YansWifiPhyHelper phy;
    phy.SetChannel (channel.Create ());

    WifiHelper wifi;
    wifi.SetRemoteStationManager ("ns3::AarfWifiManager");

    WifiMacHelper mac;
    Ssid ssid = Ssid ("ns-3-ssid");
    mac.SetType ("ns3::StaWifiMac",
                 "Ssid", SsidValue (ssid),
                 "ActiveProbing", BooleanValue (false));

    NetDeviceContainer staDeviceClient;
    staDeviceClient = wifi.Install (phy, mac, wifiStaNodeClient);

    mac.SetType ("ns3::ApWifiMac", "Ssid", SsidValue (ssid));

    NetDeviceContainer staDeviceMaster;
    staDeviceMaster = wifi.Install (phy, mac, wifiStaNodeMaster);

    mac.SetType ("ns3::StaWifiMac","Ssid", SsidValue (ssid), "ActiveProbing", BooleanValue (false));
    
    NetDeviceContainer staDeviceMapper;
    staDeviceMapper = wifi.Install (phy, mac, wifiStaNodeMapper);

    Ptr<RateErrorModel> em = CreateObject<RateErrorModel> ();
    em->SetAttribute ("ErrorRate", DoubleValue (error));
    phy.SetErrorRateModel("ns3::YansErrorRateModel");

    MobilityHelper mobility;

    mobility.SetPositionAllocator ("ns3::GridPositionAllocator",
                                   "MinX", DoubleValue (0.0),
                                   "MinY", DoubleValue (0.0),
                                   "DeltaX", DoubleValue (3.0),
                                   "DeltaY", DoubleValue (5.0),
                                   "GridWidth", UintegerValue (6),
                                   "LayoutType", StringValue ("RowFirst"));

    mobility.SetMobilityModel ("ns3::RandomWalk2dMobilityModel",
                               "Bounds", RectangleValue (Rectangle (-50, 50, -50, 50)));
    mobility.Install (wifiStaNodeClient);

    mobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
    mobility.Install (wifiStaNodeMaster);
    
    mobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
    mobility.Install(wifiStaNodeMapper);

    InternetStackHelper stack;
    stack.Install (wifiStaNodeClient);
    stack.Install (wifiStaNodeMaster);
    stack.Install (wifiStaNodeMapper);

    Ipv4AddressHelper address;

    Ipv4InterfaceContainer staNodeClientInterface;
    Ipv4InterfaceContainer staNodesMasterInterface;
    Ipv4InterfaceContainer staNodesMapperInterface;
    

    address.SetBase ("10.1.3.0", "255.255.255.0");
    staNodeClientInterface = address.Assign (staDeviceClient);
    staNodesMasterInterface = address.Assign (staDeviceMaster);
    staNodesMapperInterface = address.Assign (staDeviceMapper);

    Ipv4GlobalRoutingHelper::PopulateRoutingTables ();

    uint16_t port = 1102;

    Ptr<client> clientApp = CreateObject<client> (port, staNodeClientInterface, port, staNodesMasterInterface);
    wifiStaNodeClient.Get (0)->AddApplication (clientApp);
    clientApp->SetStartTime (Seconds (0.0));
    clientApp->SetStopTime (Seconds (duration));  

    Ptr<master> masterApp = CreateObject<master> (port, staNodesMasterInterface, port, port, port,
                                                staNodesMapperInterface);
    wifiStaNodeMaster.Get (0)->AddApplication (masterApp);
    masterApp->SetStartTime (Seconds (0.0));
    masterApp->SetStopTime (Seconds (duration));  
    
    Ptr<mapper> mapperApp_1 = CreateObject<mapper>(port, staNodesMapperInterface, mapper1_mapping, 0);
    wifiStaNodeMapper.Get(0)->AddApplication (mapperApp_1);
    mapperApp_1->SetStartTime (Seconds (0.0));
    mapperApp_1->SetStopTime (Seconds (duration));

    Ptr<mapper> mapperApp_2 = CreateObject<mapper> (port, staNodesMapperInterface, mapper2_mapping, 1);
    wifiStaNodeMapper.Get(1)->AddApplication (mapperApp_2);
    mapperApp_2->SetStartTime (Seconds (0.0));
    mapperApp_2->SetStopTime (Seconds (duration));

    Ptr<mapper> mapperApp_3 = CreateObject<mapper> (port, staNodesMapperInterface, mapper3_mapping, 2);
    wifiStaNodeMapper.Get(2)->AddApplication (mapperApp_3);
    mapperApp_3->SetStartTime (Seconds (0.0));
    mapperApp_3->SetStopTime (Seconds (duration));

    NS_LOG_INFO ("Run Simulation");

    Ptr<FlowMonitor> flowMonitor;
    FlowMonitorHelper flowHelper;
    flowMonitor = flowHelper.InstallAll();

    ThroughputMonitor (&flowHelper, flowMonitor, error);
    AverageDelayMonitor (&flowHelper, flowMonitor, error);

    Simulator::Stop (Seconds (duration));
    Simulator::Run ();

    return 0;
}

client::client (uint16_t port, Ipv4InterfaceContainer& ip, uint16_t master_port, Ipv4InterfaceContainer& master_ip)
        : port (port),
          ip (ip),
          master_port (master_port),
          master_ip (master_ip)
{
    std::srand (time(0));
}

client::~client ()
{
    cout << result << endl;
}

static void GenerateTraffic (Ptr<Socket> socket, Ipv4InterfaceContainer ip, uint16_t port, uint16_t data, std::vector<uint16_t> input, uint16_t indx)
{
    Ptr<Packet> packet = new Packet();
    MyHeader m;
    m.SetData(data);
    m.SetIp(ip.GetAddress(0));
    m.SetPort(port);

    packet->AddHeader (m);
    packet->Print (std::cout);
    socket->Send (packet);
    
    // Generate traffic with the current value in the vector
    uint16_t value = input[indx];
    // Increment the index for the next iteration
    indx = (indx + 1) % input.size();

    Simulator::Schedule (Seconds (0.1), &GenerateTraffic, socket, ip, port, value, input, indx);
}

void
client::StartApplication (void)
{
    //Create socket for UDP between client and master
    Ptr<Socket> sock = Socket::CreateSocket(GetNode(), UdpSocketFactory::GetTypeId());
    InetSocketAddress sockAddr (master_ip.GetAddress(0), master_port);
    sock->Connect (sockAddr);
    
    //Create socket for UDP between client and mapper
    Ptr<Socket> sock2 = Socket::CreateSocket (GetNode (), UdpSocketFactory::GetTypeId ());
    InetSocketAddress sockAddr2(ip.GetAddress(0), port);
    sock2->Bind (sockAddr2);
    sock2->SetRecvCallback (MakeCallback (&client::HandleRead, this));

    GenerateTraffic(sock, ip, port, 0, input, indx);
}

void
client::HandleRead(Ptr<Socket> socket)
{
    Ptr<Packet> packet;
    while ((packet = socket->Recv ()))
    {
        if (packet->GetSize() == 0) {
            break;
        }
        MyHeader m;
        packet->RemoveHeader (m);
        result += static_cast<char>(m.GetData());
    }
}

master::master (uint16_t port, Ipv4InterfaceContainer& ip ,
                uint16_t mapper_1_port, uint16_t mapper_2_port, uint16_t mapper_3_port,
                Ipv4InterfaceContainer& mapper_ip)
        : port (port),
          ip (ip),
          mapper_1_port(mapper_1_port),
          mapper_2_port(mapper_2_port),
          mapper_3_port(mapper_3_port),
          mapper_ip(mapper_ip)
{
    std::srand (time(0));
}

master::~master ()
{
}

void
master::StartApplication (void)
{
    socket = Socket::CreateSocket (GetNode (), UdpSocketFactory::GetTypeId ());
    InetSocketAddress local = InetSocketAddress (ip.GetAddress(0), port);
    socket->Bind (local);
    
    mapper_1_socket = Socket::CreateSocket(GetNode(), TcpSocketFactory::GetTypeId());
    InetSocketAddress remote_1 = InetSocketAddress (mapper_ip.GetAddress(0), mapper_1_port);
    mapper_1_socket->Connect (remote_1);
    mapper_2_socket = Socket::CreateSocket(GetNode(), TcpSocketFactory::GetTypeId());
    InetSocketAddress remote_2 = InetSocketAddress (mapper_ip.GetAddress(1), mapper_2_port);
    mapper_2_socket->Connect (remote_2);
    mapper_3_socket = Socket::CreateSocket(GetNode(), TcpSocketFactory::GetTypeId());
    InetSocketAddress remote_3 = InetSocketAddress (mapper_ip.GetAddress(2), mapper_3_port);
    mapper_3_socket->Connect (remote_3);

    socket->SetRecvCallback (MakeCallback (&master::HandleRead, this));
}

void
master::HandleRead (Ptr<Socket> socket)
{
    Ptr<Packet> packet;

    while ((packet = socket->Recv ()))
    {
        if (packet->GetSize() == 0)
        {
            break;
        }
        MyHeader header;
        packet->RemoveHeader (header);
        Ptr<Packet> packet2 = new Packet();
        Ptr<Packet> packet3 = new Packet();
        Ptr<Packet> packet4 = new Packet();
        packet2->AddHeader (header);
        packet3->AddHeader (header);
        packet4->AddHeader (header);

        mapper_1_socket -> Send(packet2);
        mapper_2_socket->Send(packet3);
        mapper_3_socket -> Send(packet4);
    }
}

mapper::mapper (uint16_t port, Ipv4InterfaceContainer& ip, std::map<int, char> map, int i)
        : port (port),
          ip (ip),
          map_set (map),
          i(i)

{
    std::srand (time(0));
}

mapper::~mapper ()
{
}

void
mapper::StartApplication (void)
{
    Ptr<Socket> socket = Socket::CreateSocket (GetNode (), TcpSocketFactory::GetTypeId());
    InetSocketAddress local = InetSocketAddress (ip.GetAddress(i), port);
    socket->Bind (local);

    // Listen for incoming connections
    socket->Listen ();
    socket->SetAcceptCallback(MakeNullCallback<bool, Ptr<Socket>, const Address &>(), MakeCallback(&mapper::HandleAccept, this));
    
    client_socket = Socket::CreateSocket (GetNode (), UdpSocketFactory::GetTypeId ());
}

void
mapper::HandleRead (Ptr<Socket> socket)
{
    Ptr<Packet> packet;

    while((packet = socket->Recv()))
    {
        if (packet->GetSize() == 0)
            break;

        MyHeader header;
        packet->RemoveHeader(header);
        
        //we should find the data in the header
        uint16_t data = header.GetData();
        Ipv4Address ip = header.GetIp();
        uint16_t port = header.GetPort();
        for (const auto &kv : map_set)
        {
            if(kv.first == data)
            {
                MyHeader header1;
                char new_data = kv.second;
                header1.SetData(new_data);
                Ptr<Packet> packet1 = Create<Packet> (header1.GetSerializedSize());
                packet1->AddHeader(header1);
                InetSocketAddress remote = InetSocketAddress (ip, port);
                client_socket->SendTo(packet1, 0, remote);
                break;
            }
        }
        
    }
}

void
mapper::HandleAccept (Ptr<Socket> socket, const Address& from)
{
    socket->SetRecvCallback (MakeCallback (&mapper::HandleRead, this));
}
