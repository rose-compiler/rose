<?php

require_once 'AcceptAbstract.class.php';

class AcceptMime extends AcceptAbstract {
	function AcceptMime() {
		parent::AcceptAbstract(isset($_SERVER['HTTP_ACCEPT']) ? $_SERVER['HTTP_ACCEPT'] : '*');
	}
}

?>
