# CN_Chomeworks_2
**Maryam Jafarabadi Ashtiani : 810199549**  
**Sana Sari Navaei : 810199435**

# Project Description  
In this project, we aim to simulate a 4G wireless network using the 3-ns simulation tool and analyze its various parameters. This network implements a simple encryption system.  

The network consists of 3 nodes:
- Client node
- Master node
- Mapper node  

client wants to send a message to the master node usign UDP protocol. The client node encrypts the message using a simple encryption algorithm and sends it to the master node. The master node gets the message and sends it to the mapper nodes using TCP protocol. The mapper nodes decrypt the message and send it to the client node using UDP protocol. At this program, we want to learn how to simulate a network with different encryption algorithms and analyze the network's performance.  

Here's the picture of the network we want to simulate and analyze:  
![network](pictures/network.jpg)

# Code Description  
The code is written in C++. We are going to explain the code step by step.

## 1. Including the necessary libraries
At the beginning of the code, we include the necessary libraries. Most of the libraries are included in the ns3 library. There are other libraries such as `map` and `vector` that are used to store the data in the code.  
```c++
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
```

## 2. Defining the necessary variables
In this part, at first we use the `using namespace` command to use the ns3 and std libraries. Then we define `mapping variables` for the encryption algorithm. The encryption algorithm is a simple algorithm that maps each number to a character. There are 3 mappers in the network. Each mapper has its own mapping.  

```c++
using namespace ns3;
using namespace std;
std::map<int, char> mapper1_mapping = {{0, 'a'},{1, 'b'},{2, 'c'},{3, 'd'},{4, 'e'},{5, 'f'},{6, 'g'},{7, 'h'},{8, 'i'}};
std::map<int, char> mapper2_mapping = {{9, 'j'},{10, 'k'},{11, 'l'},{12, 'm'},{13, 'n'},{14, 'o'},{15, 'p'},{16, 'q'},{17, 'r'}};
std::map<int, char> mapper3_mapping = {{18, 's'},{19, 't'},{20, 'u'},{21, 'v'},{22, 'w'},{23, 'x'},{24, 'y'},{25, 'z'},{26, ' '}};
```
## 3. Header Class
Header class is used to define the header of the packets. The header of the packets contains the data that is sent to the master node. The header of the packets also contains the IP address and the port number of the client node. The header class is defined as follows:  

```c++
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
```
#### **Now we are going to explain the functions of the header class.**  
Functin `GetTypeId` is used to return a unique identifier (or TypeId) for the MyHeader class. This function is a common pattern used in ns-3 to get the unique TypeId for a class, which is useful for various purposes such as creating objects dynamically, registering callbacks, and more.  

Function `GetInstanceTypeId` is used to return the TypeId of the instance of the MyHeader class. This function is a simple and useful way to get the TypeId of a specific instance of the MyHeader class.  

Function `Print` is used to print the data of the header.  

```c++
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
```

Function `GetSerializedSize` is used to return the size of the header which is 8 bytes.  

The `Serialize` function takes a **Buffer::Iterator** as an argument and serializes the contents of the MyHeader object to the buffer starting from the given iterator position. The function first converts the **m_data** member variable to network byte order using the **WriteHtonU16** function, then writes the **m_ip** member variable using the **WriteHtonU32** function, and finally writes the **m_port** member variable using the **WriteHtonU16** function.  

The `Deserialize` function takes a **Buffer::Iterator** as an argument and deserializes the contents of the MyHeader object from the buffer starting from the given iterator position. The function first reads the **m_data** member variable using the **ReadNtohU16** function, then reads the **m_ip** member variable using the **ReadNtohU32** function, and finally reads the **m_port** member variable using the **ReadNtohU16** function. The function returns the size of the deserialized data, which is again a fixed value of 8 bytes.  

```c++
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
```

Function `SetData` takes a **uint16_t** value as an argument and sets the **m_data** member variable to that value.  

Function `GetData` returns the value of the **m_data** member variable.  

Function `SetIp` takes an **Ipv4Address** value as an argument and sets the **m_ip** member variable to that value.  

Function `SetPort` takes a **uint16_t** value as an argument and sets the **m_port** member variable to that value.  

Function `GetIp` returns the value of the **m_ip** member variable.  

Function `GetPort` returns the value of the **m_port** member variable.  

```c++
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
```
## 4. Client Class

Client class is used to define the client node. The client node sends the data to the master node. It also receives the result from the master node. The **std::vector<uint16_t> input** stores data that will be sent to the master node. We choose the word `computer-network` as the data to be sent. The client class is defined as follows:  

```c++
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
```

#### **Now we are going to explain the functions of the client class.**  

Functin `StartApplication` is called when the application starts. This function creates two UDP sockets, one for communication between the client and the master and another for communication between the client and a mapper.  

The first socket is created using **Socket::CreateSocket** method, which returns a pointer to a new socket object. The type of socket being created is specified using **UdpSocketFactory::GetTypeId()**. The socket is then connected to the InetSocketAddress object sockAddr, which contains the master IP address and port number.

