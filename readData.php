<?php
header('Content-Type: application/json');

// dang nhap vao database
include("config.php");

// Doc gia tri RGB tu database
$sql = "select * from chart where stt>(select max(stt) from chart)-20";
$result = mysqli_query($conn,$sql);

$data = array();
foreach ($result as $row){
    $data[] = $row;
}


//add random data
// $rollmode = rand(20, 50);
// $pitchmode = rand(10, 60);

// $sql = "insert into chart(rollmode,pitchmode) values ($rollmode,$pitchmode)";
// mysqli_query($conn,$sql);
mysqli_close($conn);
echo json_encode($data);

?>