TUIO 2.0 C++ Library
====================

TUIO 2.0 is an open framework that defines a common protocol and API for tangible interactive 
surfaces. The TUIO protocol allows the encoding and transmission of an abstract description of 
interactive surfaces and tangible objects, including their geometries and relations as well as 
multi-pointer input.

http://www.tuio.org/?tuio20


Current Implementation
----------------------
- implements the full TUIO 2.0 specification, including all components, 3D profiles,
  geometry descriptors, content messages and associations
- provides Server and Client implementations for TUIO 2.0 encoding/decoding
- optional TCP and Websocket transport layers in addition to the default UDP transport
- supports the multiplexing of multiple TUIO2 sources
- local UDP packets up to 65KB with transparent frame splitting and reassembly

Shared Transport and Model Code
-------------------------------
The transport layer (OSC packet handling, UDP/TCP/WebSocket senders and receivers), TuioTime, TuioPoint and OneEuroFilter are shared with the TUIO 1.1 C++ library through the TUIO/ directory and the common TUIO namespace. The TUIO2 namespace provides the 2.0-specific component and object model on top of that shared infrastructure.

Building and Testing
--------------------
- `make` in the `demos/` directory builds the library, the Tuio2Demo and Tuio2Simulator
  GUI applications and the Tuio2Dump console tools
- `make test` builds and runs the unit test suite in `tests/` (network loopback, wire-protocol decode, malformed-packet tolerance, split-frame reassembly and model API coverage)

Example Projects
----------------
Apart from the actual cross-platform TUIO 2.0 implementation, this repository also provides several example projects that serve for the illustration and testing of the overall API functionality. This includes TUIO 2.0 client and server demonstrations with a simple SDL2 GUI as well as some simple console test applications.
These examples should equally work on Linux, Mac OS X and Windows operating systems, the according Makefile as well as XCode and Visual Studio project files are included. The Windows project already include the necessary libraries, to build the GUI example on Linux make sure you have the SDL2, OpenGL and GLUT libraries and headers installed on your system. On Mac OS X you need to install the SDL2 Framework to /Library/Frameworks.

Integration and Testing
----------------------
Developers are encouraged to integrate this TUIO 2.0 library into their projects, in order to support the development and testing of this common shared code base. The library is licensed under the LGPL in order to facilitate this collaboration.


Feedback and Consultation
--------------------------
Please use the provided Github infrastructure for the discussion related to the design and implementation of this library or the TUIO 2.0 specification in general. Any suggestions for improvements of the API design and code quality are most welcome, and should be submitted and discussed through pull requests. 

For any other inquiries please get in touch with Martin Kaltebrunner via martin[at]tuio.org,
although you may need to insist to receive a reply on this noisy communication channel.

