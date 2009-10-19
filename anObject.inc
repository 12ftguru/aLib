<?PHP
/**
 * Anobject -- the Generic Abstract aLib object. 
 *
 * All of your objects should inherit from this if possible.
 *
 * @package aLib
 * @subpackage aLibCore
 * @author Bryan P Johnson <bryan@12ftguru.com>
 * @license http://www.opensource.org/licenses/bsd-license.php BSD
 * @version 3.0
 */

/** 
 * The aLib abstract standard object.
 *
 * This way we ensure that all of our objects have the same
 * interfaces.
 *
 */
abstract class anObject {
  
  /**
   * Create the object.
   */
    public function __construct() {
      }

  /**
   * Overloaded Getter
   *
   * @param string $name Name of the var to get. Checks for camelCase and under_score versions.
   * 
   * @return mixed
   */

    public function __get($name) {
        global $debug;
	if ($this->$name) {
          return $this->$name;
	} else {
	  $v = get_class_vars(get_class($this));
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
     *
     * @param string $f Function name
     * @param string $a Arguments for the function
     *
     * @return mixed Return Value of function.
     */
    public function __call($f, $a) {
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


?>