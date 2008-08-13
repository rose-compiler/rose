<?php
function f() {
    $x = 20;
    $y = 30;
    $z = $x + $y;
    return $z;
}

function main() {
    $g = f();
    $g += 10;
    return $g;
}
?>
