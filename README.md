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
