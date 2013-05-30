prismatoid (development in progress)
==========

Lightweight cross-platform daemon to control [Lightpack](https://code.google.com/p/lightpack/).

The daemon listens 7777 port and accepts the following commands:

* `SetColors <color_list>`
* `SetSmoothness <int_value>`

Examples:

`SetColors [5](10,10,10)`  
same as  
`SetColors (10,10,10),(10,10,10),(10,10,10),(10,10,10),(10,10,10)`  
and same as  
`SetColors (10,10,10),(10,10,10),(10,10,10),(10,10,10),(10,10,10),(0,0,0),(0,0,0),(0,0,0),(0,0,0),(0,0,0)`

it will set first 5 LEDs to `#0A0A0A` the rest of LEDs will set to black


`SetSmoothness 100`  
set smoothness to 100
