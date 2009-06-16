<?php
/**
 * idb -- Improved DB
 *      This is a simple extension of the mysqli object that allows
 * for dsn based connections as well as arguments based.
 *
 * @author BrYan P. Johnson
 * @package aLib
 * @version 1.0
 */
class idb extends mysqli
  {
    public $user;
    public $pass;
    public $db;
    public $host;
    
    function __construct() 
      {
        $nargs = func_num_args();
        $args = func_get_args();
        if ($nargs == 1) 
          {
            if (is_array($args[0])) 
              {
                $this->user = $args[0]['user'];
                $this->pass = $args[0]['password'];
                $this->db = $args[0]['db'];
                $this->host = $args[0]['host'];
              }
            else
              {
                if (preg_match("#mysql:\/\/([^:]+):([^@]+)\@([^\/]+)\/(.*)#", $args[0], $matches)) 
                  {
                    $this->user = $matches[1];
                    $this->pass = $matches[2];
                    $this->db = $matches[4];
                    $this->host = $matches[3];
                  }
              }
          }
        else
          {
            $this->user = $args[0];
            $this->pass = $args[1];
            $this->db = $args[3];
            $this->host = $args[2];
          }
	try {
	  parent::__construct($this->host, $this->user, $this->pass, $this->db);
	  if ($this->connect_error) {
	    throw new idbConnection_Exception("MySQL Connection error (".$this->connect_errno.") : ".$this->connect_error, $this->connect_errno);
	  }
	} catch (Exception $e) {
	  if (!isset(D::$config['handleErrors']) || D::$config['handleErrors']) {
	    D::exception($e);
	  } else {
	    print "Query Error: ".$e->getMessage();
	    exit;
	  }
	}
      }
    
    function __destruct() 
      {
        $this->close();
      }

    function query($query) {
      try {
	if (D::allowed(DEBUG_QUERY|DEBUG_PROFILE_QUERY)) {
	  $result = $this->logQuery($query);
	} else {
	  if( !$this->real_query($query) ) {// returns true or false
	    throw new idbQuery_Exception( "Query $query returned error: \"".$this->error."\"", $this->errno );
	  }
	  $result = new iRes($this); // build you subclass
	}
	return $result;
      } catch (Exception $e) {
	if (!isset(D::$config['handleErrors']) || D::$config['handleErrors']) {
	  D::exception($e);
	} else {
	  print "Query Error: ".$e->getMessage();
	  exit;
	}
      }
    }

    function logQuery($query) {
      if (D::allowed(DEBUG_PROFILE_QUERY)) {
	$this->real_query("set profiling=1;");
	$starttime = microtime(true);
	if( !$this->real_query($query) ) {// returns true or false
	  throw new idbQuery_Exception( "Query $query returned error: \"".$this->error."\"", $this->errno );
	}
	$result = new iRes($this);
	$endtime = microtime(true);
	$profile = array();
	$profileRes = parent::query("show profile all");
	while ($row = $profileRes->fetch_assoc()) {
	  $profile[] = $row;
	}
	$describe = array();
        $describestring = (stristr($query, 'select'))?"explain extended ":"describe ";
	$describeRes = parent::query($describestring.$query);;
	while ($row = $describeRes->fetch_assoc()) {
	  $describe[] = $row;
	}
	$queryInfo = array(
			   'query' => $query,
			   'elapsed' => $endtime - $starttime,
			   'rows' => $result->num_rows,
			   'columns' => $result->field_count,
			   'profile' => $profile,
			   'description' => $describe
			   );
	D::log($queryInfo, "PROFILE_QUERY");
	$this->real_query("set profiling=0;");
	return $result;
      } else {
	$starttime = microtime(true);
	if( !$this->real_query($query) ) {// returns true or false
	  throw new idbQuery_Exception( "Query $query returned error: \"".$this->error."\"", $this->errno );
	}
	$result = new iRes($this); // build you subclass
	$endtime = microtime(true);
	$queryInfo = array(
			   'query' => $query,
			   'elapsed' => $endtime - $starttime,
			   'rows' => $result->num_rows,
			   'columns' => $result->field_count
			   );
	D::log($queryInfo, "QUERY");
	return $result;
      }
    }

    function prepare($query) {
        $stmt = new iStmt($this, $query);
        return $stmt;
    }

    /**
     * f
     *
     */
    
    function f() 
      {
        print ("F");
      }
  };

class iStmt extends MySQLi_Stmt {
  function execute() {
    try {
    /* Add exceptions here! */
    $retval = parent::execute();
    if ($this->errno > 0) {
      throw new idbQuery_Exception( "Prepared statement returned error: \"".$this->error."\"", $this->errno );
    }
    return $retval;
    } catch (Exception $e) {
      if (!isset(D::$config['handleErrors']) || D::$config['handleErrors']) {
	D::exception($e);
      } else {
	print "Query Error: ".$e->getMessage();
	exit;
      }
    }
  }

  function __destruct() {
    $this->close();
  }
}

class iRes extends MySQLi_Result {
  function __destruct() {
    $this->free();
  }
}


class idb_Exception extends Exception {}
class idbConnection_Exception extends idb_Exception {}
class idbQuery_Exception extends idb_Exception {}

?>
