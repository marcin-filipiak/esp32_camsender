# esp32_camsender

<p align="center">
  <img src="https://raw.githubusercontent.com/marcin-filipiak/esp32_camsender/main/doc/esp32.jpg" width="300px">
</p>

Software for ESP32-CAM that enables camera configuration via a Bluetooth terminal. The camera transmits photos to a web server using the POST method, and the data is received by a PHP script.

<p align="center">
  <img src="https://raw.githubusercontent.com/marcin-filipiak/esp32_camsender/main/doc/data_flow.jpg">
</p>

## CAMERA 

You need own server to store images.
If the server is running on a local network, you can use it.
The remaining configuration, such as the Wi-Fi network name and password, can be done by connecting to the camera via Bluetooth. 
I recommend using the: https://github.com/marcin-filipiak/IoT_Configurator application on Android.
There you can download apk file and install on your phone. Also you should find this app in GooglePlay. 

## SERVER

If you decide to install your own server for storing images from cameras, Apache with PHP support will be required. On the server, create a folder named 'camsender' and copy the files from the PHP_server folder into it. For example, the address will look like: 
`http://api.filipiak.tech/camsender/index.php`
On the server, the images will be stored in the 'data' folder. Remember to set write permissions for this folder. 
You can test the server's functionality by opening the test.html page.

The API is very simple. To preview the latest image from a selected camera, enter the address in the format `http://api.filipiak.tech/camsender/index.php?image/foobar`, where 'foobar' should be replaced with the camera identifier you assigned during configuration. 

You can also view live stream in mjpeg. Enter the address in the format `http://api.filipiak.tech/camsender/index.php?mjpeg/foobar`, where 'foobar' should be replaced with the camera identifier you assigned during configuration. 

Of course, adjust the address accordingly to match your server's address.

## CONFIGURATION

This project using <a href="https://github.com/marcin-filipiak/DeviceConfigJSON" target="_blank">DeviceConfigJSON</a> library,
you need download this from Arduino Library before compile.
It let you use <a href="https://github.com/marcin-filipiak/IoT_Configurator">IoT Configurator</a> (Android app) to config this device.


<a href="https://play.google.com/store/apps/details?id=com.iot_config&pcampaignid=web_share" target="_blank"><img src="https://upload.wikimedia.org/wikipedia/commons/7/78/Google_Play_Store_badge_EN.svg"></a>
