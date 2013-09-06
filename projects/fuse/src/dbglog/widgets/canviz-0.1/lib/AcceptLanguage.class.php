<?php

require_once 'AcceptAbstract.class.php';

class AcceptLanguage extends AcceptAbstract {
	function AcceptLanguage() {
		parent::AcceptAbstract(isset($_SERVER['HTTP_ACCEPT_LANGUAGE']) ? $_SERVER['HTTP_ACCEPT_LANGUAGE'] : '*');
	}
}

?>
