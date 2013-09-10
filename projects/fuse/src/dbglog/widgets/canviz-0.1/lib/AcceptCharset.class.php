<?php

require_once 'AcceptAbstract.class.php';

class AcceptCharset extends AcceptAbstract {
	function AcceptCharset() {
		parent::AcceptAbstract(isset($_SERVER['HTTP_ACCEPT_CHARSET']) ? $_SERVER['HTTP_ACCEPT_CHARSET'] : '*');
	}
}

?>
