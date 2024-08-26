<?php
class Database {
    private static $dbName = 'tesis';  // Nombre de la base de datos
    private static $dbHost = 'localhost';
    private static $dbPort = '3306'; 
    private static $dbUsername = 'root';  // Usuario de la base de datos
    private static $dbUserPassword = '';  // Contraseña del usuario
    private static $cont = null;
     
    public function __construct() {
        die('Init function is not allowed');
    }
     
    public static function connect() {
        if (null == self::$cont) {     
            try {
                self::$cont = new PDO("mysql:host=".self::$dbHost.";port=".self::$dbPort.";"."dbname=".self::$dbName, self::$dbUsername, self::$dbUserPassword); 
                self::$cont->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_EXCEPTION);
            } catch(PDOException $e) {
                die($e->getMessage()); 
            }
        }
        return self::$cont;
    }
     
    public static function disconnect() {
        self::$cont = null;
    }
}

header('Content-Type: application/json');

if ($_SERVER['REQUEST_METHOD'] === 'POST') {
    if (!empty($_POST['spO2']) && !empty($_POST['ir'])) {
        $spO2 = $_POST['spO2'];
        $ir = $_POST['ir'];

        // Validación adicional
        if (!is_numeric($spO2) || !is_numeric($ir)) {
            echo json_encode(['status' => 'error', 'message' => 'Todos los datos deben ser números válidos.']);
            exit;
        }

        // Conexión y almacenamiento en la base de datos
        $pdo = Database::connect();
        try {
            $sql = "INSERT INTO oximetro (spO2, ir) VALUES (?, ?)";
            $q = $pdo->prepare($sql);
            $q->execute([$spO2, $ir]);

            $lastId = $pdo->lastInsertId();

            Database::disconnect();
            echo json_encode(['status' => 'success', 'message' => 'Dato insertado correctamente.', 'id' => $lastId]);
        } catch (PDOException $e) {
            echo json_encode(['status' => 'error', 'message' => 'Error al insertar el dato: ' . $e->getMessage()]);
        }
    } else {
        echo json_encode(['status' => 'error', 'message' => 'Los datos POST están incompletos o vacíos.']);
    }
} else {
    echo json_encode(['status' => 'error', 'message' => 'Método no permitido.']);
}
?>
