CTrackerClient
==========

What?
-----
A library for managing connections with BitTorrent trackers

How does it work?
-----------------
The library doesn't do any networking out-of-the-box. For an example of CTrackerClient in action, please see: http://github.com/willemt/YABTorrent

Dependencies
------------
- http://github.com/joyent/http-parser for HTTP trackers

Features
--------
- BEP 3 - The BitTorrent Protocol Specification
- BEP 23 - Tracker Returns Compact Peer Lists

TODO
----
- BEP 5 - DHT Protocol
- BEP 7 - IPv6 Tracker Extension
- BEP 12 - Multitracker Metadata Extension
- BEP 15 - UDP Tracker Protocol
- BEP 22 - BitTorrent Local Tracker Discovery Protocol
- BEP 24 - Tracker Returns External IP
- BEP 31 - Tracker Failure Retry Extension
- BEP 33 - DHT scrape
- BEP 41 - UDP Tracker Protocol Extensions

