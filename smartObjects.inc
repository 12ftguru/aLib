<?

/**
 * Smart Objects
 *
 * Smart objects are objects tied directly to a single record in a db.
 */

abstract class smartObject extends anObject {
  public $table = FALSE;
  public $idField = FALSE;
  protected $db;
  public $error;
  public $errno;
  public $insertID;
  protected $foreignKeys = array();
  protected $serializedKeys = array();
  protected $serializeMethod = 'json'; // json or php
  /*
    protected $format = array(
      'string' => '', // a sprintf style format string
      'vars' => array() // class variables;
    );
  */
  protected $format = 'json'; // interpret as a json string when called as a string.
  protected $vars = array();

  public function __construct($id = NULL) {
    global $db;
    if ($this->idField === FALSE) {
      $this->idField = get_class($this)."ID";
    }
    if ($this->table === FALSE) {
      $this->table = strtolower(get_class($this))."s";
    }
    if (is_a($db, 'idb')) {
      $this->db = $db;
    }
    if (is_array($id)) {
      foreach ($id as $key => $val) {
	$this->{$key} = $val;
      }
    } else if ($id) {
      $this->{$this->idField} = $id;
      $this->read();
    }
  }

    public function __set($name, $value) 
      {
	if ($name == 'id' && $this->idField != 'id') {
          $this->vars[$this->idField] = $value;
	} else if (in_array($name, $this->serializedKeys)) {
          if ($this->serializeMethod == 'php') {
            $this->vars[$name] = serialize($value);
          } else {
            $this->vars[$name] = json_encode($value);
          }
        } else if (in_array($name, array_keys($this->foreignKeys))) {
          if (is_object($value) && ($value instanceof $this->foreignKeys[$name]['type'])) {
            $this->vars[$this->foreignKeys[$name]['key']] = $value->id;
            $this->foreignKeys[$name]['obj'] = $value;
          }
	} else {
	  $this->vars[$name] = $value;
	}
      }


  public function __get($name) 
      {
	if ($name == 'id' && $this->idField != 'id') {
	  return $this->vars[$this->idField];
	} else if (in_array($name, $this->serializedKeys)) {
          if ($this->serializeMethod == 'php') {
            return unserialize($this->vars[$name]);
          } else {
            return json_decode($this->vars[$name]);
          }
	} else if (!is_array($this->foreignKeys[$name])) {
	  return $this->vars[$name];
	} else {
	  if (!is_a($this->foreignKeys[$name]['obj'], 'smartObject')) {
	    $type = $this->foreignKeys[$name]['type'];
	    $fKey = $this->foreignKeys[$name]['key'];
	    $this->foreignKeys[$name]['obj'] = new $type($this->{$fKey});
	  }
	  return $this->foreignKeys[$name]['obj'];
	}
      }

  public function __toString() {
    if ($this->format != 'json') {
      $vars = array($this->format['string']);
      foreach ($this->format['vars'] as $var) {
        $vars[] = $this->vars[$var];
      }
      return call_user_func_array('sprintf', $vars);
    } else {
      foreach (array_keys($this->vars) as $key) {
        $retval[$key] = $this->vars[$key];
      }
      $retval['sotype'] = get_class($this);
      return json_encode($retval);
    }
  }


  public function reset() {
    foreach ($this->vars as $key => $val) {
      $this->vars[$key] = NULL;
    }
    foreach ($this->foreignKeys as $name => $arr) {
      unset($arr['obj']);
	$arr['obj'] = NULL;
    }
  }


  public function read() {
    if (is_a($this->db, 'idb')) {
      $sql = "select * from `".$this->table."` where `".$this->idField."` = '".$this->db->real_escape_string($this->vars[$this->idField])."'";
      $this->reset();
      Debug::log('SmartObject Read SQL: '.$sql);
      $res = $this->db->query($sql);
      if (is_object($res)) {
        if ($res->num_rows == 1) {
          $row = $res->fetch_assoc();
          foreach ($row as $key => $val) {
            $this->vars[$key] = $val;
          }
          return TRUE;
        } else {
          return FALSE;
        } 
      } else {
        return $db->error;
      }
    } else {
      return FALSE;
    }
  }

  public function insert() {
    if (is_a($this->db, 'idb')) {
      $sql = "insert into `".$this->table."` (";
      foreach ($this->foreignKeys as $fkey => $farray) {
        $this->vars[$farray['key']] = $farray['obj']->id;
      }
      $keys = array_keys($this->vals);
      $sql .= implode(',', $keys);
      $sql .= ") values (";
      $sql .= array_map(array($this->db, 'real_escape_string'), array_values($this->vals));
      $sql .= ")";
      Debug::log("Smart Object attempting to insert: ".$sql);
      $this->db->query($sql);
      if ($this->db->insert_id > 0) {
        $this->vals[$this->idField] = $this->db->insert_id;
        return $this->vals[$this->idField];
      }
    } else {
      return FALSE;
    }
  }

  public function update() {
    if (is_a($this->db, 'idb')) {
      $sql = "update `".$this->table."` set ";
      foreach ($this->foreignKeys as $fkey => $farray) {
        $this->vars[$farray['key']] = $farray['obj']->id;
      }
      foreach ($this->vars as $key => $val) {
        if ($key != $this->idField) {
        $sql .= "`$key` = '".$this->db->real_escape_string($val)."', ";
        }
      }
      $sql = substr($sql, 0, -2);
      $sql .= " where `".$this->idField."` = '".$this->db->real_escape_string($this->vars[$this->idField])."'";
      Debug::log("Smart Object attempting to update: ".$sql);
      $this->db->query($sql);
      if ($this->db->errno == 0) {
        return TRUE;
      }
    }
    return FALSE;
  }

