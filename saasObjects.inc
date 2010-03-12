<?PHP

/**
 * SAAS extensions to Smart Objects
 */

abstract class saasObject extends smartObject {

  public function __construct($id = NULL, $mydb = NULL) {
    global $config;
    /* If we haven't got a table defined, set it to the plural of the class name */
    if ($this->table === FALSE) {
      $this->table = Inflect::pluralize(strtolower(get_class($this)));
    }
    /* Now check to see if we're saas */
    if (isset($config->saasID) && $config->saasID > 0) {
      $this->table = $config->saasID."_".$this->table;
    }
    parent::__construct($id, $mydb);
  }
}

abstract class saasObjectWithTags extends smartObjectWithTags {

  public function __construct($id = NULL, $mydb = NULL) {
    global $config;
    /* If we haven't got a table defined, set it to the plural of the class name */
    if ($this->table === FALSE) {
      $this->table = Inflect::pluralize(strtolower(get_class($this)));
    }
    /* Now check to see if we're saas */
    if (isset($config->saasID) && $config->saasID > 0) {
      $this->table = $config->saasID."_".$this->table;
    }
    parent::__construct($id, $mydb);
  }

}

?>