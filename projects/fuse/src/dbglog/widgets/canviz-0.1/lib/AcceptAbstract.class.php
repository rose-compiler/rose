<?php

class AcceptAbstract {
	
	function AcceptAbstract($accept_header) {
		$this->accept = array();
		
		if (empty($accept_header)) {
			return;
		}
		
		$fudge_factor = 0.00001;
		
		$components = preg_split('%\s*,\s*%', $accept_header);
		$temp = array();
		foreach ($components as $i => $component) {
			
			// Find the parts of the string.
			preg_match('%^
				([^\s;]+)            # one or more chars -- the value -- match 1
				(?:                  # begin group (optional)
					\s*;\s*              # semicolon optionally surrounded by whitespace
					q=                   # literal text "q="
					([01](?:\.\d+)?)     # floating-point number, 0 >= n >= 1 -- the quality -- match 2
				)?                   # end group
				$%ix',
				$component, $matches
			);
			
			$value = $matches[1];
			
			// If no quality is given, quality 1 is assumed.
			$q = isset($matches[2]) ? (float)$matches[2] : (float)1;
			
			// Stuff it in the array, if the quality is non-zero.
			if ($q > 0) {
				$temp[$value] = array(
					'value' => $value,
					'q' => $q - ($i * $fudge_factor),
					'i' => $i,
				);
			}
			
		}
		
		// Sort descending by quality.
		usort($temp, create_function('$a, $b', 'return ($a["q"] == $b["q"]) ? 0 : ($a["q"] > $b["q"]) ? -1 : 1;'));
		
		// Unfudge the quality parameter and simplify the array.
		foreach ($temp as $x) {
			$this->accept[$x['value']] = $x['q'] + $x['i'] * $fudge_factor;
		}
	}
	
	function getPreferred() {
		if (empty($this->accept)) {
			return false;
		} else {
			$keys = array_keys($this->accept);
			return $this->accept[$keys[0]];
		}
	}
	
	function acceptable($value) {
		return array_key_exists($value, $this->accept);
	}
	
}

?>
