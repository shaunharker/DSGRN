<?php

chdir ( '../' );
$dbpath = $_GET["db"];
$cmd = $_GET["cmd"];

// Sanitize $dbpath
$dbpath = realpath($dbpath);
if ( ! $dbpath ) die ();
$vroot = "../";
$vroot = realpath($vroot);
if(substr(realpath($dbpath), 0, strlen($vroot)) != $vroot ) die ();

// Sanitize $cmd
$cmd_whitelist = array("zoocohort", "parameters", "annotationcohort", "inequalities", "domaingraph", "networks", "timeseries" );
if ( ! in_array($cmd, $cmd_whitelist) ) die ();

// Parse command

// zoocohort start end
if ( $cmd === "zoocohort" ) {
  $worktype = "sqlite";
  $start = $_GET["start"];
  $end = $_GET["end"];
  // Sanitize start and stop
  if ( ! isset($start) ) die ();
  if ( ! isset($end) ) die ();
  if ( ! is_numeric($start) ) die ();
  if ( ! is_numeric($end) ) die ();
  if ( intval($end)- intval($start) > 100 ) die ();
  $query = "SELECT * FROM MorseGraphZoo limit " . 
           (intval($end)-intval($start)) . " offset " . intval($start) . ";";
// parameters mgi
} else if ( $cmd === "parameters" ) {
  $mgi = $_GET["mgi"];
  // Sanitize mgi
  if ( ! isset($mgi) ) die ();
  if ( ! is_numeric($mgi) ) die ();
  $query = "SELECT ParameterIndex FROM Signatures where MorseGraphIndex = " . intval($mgi) . " limit 1000;";
  $worktype = "sqlite";

// annotationcohort start end fp fpon fpoff fc xc
} else if ( $cmd === "annotationcohort" ) {
  $worktype = "sqlite";
  $start = $_GET["start"];
  $end = $_GET["end"];

  // Sanitize start and end
  if ( ! isset($start) ) die ();
  if ( ! isset($end) ) die ();
  if ( ! is_numeric($start) ) die ();
  if ( ! is_numeric($end) ) die ();
  if ( intval($end)- intval($start) > 100 ) die ();

  $fp = $_GET["fp"];
  $fpon = $_GET["fpon"];
  $fpoff = $_GET["fpoff"];
  $fc = $_GET["fc"];
  $xc = $_GET["xc"];

  // Sanitize fp, fpon, fpoff, fc, xc
  $annot_whitelist = array("Yes", "No", "Either");
  if ( ! isset($fp) ) die ();
  if ( ! isset($fpon) ) die ();
  if ( ! isset($fpoff) ) die ();
  if ( ! isset($fc) ) die ();
  if ( ! isset($xc) ) die ();  
  if ( ! in_array($fp, $annot_whitelist) ) die ();
  if ( ! in_array($fpon, $annot_whitelist) ) die ();
  if ( ! in_array($fpoff, $annot_whitelist) ) die ();
  if ( ! in_array($fc, $annot_whitelist) ) die ();
  if ( ! in_array($xc, $annot_whitelist) ) die ();

  $cond = "";
  if ( $fp === "Yes" ) $cond = $cond . ' and FP=1';
  if ( $fpon === "Yes" ) $cond = $cond . ' and FPON=1';
  if ( $fpoff === "Yes" ) $cond = $cond . ' and FPOFF=1';
  if ( $fc === "Yes" ) $cond = $cond . ' and FC=1';
  if ( $xc === "Yes" ) $cond = $cond . ' and XC=1';
  if ( $fp === "No" ) $cond = $cond . ' and FP=0';
  if ( $fpon === "No" ) $cond = $cond . ' and FPON=0';
  if ( $fpoff === "No" ) $cond = $cond . ' and FPOFF=0';
  if ( $fc === "No" ) $cond = $cond . ' and FC=0';
  if ( $xc === "No" ) $cond = $cond . ' and XC=0';
  $query = "SELECT * FROM MorseGraphZoo where Frequency > 0" . $cond  . 
           " limit " . (intval($end)-intval($start)) . " offset " . intval($start) . ";";

// inequalities pi
} else if ( $cmd === "inequalities" ) {
  $worktype = "dsgrn";
  $pi = $_GET["pi"];
  // Sanitize pi
  if ( ! isset($pi) ) die ();
  if ( ! is_numeric($pi) ) die ();
  $invoke = './bin/dsgrn network ' . escapeshellarg($dbpath) . ' parameter inequalities ' . intval($pi);

// domaingraph pi
} else if ( $cmd === "domaingraph" ) {
  $worktype = "dsgrn";
  $pi = $_GET["pi"];
  // Sanitize pi
  if ( ! isset($pi) ) die ();
  if ( ! is_numeric($pi) ) die ();
  $invoke = './bin/dsgrn network ' . escapeshellarg($dbpath) . ' domaingraph graphviz ' . intval($pi);  
} else if ( $cmd === "networks" ) {
  $worktype = "sqlite";
  $query = "SELECT * from Networks;";
} else if ( $cmd === "timeseries" ) {
  $worktype = "dsgrn";
  $pi = $_GET["pi"];
  // Sanitize pi
  if ( ! isset($pi) ) die ();
  if ( ! is_numeric($pi) ) die ();
  $invoke = './bin/RunSimulation ' . escapeshellarg($dbpath) . ' "`./bin/ParameterSampler ' . escapeshellarg($dbpath) . ' ' . intval($pi) .'`"';
}

/// Dispatch to required behavior "sqlite" or "dsgrn"
if ( $worktype === "sqlite" ) { // sqlite3 operation
  $db = new PDO('sqlite:' . $dbpath);
  $stmt = $db->prepare($query);
  $stmt->execute();
  echo '{"error":null,"data":[';
  $first = true;
  while ($row = $stmt->fetch(PDO::FETCH_ASSOC)) {
    if ( $first === true ) $first = false; else echo ',';
    echo json_encode ( $row ); 
  }
  echo ']}';
  $db=null;
} else if ( $worktype === "dsgrn" ) { // dsgrn operation
  $output = shell_exec($invoke);
  echo $output;
} else {
  die ();
}

?>
