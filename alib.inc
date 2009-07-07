<?php
/**
 * Alib Master File -- just include_once and forget.
 * This file inits several variables. So better check
 * them. (They're documented below). It also defines the anObject
 * object, the one from which all other alib objects should be.
 * defined.
 * @package aLib
 * @author BrYan P. Johnson
 * @version 3.0
 */
/**
 * function alib_init -- initializes the alib. Don't worry. Just ignore it.
 */

function alibInit() 
  {
    addIncludePath('.');
    global $debug, $debugLevel, $debugStyle;
    include_once ('configHandler.inc');
    if ($config->oldDebug || isset($debugLevel)) {
      if ($debugStyle == 'firebug') {
	include_once ('debug.fb.inc');
	$debug = Debug::singleton();
	$debug->setEnabled(TRUE);
      } else {
	include_once ('debug.old.inc');
	$debug = Debug::singleton();
      }
      $debug->level = ($debugLevel) ? $debugLevel : 0;
    } else {
      include_once('debug.inc');
      Debug::init();
    }
    include_once ('smartObjects.inc');
  }
/**
 * Add a path to the include path, independent of system
 *
 * @param string or array $directory Directory(s) to add to the include path
 * @param bool $recurse Add subdirectories to the include path
 * @return none
 */

function addIncludePath($directory, $recurse = FALSE) 
  {
    if (is_array($directory)) 
      {
        foreach($directory as $dir) 
          {
            addIncludePath($dir, $recurse);
          }
      }
    else
      {
          
        if (stristr(php_uname('s') , 'win') && !stristr(php_uname('s') , 'dar')) 
          {
            /* We're on Windows */
            $delim = ";";
          }
        else
          {
            $delim = ":";
            /* Pretty much everyone else. */
          }
        set_include_path(get_include_path() . $delim . $directory);
        if ($recurse) 
          {
              
            if ($handle = opendir($directory)) 
              {
                          
                while (false !== ($filename = readdir($handle))) 
                  {
                     
                      
                    if ($filename != '.' && $filename != '..') {
                         if (is_dir($directory.'/'.$filename)) 
                      {
                         
                        addIncludePath($directory.'/'.$filename, $recurse);
                      }
                         }
                  }
              }
          }
      }
  } # End addIncludePath

/**
 * We autoload any object someone tries to load. If there is a file by
 * the same name as the object, we load it. Since object class names
 * are case insensitive, we downcase the filename. No more including!
 */

function __autoload($class_name) 
  {
    if (preg_match('/^[A-Za-z0-9\.\-\_]+$/', $class_name)) {
    if (!include_once (strtolower($class_name) . ".inc")) 
      {
        include_once ($class_name . ".inc");
      }
    }
  }
/** 
 * The aLib abstract standard object.
 *
 * This way we ensure that all of our objects have the same
 * interfaces.
 * @package aLib
 * @subpackage Utility
 * @author BrYan P. Johnson
 * @version 3.0
 */
abstract class anObject
  {
    public 
    function __construct() 
      {
      }
    public 
    function __get($name) 
      {
        global $debug;
	if ($this->$name) {
          return $this->$name;
	} else {
	  $v = get_class_vars($this);
	  $underscores = strtolower(preg_replace('/([A-Z]{1,1}[A-Za-z]+)/', '_$1', $name));
	  $camelcase = strtolower(preg_replace('/_/', '', $name));
	  if (is_array($v) && in_array($underscores, $v)) {
	    return $this->$underscores;
	  } else if (is_array($v) && in_array($camelcase, $v)) {
	    return $this->$camelcase;
	  } else {
	    return NULL;
	  }
	}
      }
    /**
     * This catches any calls to undefined functions.
     * If you camelcased when the function was underscored ( fetchAssoc versus fetch_assoc ) or vice versa, we will autocorrect.
     * Yum. With debuglevel of 0, it's silent.
     */
    public 
    function __call($f, $a) 
      {
        global $debug;
	$m = get_class_methods($this);
	$c = get_class($this);
            
	// Check to see if we camelcased and meant underscores:

        $underscores = strtolower(preg_replace('/([A-Z]{1,1}[A-Za-z]+)/', '_$1', $f));
	$camelcase = strtolower(preg_replace('/_/', '', $f));


	if (in_array($underscores, $m)) {
	  call_user_func_array(array($this, $underscores), $a);
	} else if (in_array($camelcase, $m)) {
	  call_user_func_array(array($this, $camelcase), $a);
	} 
      }

  }



/* Some constants! */

/* Now determine if we're CLI or Web */
if (isset($_SERVER['argc'])) {
    define('CLI', true);
} else {
    define('CLI', false);
}

/* Our debug levels: */
define("DEBUG_LOG", 1);
define("DEBUG_WARN", 2);
define("DEBUG_INFO", 4);
define("DEBUG_ERROR", 8);
define("DEBUG_EXCEPTION", 16);
define("DEBUG_VARIABLE", 32);
define("DEBUG_QUERY", 64);
/* Space for user defined debugging low levels*/

define("DEBUG_TRACE", 1024);
define("DEBUG_MEMORY", 2048);
define("DEBUG_PROFILE_QUERY", 4096);
/* Space for user defined high debugging levels */


define("DEBUG_ALL", 32767);
define("DEBUG_LOW", 1023);
define("DEBUG_HIGH", 31744);
define("DEBUG_OFF", 0);

/* Now I do the alibInit because why wouldn't you? And why should I make
you? */
alibInit();
?>
