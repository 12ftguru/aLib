<?PHP

include_once("FirePHP.class.php");

class firePHPDebugOutputHandler extends DebugOutputHandler {
  public $fb;
  public $groups;
  
  public function __construct() {
    $this->fb = FirePHP::getInstance(true);
  }

  public function output() {
    $dconfig = Debug::$config;
    if (isset($dconfig['detailedOutput']) && $dconfig['detailedOutput']) {
    $now = microtime(true);
    $lastTime = $dconfig['started'];
    $mem = $this->getReadableBytes(memory_get_peak_usage());
    $elapsed = $now - $dconfig['started'];
    $this->fb->group('Summary');
    $this->fb->fb('Elapsed Time: '.$this->getReadableTime($elapsed));
    $this->fb->fb('Memory Consumption: '.$mem);
    if (isset($dconfig['showIncludedFiles']) && $dconfig['showIncludedFiles']) {
      $files = get_included_files();
      $fileTable = array();
      $totalSize = 0;
      $totalFiles = 0;
      $fileTable[] = array('Name', 'Size');
      foreach ($files as $file) {
	$size = filesize($file);
	$fileTable[] = array($file, $this->getReadableBytes($size));
	$totalSize += $size;
	$totalFiles++;
      }
      $this->fb->table($totalFiles.' included files ('.$this->getReadableBytes($totalSize).')', $fileTable);
    }
    $this->fb->groupEnd();
    $this->fb->group('Details', array('Collapsed' => TRUE));
    }
    foreach (Debug::$output as $row) {
      extract($row);
      if ($group != "" && $group != NULL) {
	if (count($this->groups) > 0 && $group != $this->groups[count($this->groups) - 1]) {
	  // Are we the up group or a new group
	  if (isset($dconfig['groupByClass']) && $dconfig['groupByClass']) {
	    if (isset($bt[0]) && isset($bt[0]['class'])) {
	      if ($bt[0]['class'] == $this->groups[count($this->groups) - 2]) {
		$this->fb->groupEnd();
		array_pop($this->groups);
	      } else {
		// new group!
		$this->fb->group($group, array('Collapsed' => TRUE));
		$this->groups[] = $group;
	      }
	    } else {
	      $this->fb->groupEnd();
	      array_pop($this->groups);
	    }
	  } else {
	    // new group!
	    $this->fb->group($group, array('Collapsed' => TRUE));
	    $this->groups[] = $group;
	  }
	} else if (count($this->groups) == 0) {
	    // new group!
	    $this->fb->group($group, array('Collapsed' => TRUE));
	    $this->groups[] = $group;
	}
      } else if (count($this->groups) > 0) {
	$this->fb->groupEnd();
	array_pop($this->groups);
      }
    if (isset($dconfig['detailedOutput']) && $dconfig['detailedOutput']) {
      $myTime = $this->getReadableTime($dtime - $lastTime).' -- ';
      $lastTime = $dtime;
    } else {
      $myTime = "";
    }
      if ($type == "EXCEPTION") {
	$fbtype = FirePHP::ERROR;
	$this->fb->fb($data, $myTime.ucfirst($type), $fbtype);
      } else if ($type == "ERROR") {
	$fbtype = FirePHP::ERROR;
	$this->fb->fb($data['exception'], $myTime.ucfirst($type), $fbtype);
      } else if ($type == "WARN") {
	$fbtype = FirePHP::WARN;
	$this->fb->fb($data, $myTime.'Warning', $fbtype);
      } else if ($type == "INFO") {
	$fbtype = FirePHP::INFO;
	$this->fb->fb($data, $myTime.'Info', $fbtype);
      } else if ($type == "VARIABLE") {
	$fbtype = FirePHP::LOG;
	if (isset($data['Memory Usage'])) {
	  $data['Memory Usage'] = $this->getReadableBytes($data['Memory Usage']);
	}
	$this->fb->fb($data, $myTime.$data['name'], $fbtype);
      } else if ($type == "QUERY") {
	$fbtype = FirePHP::LOG;
	$msg = '"'.$data['query'].'" took '.$this->getReadableTime($data['elapsed']). ' and returned '.$data['rows'].' row(s) and '.$data['columns'].' column(s).';
	$this->fb->fb($msg, 'MySQL Query', $fbtype);
      } else if ($type == "PROFILE_QUERY") {
	$this->fb->group('MySQL Query Profile for: "'.$data['query'].'"', array("Collapsed" => TRUE));
	$this->fb->fb('Query took '.$this->getReadableTime($data['elapsed']). ' and returned '.$data['rows'].' row(s) and '.$data['columns'].' column(s).');
	$description = array();
	$description[] = array_keys($data['description'][0]);
	$description[] = array_values($data['description'][0]);
	$this->fb->table("Query Description", $description);
	$profile = array();
	$profile[] = array_keys($data['profile'][0]);
	foreach ($data['profile'] as $pArray) {
	  $profile[] = array_values($pArray);
	}
	$this->fb->table("Query Profiler", $profile);
	$this->fb->groupEnd();
      } else {
	$fbtype = FirePHP::LOG;
	$this->fb->fb($data, $myTime.$type, $fbtype);
      }
    }
    if (isset($dconfig['detailedOutput']) && $dconfig['detailedOutput']) {
      $this->groups[] = "Details"; // Close it out.
    }
    //Catch any unclosed groups.
    foreach ($this->groups as $group) {
    $this->fb->groupEnd();
    }
  }

  public function getReadableBytes($size, $retstring = null) {
        	// adapted from code at http://aidanlister.com/repos/v/function.size_readable.php
	       $sizes = array('bytes', 'kB', 'MB', 'GB', 'TB', 'PB', 'EB', 'ZB', 'YB');

	       if ($retstring === null) { $retstring = '%01.2f %s'; }

		$lastsizestring = end($sizes);

		foreach ($sizes as $sizestring) {
	       	if ($size < 1024) { break; }
	           if ($sizestring != $lastsizestring) { $size /= 1024; }
	       }
	       if ($sizestring == $sizes[0]) { $retstring = '%01d %s'; } // Bytes aren't normally fractional
	       return sprintf($retstring, $size, $sizestring);
	}
	
	public function getReadableTime($time) {
		$ret = $time;
		$formatter = 0;
		$formats = array('ms', 's', 'm');
		if($time >= 1000 && $time < 60000) {
			$formatter = 1;
			$ret = ($time / 1000);
		}
		if($time >= 60000) {
			$formatter = 2;
			$ret = ($time / 1000) / 60;
		}
		$ret = number_format($ret,6,'.','') . $formats[$formatter];
		return $ret;
	}

}

?>