# esp32_camsender
Software for ESP32-CAM that allows camera configuration via Bluetooth terminal. The camera sends photos to a web server using the POST method, and the data is received by a PHP script.

<p align="center">
  <img src="https://raw.githubusercontent.com/marcin-filipiak/esp32_camsender/main/doc/data_flow.jpg">
</p>

## CAMERA. 

If you are using your own server to store images, before compiling the code, change lines:

```
//String serverName = "192.168.1.XXX";   // REPLACE WITH SERVER API IP ADDRESS
String serverName = "api.filipiak.tech";   // OR REPLACE WITH YOUR DOMAIN NAME SERVER API
String serverPath = "/camsender/index.php?send/";  //REPLACE WITH YOUR PATH, "/" before and at the end is important
```

to the address of your server. If the server is running on a local network, you can use its fixed IP address in that network. Once you compile this code, you can load it onto all your cameras. The remaining configuration, such as the Wi-Fi network name and password, can be done by connecting to the camera via Bluetooth. I recommend using the 'Serial Bluetooth Terminal' application on Android. Use "LF" for a new line in Serial Bluetooth Terminal - look at "settings". After pairing the camera, connect to it. Sending the character 'h' will display the menu. The operation is simple and intuitive — use it like a terminal.

## SERVER. 

If you decide to install your own server for storing images from cameras, Apache with PHP support will be required. On the server, create a folder named 'camsender' and copy the files from the PHP_server folder into it. For example, the address will look like: 
http://api.filipiak.tech/camsender/index.php
On the server, the images will be stored in the 'data' folder. Remember to set write permissions for this folder. 
You can test the server's functionality by opening the test.html page.

The API is very simple. To preview the latest image from a selected camera, enter the address in the format http://api.filipiak.tech/camsender/index.php?image/foobar, where 'foobar' should be replaced with the camera identifier you assigned during configuration. 
Of course, adjust the address accordingly to match your server's address.
