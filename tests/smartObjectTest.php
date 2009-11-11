<?php

error_reporting(E_ALL ^ E_NOTICE);
include_once(dirname(__FILE__).'/../alib.inc');
addIncludePath(dirname(__FILE__).'/../');
addIncludePath(dirname(__FILE__).'/mockery/library', TRUE);
include_once('Mockery.php');
include_once('Mockery/Framework.php');
//include_once('idb.inc');

/* Can't test an abstract class directly. */

$db = Mockery::mock('idb');


class testSmartObject extends smartObject {};

/**
 * Test class for smartObject.
 * Generated by PHPUnit on 2009-09-15 at 13:56:23.
 */
class smartObjectTest extends PHPUnit_Framework_TestCase
{
    /**
     * @var    smartObject
     * @access protected
     */
    protected $object;

    /**
     * Sets up the fixture, for example, opens a network connection.
     * This method is called before a test is executed.
     *
     * @access protected
     */
    protected function setUp()
    {
      global $db;
      $this->object = new testSmartObject();
      $this->object->db = $db;
    }

    /**
     * Tears down the fixture, for example, closes a network connection.
     * This method is called after a test is executed.
     *
     * @access protected
     */
    protected function tearDown()
    {
      
      unset($this->object->db);

    }

    /**
     * @todo Implement test__set().
     */
    public function test__set()
    {
      $this->object->foo = 'bar';
      $this->assertEquals('bar', $this->object->vars['foo']);
    }

    /**
     * @todo Implement test__get().
     */
    public function test__get()
    {
      $this->object->foo = 'bar';
      $this->assertEquals('bar', $this->object->foo);
    }

    /**
     * @todo Implement test__toString().
     */
    public function test__toString()
    {
      $this->object->foo = 'bar';
      ob_start();
      print $this->object;
      $string = ob_get_contents();
      ob_end_clean();
      $this->assertEquals('{"foo":"bar","sotype":"testSmartObject"}', $string);
   }

    /**
     * @todo Implement testReset().
     */
    public function testReset()
    {
      $this->object->foo = 'bar';
      $this->object->reset();
      $this->assertEquals(NULL, $this->object->vars['foo']);
    }

    /**
     * @todo Implement testRead().
     */
    public function testRead()
    {
      $this->object->testSmartObjectID = 1;

      $retval = array('testSmartObjectID' => 1, 'column1' => 'value1', 'column2' => 'value2');

      $testResult = mockery('result', array('fetch_assoc' => $retval));
      $testResult->num_rows = 1;


      $this->object->db->shouldReceive('real_escape_string')->withAnyArgs()->zeroOrMoreTimes()->andReturn(1);
      $this->object->db->shouldReceive('query')->withArgsMatching("/select \* from \`testsmartobjects\` where \`testSmartObjectID\` = \'1\'/")->once()->andReturn($testResult);

      /* This is the actual test */
      $this->object->read();
      /* And that's that. */


      mockery_verify();

      $this->assertEquals(3, count($this->object->vars));
      $this->assertEquals('value1', $this->object->column1);
      $this->assertEquals('value2', $this->object->column2);
    }

    /**
     * @todo Implement testInsertSuccess().
     */
    public function testInsertSuccess()
    {




      $this->object->db->shouldReceive('real_escape_string')->withAnyArgs()->zeroOrMoreTimes()->andReturnArgs();
      $this->object->db->shouldReceive('query')->withArgsMatching("/insert into \`testsmartobjects\` \(foo\) values \(\'bar\'\)/")->once()->andReturn(TRUE);
      $this->object->db->insert_id = 1;
      $this->object->db->errno = 0;
      $this->object->db->error = 'Success';

      $this->object->foo = 'bar';

      $retval = $this->object->insert();

      mockery_verify();

      $this->assertEquals(1, $retval);
      $this->assertEquals('Success', $this->object->error);

    }


    /**
     * @todo Implement testInsertFailure().
     */
    public function testInsertFailure()
    {


      $this->object->db->insert_id = NULL;
      $this->object->db->errno = 1062;
      $this->object->db->error = "Duplicate entry '1' for key 'idnum'";


      $this->object->db->shouldReceive('real_escape_string')->withAnyArgs()->zeroOrMoreTimes()->andReturnArgs();
      $this->object->db->shouldReceive('query')->withArgsMatching("/insert into \`testsmartobjects\` \(testSmartObjectID,foo\) values \('1', \'bar\'\)/")->once()->andReturn(TRUE);

      $this->object->id = 1;
      $this->object->foo = 'bar';


      $retval = $this->object->insert();

      mockery_verify();

      $this->assertEquals(FALSE, $retval);
      $this->assertEquals(1062, $this->object->errno);

    }



    /**
     * @todo Implement testUpdate().
     */
    public function testUpdate()
    {
        // Remove the following lines when you implement this test.
        $this->markTestIncomplete(
          'This test has not been implemented yet.'
        );
    }

    /**
     * @todo Implement testDelete().
     */
    public function testDelete()
    {
        // Remove the following lines when you implement this test.
        $this->markTestIncomplete(
          'This test has not been implemented yet.'
        );
    }

    /**
     * @todo Implement testLoad().
     */
    public function testLoad()
    {
        // Remove the following lines when you implement this test.
        $this->markTestIncomplete(
          'This test has not been implemented yet.'
        );
    }

    /**
     * @todo Implement testCreate().
     */
    public function testCreate()
    {
        // Remove the following lines when you implement this test.
        $this->markTestIncomplete(
          'This test has not been implemented yet.'
        );
    }

    /**
     * @todo Implement testSave().
     */
    public function testSave()
    {
        // Remove the following lines when you implement this test.
        $this->markTestIncomplete(
          'This test has not been implemented yet.'
        );
    }

    /**
     * @todo Implement testNuke().
     */
    public function testNuke()
    {
        // Remove the following lines when you implement this test.
        $this->markTestIncomplete(
          'This test has not been implemented yet.'
        );
    }
}


?>
