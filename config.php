<?php

// kết nối database

$server = "localhost";
$user = "quangthanh";
$password = "123456";
$dbname = "mpu6050";

$conn = mysqli_connect($server,$user,$password,$dbname);
// Kiểm tra kết nối
if ($conn == false) {
  echo "Failed to connect to MySQL: " . mysqli_connect_error();
  exit();
}

?>