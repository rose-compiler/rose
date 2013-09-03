<?php

/*
 * This file is part of Canviz. See http://www.canviz.org/
 * $Id: image.php 246 2008-12-27 08:36:24Z ryandesign.com $
 */

define('GRAPH_IMAGES_PATH', 'graphs/images');

$image = GRAPH_IMAGES_PATH . $_SERVER['PATH_INFO'];
$image = strtr($image, array('../' => ''));
$info = getimagesize($image);
$size = filesize($image);
header('Content-Type: ' . $info['mime']);
header('Length: ' . $size);
//usleep(mt_rand(500, 2000) * 1000);
readfile($image);

?>
