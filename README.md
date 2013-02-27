A NetworkManager front-end based on NetworkManager dbus interface
=================================================================

Features 
========
develop branch
--------------
* with kernel pppoe
* with wireless connection
* based on rp-pppoe, net-tools and so on

networkmanager branch 
---------------------
* simulate win8 system management interface
* use webkit as the interface, the backend is qt, the frontend is 
based on html+css+javascript
* this design is based on the vision that future system UI is based 
on internel, all things is from the internet;few more years, 
filemanager, IDE and so on could based on web interface.but, of course
there are dozens of problem need to do.more years is necessary.as if 
this is more like chrome os.

setps for networkmanager branch 
---------------------------------
* notification class
* design the interface with backend and UI.[Important]
* UI.it's time custom. 
* wired connection, a lot to do with authentication.
* pppoe connection
* wireless connection, there are question about WPA and WEP authentication
* maybe use rp-pppoe as the pppoe connection backend
