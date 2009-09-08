<?PHP

class iuser extends smartObject {
  public $table = 'users';
  public $idField = 'userID'; //or FALSE 
 /* protected $foreignKeys = array('buddy' => array('key' => 'buddyID',
                                                  'type' => 'iuser')); // '<name>' => array('key' => '<foreignKey>',
  	    		   	       // 	            'type' => '<smartObjectType>), */
  protected $serializedKeys = array();
  
   protected $format = array(
			  'string' => '%s %s <%s>', // a sprintf style format string
			  'vars' => array('gn', 'sn', 'email') // class variables;
			  );
   //  protected $format = 'json';
  
}


/* Example table:

CREATE TABLE IF NOT EXISTS `users` (
  `loginName` varchar(255) CHARACTER SET utf8 NOT NULL,
  `gn` varchar(32) DEFAULT NULL,
  `sn` varchar(32) DEFAULT NULL,
  `email` varchar(128) DEFAULT NULL,
  `createDate` int(64) DEFAULT NULL,
  `modDate` int(64) DEFAULT NULL,
  `passwd` varchar(128) DEFAULT NULL,
  `status` tinyint(4) NOT NULL DEFAULT '0',
  `access` tinyint(4) NOT NULL DEFAULT '0',
  `userID` int(10) NOT NULL AUTO_INCREMENT,
  PRIMARY KEY (`loginName`),
  KEY `emailSearch` (`email`(6)),
  KEY `idnum` (`userID`)
) ENGINE=MyISAM  DEFAULT CHARSET=latin1;

*/

?>