The second socket is created using **Socket::CreateSocket** method, which returns a pointer to a new socket object. The type of socket being created is specified using **UdpSocketFactory::GetTypeId()**. The socket is then bound to the InetSocketAddress object sockAddr2, which contains the client IP address and port number. The socket is then set to receive data using the **SetRecvCallback** method.  

Finally, the **GenerateTraffic** function is called with parameters for the first socket, the local IP address, the local port number, an offset value of 0, the input vector, and the index. This function likely generates traffic to be sent over the first socket to the server, using the input vector as the data payload. The index is used to keep track of the current position in the input vector.

```c++
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
```

Because the `GenerateTraffic` function is used in this section, we will explain it here.  
This function can be used to generate network traffic with custom data payloads and intervals between packets. Inside the function, a new Packet object is created and a header is added to it. The header is initialized with the **data**, **ip**, and **port** values passed into the function.  
After sending the packet, the function schedules itself to be called again in 0.1 seconds using the **Simulator::Schedule** method. The next value in the **input** vector is retrieved using the **indx** variable as the index. The **indx** variable is then incremented, and if it exceeds the size of the **input** vector, it is set back to 0.

```c++
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
```
Function `HandleRead` is called when the client receives data from the mapper node.   
Inside the function, a new Packet object is created and set to the value returned by the **Recv** method of the Socket object. This method blocks until data is received from the socket.  

A while loop is then used to process all the packets received from the socket. The loop continues until the **Recv** method returns a null pointer, indicating that there are no more packets to process.  

If the packet size is zero, the loop is broken as it indicates that the connection is closed. Otherwise, a custom header called **MyHeader** is added to the packet, and the header is removed from the packet using the **RemoveHeader** method.  

```c++
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
```
## 5. Master Class

The master class is used to receive data from the client and send it to the mappers.  

```c++
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
```
#### **Now we are going to explain the functions of the master class.**

Function `StartApplication` is called when the application starts. This function is used to initialize and configure the sockets required by the master node to communicate with the mapper nodes. The function creates and binds a **UDP socket** for receiving data from the client node and creates three **TCP sockets** for sending data to the mapper nodes. Finally, it sets the receive callback function for the UDP socket to the **HandleRead** function.

```c++
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
```
Function `HandleRead` is called when the master node receives data from the client node. Inside the function, a new Packet object is created and set to the value returned by the **Recv** method of the Socket object. This method blocks until data is received from the socket. This function is used to forward incoming packets from the UDP socket to the mapper nodes using TCP connections. It creates three new packets for each incoming packet and sends them to the mapper nodes with the same header as the original packet.
```c++
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
```

## 6. Mapper Class

The `mapper` class is an application that runs on each of the mapper nodes in a distributed system. It receives packets from the master node over a **TCP** connection, processes them using a map_set, and sends the results back to the client node over a **UDP** connection.  
```c++
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
    Ipv4InterfaceContainer ip;
    std::map<int, char> map_set;
    int i;
};

```

#### **Now we are going to explain the functions of the mapper class.**

In the mapper class, the `StartApplication` function sets up the application by creating a TCP socket and binding it to a local address, and then sets up a callback function to handle incoming connections.

Specifically, it does the following:

- Creates a TCP socket using the TcpSocketFactory.
- Binds the socket to a local address using the **Bind()** function and the **Ipv4InterfaceContainer** passed in as a parameter.
- Sets up a callback function to handle incoming connections using the **SetAcceptCallback()** function and the **HandleAccept()** function as the callback.  

```c++
void
mapper::StartApplication (void)
{
    Ptr<Socket> socket = Socket::CreateSocket (GetNode (), TcpSocketFactory::GetTypeId());
    InetSocketAddress local = InetSocketAddress (ip.GetAddress(i), port);
    socket->Bind (local);

    // Listen for incoming connections
    socket->Listen ();
    socket->SetAcceptCallback(MakeNullCallback<bool, Ptr<Socket>, const Address &>(), MakeCallback(&mapper::HandleAccept, this));
}
```
The `HandleRead` function iterates over the **map_set**, which is a map containing a set of key-value pairs that map a specific data value to a character. For each key-value pair, it checks if the data value from the packet matches the key in the map. If there is a match, it creates a new MyHeader with the corresponding character as the data value and creates a new Packet with this header. It then creates a new **UdpSocket** and connects to the ip and port from the original packet's header. Finally, it sends the new packet through the socket and closes it.  
If no match is found in the map_set, the loop continues to the next packet.  

```c++
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
                Ptr<Socket> socket = Socket::CreateSocket (GetNode (), UdpSocketFactory::GetTypeId ());
                InetSocketAddress remote = InetSocketAddress (ip, port);
                socket->Connect (remote);
                socket->Send(packet1);
                socket->Close();
                break;
            }
        }
        
    }
}
```

In `HandleAccept` function, we set the receive callback for the socket to the HandleRead function using the **MakeCallback** method. This means that whenever the socket receives a packet, it will call the HandleRead function to process it.  

```c++
void
mapper::HandleAccept (Ptr<Socket> socket, const Address& from)
{
    socket->SetRecvCallback (MakeCallback (&mapper::HandleRead, this));
}
```




