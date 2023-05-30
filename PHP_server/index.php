/ index.php

// Kontroler
class EspCamController {
    public function send($id_camera) {
        // Sprawdzenie, czy żądanie jest typu POST
        if ($_SERVER['REQUEST_METHOD'] === 'POST') {
            // Pobranie zawartości obrazka z żądania POST
            $imageData = file_get_contents('php://input');
            
            // Zapisanie obrazka na dysku
            $fileName = $id_camera . '.jpg';
            $filePath = 'data/' . $fileName;
            file_put_contents($filePath, $imageData);
            
            // Zwrócenie odpowiedzi sukcesu
            echo 'Image saved successfully!';
        } else {
            // Jeśli żądanie nie jest typu POST
            echo 'Invalid request method!';
        }
    }
    
    public function image($id_camera) {
        // Sprawdzenie, czy żądanie jest typu GET
        if ($_SERVER['REQUEST_METHOD'] === 'GET') {
            // Ścieżka do zapisanego obrazka
            $filePath = 'path/to/save/images/' . $id_camera . '.jpg';
            
            // Sprawdzenie, czy plik istnieje
            if (file_exists($filePath)) {
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
$baseUrl = '/index.php';
$route = str_replace($baseUrl, '', $requestUri);
$routeParts = explode('/', $route);

// Sprawdzenie poprawności adresu URL
if ($routeParts[0] === 'EspCam') {
    $controller = new EspCamController();
    
    if (isset($routeParts[1]) && isset($routeParts[2])) {
        $method = $routeParts[1];
        $id_camera = $routeParts[2];
        
        if ($method === 'send') {
            $controller->send($id_camera);
        } elseif ($method === 'image') {
            $controller->image($id_camera);
        } else {
            echo 'Invalid method!';
        }
    } else {
        echo 'Invalid URL!';
    }
} else {
    echo 'Invalid URL!';
}
