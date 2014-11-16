TUIO 2.0 C++ Library
====================

TUIO 2.0 is an open framework that defines a common protocol and API for tangible interactive 
surfaces. The TUIO protocol allows the encoding and transmission of an abstract description of 
interactive surfaces, including tangible object relations, geometries and multitouch input.

http://www.tuio.org/?tuio20/


Current Implementation
----------------------

- encapsulates the TUIO 2.0 Token, Pointer, Symbol and Bounds components 
- provides the TUIO 2.0 Server and Client implementations for sending/receiving
- optional TCP and Flash Socket transport layers in addition to the default UDP transport

Alpha Code and API
------------------

This library currently only implements a subset of the TUIO 2.0 specification, 
the remaining object geometries and relations will be added subsequently.
The current code also defines a general API for the encapsulation of TUIO components.
This API is already quite usable, but may contain some partially or badly implemented sections,
which are obviously subject to change and improvements.

Integration and Testing
----------------------

Developers are encouraged to integrate this TUIO 2.0 library into their projects,
in order to support the development and testing of this common shared code base.
The library is licensed under the LGPL in order to facilitate this collaboration.


Feedback and Consulatation
--------------------------
Please use the provided Github infrastructure for the discussion related to the design 
and implementation of this library or the TUIO 2.0 specification in general. 
Any suggestions for improvements of the API design and code quality are most welcome, 
and should be submitted and discussed through pull requests. 

For any other inquiries please get in touch with Martin Kaltebrunner via <martin[at]tuio.org>,
although you may need to insist to receive a reply on this noisy communication channel.

