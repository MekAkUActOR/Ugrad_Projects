# Data Communications

### CREDIT 2 *|* SCORE 89 | 3rd year, 2nd semester

**Environment:**

- *macOS Catalina 10.15.4*
  - *Parallels Desktop 15 for Mac Pro Edition, version 15.1.4 (47270)*
  - *Windows 10* *Pro* *x64*
  - *Visual Studio 2019*

- Ubuntu 18.04

***

This folder contains some of my course projects.

- Echo Server-Client program(C++, winsock)

- GetHostInfo(C++, winsock)

- TCP chatting Server-Client program(C++, winsock)

- UDP chatting Server-Client program(C++, winsock)

- PING program(C++, winsock)

- web Server program(C++, winsock)

- ns3 network simulating(C, ns3)

  - lab2

     *      	192.168.10.0                       192.168.50.0
     n0 ------------------------ n1-------------------------------- n2
             		point-to-point               point-to-point
              		5Mbps, 2ms                    1Mbps, 2ms 

  - lab3

    - Wifi 10.1.3.0

      \*  \*  \*

      |  |  |
      n3 n4 n5    p2p (100Mbps, 2ms)          p2p(100Mbps, 2ms)
          A (n0)------------------------B(n1)---------------------------C(n2)
         (AP)            10.1.1.0                    1 0.1.2.0          (AP)
                                     	         		                    n6  n7  n8
                                       	             		              |   |   |

      																		\*   \*   \*  

                                             	      	 		           Wifi 10.1.4.0

  - lab4

    - //   Wifi 10.1.3.0
      //            	   AP
      //  *    *    *    *
      //  |    |    |    |    	10.1.1.0
      // n5   n6   n7   n0 -------------- n1   n2   n3   n4
      //                	   point-to-point  |    |    |    |
      //                                 		  \==\==\=\==\=\=\=\=\=====
      //                                     			LAN 10.1.2.0

  - lab5&lab6
