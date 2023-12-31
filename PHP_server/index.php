<?php

//ini_set('display_errors', 1);
//ini_set('error_reporting', E_ALL);

// Kontroler
class EspCamController {
    public function send($id_camera) {
   
            // Zapisanie obrazka na dysku
            $fileName = $id_camera . '.jpg';
            $filePath = 'data/' . $fileName;

            if (move_uploaded_file($_FILES["imageFile"]["tmp_name"], $filePath)){
		echo "ok";
 	    }
	    else {
		echo "err";
	    }
    }

    public function mjpeg($id_camera) {
	    $filePath = 'data/' . $id_camera . '.jpg';
	   
	    if (!file_exists($filePath)){
		echo 'Stream not found!';
		
	    }
	    else {
		    header('Cache-Control: no-cache, no-store, must-revalidate');
		    header('Pragma: no-cache');
		    header('Expires: 0');
		    header('Content-Type: multipart/x-mixed-replace; boundary=--myboundary');
		
		    //stream klatek
		    while (true) {
		        //jesli plik istnieje
			if (file_exists($filePath)) {
				    ob_start();
				    $imageData = file_get_contents($filePath);
				    echo "--myboundary\r\n";
				    echo "Content-Type: image/jpeg\r\n";
				    echo "Content-Length: " . strlen($imageData) . "\r\n\r\n";
				    echo $imageData . "\r\n";	
				    ob_end_flush();
				    flush();    
			    }
		        sleep(3); 
		    }
		}
    }
    
    public function image($id_camera) {
        // Sprawdzenie, czy żądanie jest typu GET
        if ($_SERVER['REQUEST_METHOD'] === 'GET') {
            // Ścieżka do zapisanego obrazka
            $filePath = 'data/' . $id_camera . '.jpg';
            
            // Sprawdzenie, czy plik istnieje
            if (file_exists($filePath)) {
		header('Cache-Control: no-cache, no-store, must-revalidate'); // HTTP 1.1.
		header('Pragma: no-cache'); // HTTP 1.0.
		header('Expires: 0'); // Proxies.

                // Ustawienie nagłówka Content-Type dla obrazka
                header('Content-Type: image/jpeg');
                
                // Wyświetlenie zawartości obrazka
                readfile($filePath);
            } else {
                // Jeśli plik nie istnieje
                echo 'Image not found!';
            }
        } else {
            // Jeśli żądanie nie jest typu GET
            echo 'Invalid request method!';
        }
    }
}

// Routing
$requestUri = $_SERVER['REQUEST_URI'];
$route = parse_url($requestUri, PHP_URL_QUERY);
$routeParts = explode('/', $route);

if (isset($routeParts[0])) {
	$controller = new EspCamController();
        $method = $routeParts[0];
        
        if ($method === 'send') {
	    $id_camera = $routeParts[1];
            $controller->send($id_camera);
        } 
	if ($method === 'image') {
	    $id_camera = $routeParts[1];
            $controller->image($id_camera);
        } 
	if ($method === 'mjpeg') {
            $id_camera = $routeParts[1];
	    $controller->mjpeg($id_camera);
	}
	
} 
else {
   echo 'Invalid!';
}

?>
