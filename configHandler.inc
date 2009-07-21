<?php

/**
 * Config -- Our global config object.
 *
 * @package aLib
 * @subpackage Config
 * @author Bryan P Johnson <bryan@12ftguru.com>
 * @license http://www.opensource.org/licenses/bsd-license.php BSD
 * @version 3.0
 */


/**
 * The Config Object
 *
 * This object is useful for storing various variables related to the configuration of your application:
 * A file called config.inc looking like:
 * <code><?PHP
 * $myVar = "myVal";
 * ?></code>
 * Would be accessed thus:
 * <code>print $config->myVar; //Prints "myVal"</code> 
 *
 */
class configHandler extends anObject
  {
    private $_configVars;
    public $loadedFiles;
    /**
     * Create our config object
     *
     * @param string $filename Config file to load
     *
     * @return object
     */
    public function __construct($filename = FALSE) {
        if ($filename) 
          {
            $this->loadFile($filename);
          }
      }

  /**
   * Overloaded Getter
   *
   * @param string $name Name of the var to get.
   * 
   * @return mixed
   */

    public function __get($name) {
        return $this->_configVars[$name];
      }

    /**
     * Overloaded Setter
     *
     * Returns false because config vars can't be set.
     *
     * @param string $name Name of var to set.
     * @param mixed $value Name of value to set var to.
     *
     * @return bool
     */

    public function __set($name, $value) {
        return FALSE;
      }
    /**
     * Load an additional config file
     *
     * @param string $filename File to load
     *
     * @return bool
     */
    public function loadFile($filename) {
        if ($filename) {
            $origVars = get_defined_vars();
            $origVars['origVars'] = TRUE;
            if (include_once ($filename)) 
              {
                $this->loadedFiles[] = $filename;
                $newVars = get_defined_vars();
                $loadedVars = array_diff(array_keys($newVars) , array_keys($origVars));
                foreach($loadedVars as $key) 
                  {
                    $this->_configVars[$key] = $newVars[$key];
		    unset($GLOBALS[$key]); //don't pollute the global namespace
                  }
              }
          }
      }
  }; //End config object.


global $config, $configFile;
if ($configFile) {
  $config = new configHandler($configFile);
} else {
  $config = new configHandler();
}

?>