  public function delete() {
    if (is_a($this->db, 'idb')) {
      $sql = "delete from `".$this->table."` where `".$this->idField."` = '".$this->db->real_escape_string($this->vars[$this->idField])."' limit 1";
      Debug::log('SmartObject Delete SQL: '.$sql);
      $res = $this->db->query($sql);
      $this->reset();
      if ($this->db->errno == 0) {
        return TRUE;
      } else {
        return FALSE;
      }
    } else {
      return FALSE;
    }
  }



  public function load() {
    return $this->read();
  }
  public function create() {
    return $this->insert();
  }
  public function save() {
    return $this->update();
  }
  public function nuke() {
    return $this->delete();
  }


}

/* You should never have to extend this. Use the factory below. */
class smartResult extends MySQLi_Result implements IteratorAggregate {
  public $resultType;
  public $resultParams;

  public function getIterator()  
  {  
    return new smartResultIterator($this);  
  }  

  function fetch($type = NULL, $params = NULL) {
    if (!$type) { $type = $this->resultType; }
    if (!$params) { $params = $this->resultParams; }
    if ($type) {
      return $this->fetch_object($type, $params);
    } 
  }
}

class smartResultIterator implements Iterator  
{  
  protected $result;  
  protected $position;  
  protected $currentRow;  
  
  /** 
   * Constructor 
   * @param MySQLi_Result $result 
   */  
  public function __construct($result)  
  {  
    $this->result = $result;  
  }  
  
  /** 
   * Destructor 
   * Frees the result object 
   */  
  public function __destruct()  
  {  
    $this->result->free();  
  }  
  
  /** 
   * Rewinds the internal pointer 
   */  
  public function rewind()  
  {  
    // data_seek moves the results internal pointer  
    $this->result->data_seek($this->position = 0);  
    // prefetch the current row  
    // note that this advances the results internal pointer.  
    $this->currentRow = $this->result->fetch();  
  }  
  
  /** 
   * Moves the internal pointer one step forward 
   */  
  public function next()  
  {  
    // prefetch the current row  
    $this->currentRow = $this->result->fetch();  
  
    // and increment internal pointer  
    ++$this->position;  
  }  
  
  /** 
   * Returns true if the current position is valid, false otherwise. 
   * @return bool 
   */  
  public function valid()  
  {  
    return $this->position < $this->result->num_rows;  
  }  
  
  /** 
   * Returns the row that matches the current position 
   * @return array 
   */  
  public function current()  
  {  
    return $this->currentRow;  
  }  
  
  /** 
   * Returns the current position 
   * @return int 
   */  
  public function key()  
  {  
    return $this->position;  
  }  
} 


class smartObjectFactory {
  private static $instance = NULL;
  private $objType = NULL;
  private $objArgs = NULL;
  private $queryString = "";
  private $db;
  private $numWheres = 0;

  private function __construct($db) {
    if (!$db) { $this->db = $GLOBALS['db']; };
  }

  public function getInstance($db = NULL) {
    if (!self::$instance) {
      self::$instance = new smartObjectFactory($db);
    }
    return self::$instance;
  }

  public function __call($method, $args) {
    if ($this->queryString == "") {
      if (count($args) == 1 && !is_array($args[0])) {
        return new $method($args[0]);
      } else if (count($args) == 0) {
        $tmp = new $method();
        $this->objType = $method;
        $this->objArgs = $args;
        $this->queryString = "select * from `".$tmp->table."`";
      }
    } else if (strtolower($method) == "where") {
      $this->queryString .= " where ".$args[0];
      $this->numWheres++;
    } else if (strtolower($method) == "limit") {
      $this->queryString .= " limit ".$args[0].",".$args[1];
    } else if (strtolower($method) == "groupby") {
      $this->queryString .= " group by ";
      if (is_array($args)) {
        $this->queryString .= implode(', ',$args);
      } else {
        $this->queryString .= $args;
      }
    } else if (strtolower($method) == "orderby") {
      $this->queryString .= " order by ";
      if (is_array($args)) {
        $this->queryString .= implode(', ',$args);
      } else {
        $this->queryString .= $args;
      }
    } else if (strtolower($method) == "or") {
      if (! stristr($this->queryString, ' where ')) {
        $this->queryString .= " where";
      } else if ($this->numWheres > 0) {
        $this->queryString .= " or";
      }
      $this->queryString .= " ".$args[0];
      $this->numWheres++;
    } else if (strtolower($method) == "and") {
      if (! stristr($this->queryString, ' where ')) {
        $this->queryString .= " where";
      } else if ($this->numWheres > 0) {
        $this->queryString .= " and";
      }
      $this->queryString .= " ".$args[0];
      $this->numWheres++;
    } else {
      if (! stristr($this->queryString, ' where ')) {
        $this->queryString .= " where";
      } else if ($this->numWheres > 0) {
        $this->queryString .= " and";
      }
      $this->queryString .= " `".$method."` = '".$this->db->real_escape_string($args[0])."'";
      $this->numWheres++;
    }
    Debug::log("SOF QS: ".$this->queryString);
    return self::$instance;
  }

  public function reset() {
    $this->queryString = "";
    $this->numWheres = 0;
  }

  public function query($query = NULL, $db = NULL) {
    if (!$db) { $db = $GLOBALS['db']; };
    if (!$query && $this->queryString != "") {
      $query = $this->queryString;
    }
    if ($db->real_query($query)) {
      $smartResult = new smartResult($db);
      $smartResult->resultType = $this->objType;
      $smartResult->resultParams = $this->objArgs;
      Debug::variable($smartResult);
      $this->reset();
      return $smartResult;
    } else {
      return FALSE;
      // A real programmer would add exceptions here.
    }
  }

  public function f($query = NULL, $db = NULL) {
   return $this->query($query, $db);
  }

}


?>