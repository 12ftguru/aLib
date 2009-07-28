<?php
/**
 * Alib Master File -- just include_once and forget.
 * This file inits several variables. So better check
 * them. (They're documented below). It also defines the anObject
 * object, the one from which all other alib objects should be.
 * defined.
 *
 * @category aLib
 * @package aLib
 * @author BrYan P. Johnson <bryan@12ftguru.com>
 * @license http://www.opensource.org/licenses/bsd-license.php BSD
 * @version 3.0
 * @link http://github.com/12ftguru/aLib/tree/master GitHub Home 
 */

/**
 * function alib_init -- initializes the alib. Don't worry. Just ignore it.
 * 
 * @return none
 */

function alibInit() {
    addIncludePath('.');
    global $debug, $debugLevel, $debugStyle;
    include_once('anObject.inc');
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
 * @param string|array $directory Directory(s) to add to the include path
 * @param bool $recurse Add subdirectories to the include path
 *
 * @return none
 */

function addIncludePath($directory, $recurse = FALSE) {
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
                          
                while (FALSE !== ($filename = readdir($handle))) 
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
  } // End addIncludePath

/**
 * We autoload any object someone tries to load. If there is a file by
 * the same name as the object, we load it. Since object class names
 * are case insensitive, we downcase the filename. No more including!
 *
 * @param string $className name of the class to autoload
 *
 * @return none
 */

function __autoload($className) {
    if (preg_match('/^[A-Za-z0-9\.\-\_]+$/', $className)) {
    if (!include_once (strtolower($className) . ".inc")) 
      {
        include_once ($className . ".inc");
      }
    }
  }



/* Some constants! */

/* Now determine if we're CLI or Web */
if (isset($_SERVER['argc'])) {
    define('CLI', TRUE);
} else {
    define('CLI', FALSE);
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
define("DEBUG_GETSET", 8192);
/* Space for user defined high debugging levels */


define("DEBUG_ALL", 32767);
define("DEBUG_LOW", 1023);
define("DEBUG_HIGH", 31744);
define("DEBUG_OFF", 0);

/* Now I do the alibInit because why wouldn't you? And why should I make
you? */
alibInit();
?>
