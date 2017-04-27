<?php

// Query variables
$name = $_POST["name"];
$email_raw = $_POST["email"];
$user = $_POST["user"];

$html_begin = '<!DOCTYPE html>
<html>
<head>
  <meta http-equiv="Content-Type" content="text/html; charset=UTF-8" />
  <title>DSGRN Account Creation Page</title>
  <link rel="stylesheet" href="css/index.css">
</head>
<header>
<h1>
Workshop account creation page
</h1>
</header>
<body>
<div style="background: #777777; margin:50px; padding: 50px; border: black 1px
 solid; font-size:200%">
<p>';

$html_end = '</p></div></body></html>';

// Sanitize email
$email = filter_var($email_raw, FILTER_SANITIZE_EMAIL);
if ( ! filter_var($email, FILTER_VALIDATE_EMAIL)) {
  echo $html_begin . "There is a problem with e-mail address." . $html_end;
  exit;
}

// Email the link
$command = "add_user.sh '" . $name . "' " . $email . " " . $user;
shell_exec('echo "' . $command . '"' . " | mail -s 'Account Creation' sharker@math.rutgers.edu" );

// Success
echo $html_begin . "An email has been sent to the system administrator. You should be contacted soon." . $html_end;

?>
