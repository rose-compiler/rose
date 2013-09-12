<?php

require_once 'AcceptAbstract.class.php';

class AcceptEncoding extends AcceptAbstract {
	function AcceptEncoding() {
		parent::AcceptAbstract(isset($_SERVER['HTTP_ACCEPT_ENCODING']) ? $_SERVER['HTTP_ACCEPT_ENCODING'] : 'identity');
	}
}

?>
