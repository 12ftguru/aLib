<?PHP



/**
 * The Debug Object, FirePHP compatible.
 *
 * This is a static object that automatically determines whether you
 * are using cli or web and adjusts its output accordingly.
 *
 * @author Bryan P. Johnson <bryan@12ftguru.com>
 * @version 3.0
 * @package aLib
 * @subpackage Debug
 */

class Debug {
  public static $config = array();
  public static $output = array();
  public  static $counts = array();
  protected static $outputHandler;
  protected static $instance = null;

  public static function getInstance($AutoCreate=false) {
    if($AutoCreate===true && !self::$instance) {
      self::init();
    }
    return self::$instance;
  }

  public static function init() {
    global $config;
    ob_start();
    self::$instance = new self();
    if (is_array($config->debugConfig)) {
      self::$config = $config->debugConfig;
    } 
    /* Set up how we do our output */
    if (self::$config['outputHandler']) {
      //ignore
    } else if (CLI) {
      self::$config['outputHandler'] = 'plainText';
    } else {
      self::$config['outputHandler'] = 'firePHP';
    }
    $oh = self::$config['outputHandler'];
    $oh .= 'DebugOutputHandler';
    self::$outputHandler = new $oh();
    /* We started when? */
    self::$config['started'] = microtime(true);

    if (self::$config['handleErrors'] || !isset(self::$config['handleErrors'])) {
      $flags = (isset(self::$config['reportErrors']))?self::$config['reportErrors']:E_ALL;
      if (!empty(self::$config['ignoreNotices'])) {
            $flags = $flags ^ E_NOTICE;
            $flags = $flags ^ E_USER_NOTICE;
      }
      if (!empty($options['ignoreWarnings'])) {
	$flags = $flags ^ E_WARNING;
	$flags = $flags ^ E_USER_WARNING;
      }
      set_error_handler(array('Debug', "error"), $flags);
      error_reporting($flags);
      set_exception_handler(array('Debug', "exception"));
    }

  }

  public static function log($mixed, $type = "LOG", $group = "") {
    $foo = constant("DEBUG_".$type);
    $bar = self::$config['debugLevel'];
    if (constant('DEBUG_'.$type) & self::$config['debugLevel']) {
      $log = array(
		   'data' => $mixed,
		   'type' => $type,
		   'dtime' => microtime(true),
		   'group' => $group
		   );
      if (self::$config['groupByClass'] && ($group == "" || $group == NULL)) {
	$bt = debug_backtrace();
	while ($abt = array_shift($bt)) {
	  if (isset($abt['class']) && $abt['class'] != 'Debug' && $abt['class'] != 'D') {
	    break;
	  }
	}
	  if (isset($abt['class']) && $abt['class']) {
	  $group = $abt['class'];
	} else if ($abt['file']) {
	  $group = $abt['file'];
	} else {
	  $group = NULL;
	  
	}
	  $log['group'] = $group;
	  $log['bt'] = $bt;
      }
      self::$output[] = $log;
      (isset(self::$counts[$type]))?self::$counts[$type]++:1;
    }
  }

  public static function info($mixed, $group = "") {
    self::log($mixed, 'INFO', $group);
  }

  public static function warn($mixed, $group = "") {
    self::log($mixed, 'WARN', $group);
  }

  public static function memory($mixed, $name = "PHP", $group = "") {
    $memory = memory_get_usage();
    if ($mixed) {
      $memory = strlen(serialize($mixed));
    }
    $logItem = array(
		     "Memory Usage" => $memory,
		     );
    if ($mixed) {
      $logItem["name"] = $name;
      $logItem["dataType"] = gettype($mixed);
      $logItem["variable"] = $mixed;
      self::log($logItem, 'VARIABLE', $group);
    } else {
      self::log($logItem, 'MEMORY', $group);
    }
  }

  public static function variable($mixed, $name = "VARIABLE", $group = "") {
    self::memory($mixed, $name, $group);
  }

  public static function v($mixed, $name = "VARIABLE", $group = "") {
    self::memory($mixed, $name, $group);
  }

  public static function trace($group = "") {
    $trace = debug_backtrace();
    /* We remove ourselves from the bt */
    if ($trace[0]['function'] == 'trace' && $trace[0]['type'] = '::') {
      $cruft = array_shift($trace);
    }
    self::log($trace, 'TRACE', $group);
  }




  /* Error and exception handling (no grouping) */

  public static function exception (Exception $exception) {
    self::log($exception, 'EXCEPTION');
  }

  public static function error($errno, $errstr, $file, $line, $context) {
      $errorType = array (
			  E_ERROR          => 'ERROR',
			  E_WARNING        => 'WARNING',
			  E_PARSE          => 'PARSING ERROR',
			  E_NOTICE         => 'NOTICE',
			  E_CORE_ERROR     => 'CORE ERROR',
			  E_CORE_WARNING   => 'CORE WARNING',
			  E_COMPILE_ERROR  => 'COMPILE ERROR',
			  E_COMPILE_WARNING => 'COMPILE WARNING',
			  E_USER_ERROR     => 'USER ERROR',
			  E_USER_WARNING   => 'USER WARNING',
			  E_USER_NOTICE    => 'USER NOTICE',
			  E_STRICT         => 'STRICT NOTICE',
			  E_RECOVERABLE_ERROR  => 'RECOVERABLE ERROR'
			  );
      $errMsg = $errorType[$errno].': '.$errstr;
      $exception = new ErrorException($errMsg, 0, $errno, $file, $line);
      $error = array('message' => $errMsg,
		     'file' => $file,
		     'line' => $line,
		     'exception' => $exception);
      self::log($error, 'ERROR');
  }

  public static function allowed($type) {
    $foo = self::$config['debugLevel'];
    return $foo & $type;
  }

   function __destruct() {
     self::$outputHandler->output();
     $string = ob_get_contents();
     ob_end_clean();
     print $string;
  }



}
 
/* For ease of typing: */

class D extends Debug {

}


/* Abstract class to extend for output */

abstract class DebugOutputHandler {
  abstract public function output();
}

?>