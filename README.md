# LAP-DSR: Link Availability Prediction Dynamic Source Routing

[![OMNeT++](https://img.shields.io/badge/OMNeT%2B%2B-5.x%20%7C%206.x-blue.svg)](https://omnetpp.org/)
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
[![Language](https://img.shields.io/badge/Language-C%2B%2B11-green.svg)](https://isocpp.org/)

An advanced **OMNeT++** implementation of the Dynamic Source Routing (DSR) protocol enhanced with **Link Availability Prediction (LAP)**, dynamic selfish node isolation, and comprehensive metric collection for Mobile Ad-Hoc Networks (MANETs).

---

## 📌 Features

* **Link Availability Prediction (LAP):** Evaluates link stability using node mobility vectors (speed, direction) and signal strength attenuation to prevent route failures before they happen.
* **Dynamic Selfish Node Isolation:** Detects malicious or uncooperative behavior. Nodes dropping packets above a set threshold (`dropCount > 3`) are flagged, isolated, and highlighted visually in the GUI.
* **Complete DSR Control Cycle:** Implements standard route discovery and maintenance mechanisms including `RREQ`, `RREP`, `RERR`, and `HELLO` beaconing with local route caching.
* **Integrated 2D Mobility:** Built-in spatial mobility handling customizable speed ranges, variable pause times, and dynamic boundary constraints.
* **Automated CSV Metrics Export:** Appends detailed per-node operational metrics and global simulation summary headers directly to `lap_dsr_results.csv` upon run completion.

---

## 📁 Repository Structure

```text
.
├── DSRRouting.h        # Module declarations, structures, and metric state tracking
├── DSRRouting.cc       # Core routing, prediction logic, mobility, and CSV export
├── omnetpp.ini         # Network simulation parameters and run configurations
└── README.md           # Project documentation
```

## ⚙️ Prerequisites & Dependencies

To build and run this simulation, ensure you have:

- **OMNeT++** (Version 6.x)
- **C++11** compliant compiler (GCC 7+, Clang, or MSVC)
- Standard C++ Libraries (`<algorithm>`, `<cmath>`, `<fstream>`, `<iomanip>`, `<sstream>`)

## 🚀 Quick Start

### 1. Build the Project

Import the project into your OMNeT++ IDE or build via terminal:

```bash
# Generate Makefile
opp_makemake -f --deep

# Compile the project
make
```

### 2. Run the Simulation

Execute the compiled binary against your configuration file:

```bash
# GUI Mode
./lap_dsr -u Qtenv -c General

# Command-Line / Headless Mode
./lap_dsr -u Cmdenv -c General
```

## ⚙️ Configuration (`omnetpp.ini`)

Key operational parameters can be customized in your `omnetpp.ini` file:

```ini
[General]
network = LAPDSRNetwork
sim-time-limit = 200s

*.numHosts = 50
*.transmissionRange = 250m

# LAP-DSR Specific Configurations
*.host[*].routing.useLinkAvailabilityPrediction = true
*.host[*].routing.linkAvailabilityThreshold = 0.70
*.host[*].routing.routeCacheTimeout = 10s
*.host[*].routing.routeDiscoveryTimeout = 2s
*.host[*].routing.helloInterval = 1.0s

# Mobility Parameters
*.host[*].routing.minSpeed = 1.0
*.host[*].routing.maxSpeed = 10.0
*.host[*].routing.pauseTime = 2.0
*.host[*].routing.constraintAreaMaxX = 500m
*.host[*].routing.constraintAreaMaxY = 500m
```

## 📊 Collected Metrics

The project automatically gathers and calculates the following network performance indicators:

| **Metric** | **Unit** | **Description** |
|---|---|---|
| **Packet Loss Rate** | `%` | Percentage of dropped packets relative to total attempted transmissions. |
| **Throughput** | `kbps` | Effective payload data successfully received over total simulation runtime. |
| **End-to-End Delay** | `seconds` | Average latency experienced by packets from creation to destination delivery. |
| **Routing Overhead** | `ratio` | Ratio of control packets (`RREQ`, `RREP`, `RERR`) generated per delivered data packet. |
| **Selfish Drops** | `count` | Total data/control packets intentionally dropped by uncooperative nodes. |

## 📄 Exported Output Format (`lap_dsr_results.csv`)

At the conclusion of the simulation, results are exported to `lap_dsr_results.csv` with structured headers and run summaries:

```text
# ============================================================================
# LAP-DSR SIMULATION EXECUTION METRICS REPORT
# Protocol: Link Availability Prediction Dynamic Source Routing (LAP-DSR)
# ============================================================================
Node_ID,Node_Role,Packets_Sent,Packets_Delivered,Packets_Forwarded,Packets_Dropped,Selfish_Drops,Packet_Loss_Rate_Percent,Throughput_Kbps,Avg_EndToEnd_Delay_Sec,Routing_Overhead_Ratio
0,Normal,18,15,6,1,0,4.00,48.20,0.0124,0.28
5,Selfish/Isolated,0,0,0,5,5,100.00,0.00,0.0000,0.00
...

# --- SIMULATION CONFIGURATION SUMMARY ---
Metric_Label,Metric_Value
Total_Nodes,50
Transmission_Range_Meters,250
Link_Availability_Threshold,0.7
Route_Cache_Timeout_Sec,10
# --- END OF SUMMARY ---
```

## 📜 License

This project is licensed under the MIT License - see the `LICENSE` file for details.
