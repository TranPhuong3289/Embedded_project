<?php
// doc du lieu tu website gui ve
$sample_rate = $_POST["sample_rate"];
$dlpf = $_POST["dlpf"];
$acc = $_POST["acc_fs"];
$gyro = $_POST["gypro_fs"];
$interrupt = $_POST["interrupt"];

// ket noi database
include("config.php");

// gui data xuong database
$sql = "update dataMPU set sample_rate = $sample_rate, dlpf = $dlpf,acc = $acc,gypro=$gyro ,interrupt = $interrupt, isUpdate = 1";
//$sql = "insert into dataMPU(sample_rate,dlpf) values (123,184)";
//$sql = "insert into test2(stt,dt1,dt2) values (,$dlpf,$acc)";

if(!mysqli_query($conn, $sql)){
    echo "error: " . mysqli_error($conn);
}


// doc gia tri tu bang database
// $sql = "select * from dataMPU";
// $result = mysqli_query($conn, $sql);
// $mpu = mysqli_fetch_row($result);

// ngat ket noi voi database
mysqli_close($conn);
?>

