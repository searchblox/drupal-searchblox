<?php
/**
 * @file
 * Contains \Drupal\searchblox\Controller\SearchbloxController.
 */
 
namespace Drupal\searchblox\Controller;

use Symfony\Component\HttpFoundation\Request;
use Symfony\Component\HttpFoundation\RedirectResponse;
use Drupal\Core\Controller\ControllerBase;
use Drupal\Component\Utility\Html;
use Drupal\node\Entity\Node;
use Drupal\Core\Entity\Query\QueryInterface;
use Drupal\Core\Url;
use Drupal\Core\File;

/**
 * An example controller.
 */
class SearchbloxController extends ControllerBase 
{
  	private $searchblox_apikey;
  	private $searchblox_collection;
  	private $searchblox_location;
  	private $searchblox_portno;


	public function __construct() {
		$this->searchblox_apikey = \Drupal::state()->get('searchblox_apikey') ?: false; 
		$this->searchblox_collection = \Drupal::state()->get('searchblox_apikey') ?: false;
		$this->searchblox_location = \Drupal::state()->get('searchblox_apikey') ?: false;
		$this->searchblox_portno = \Drupal::state()->get('searchblox_apikey') ?: false;
	}

	public function sbSettings() {
		if (!($this->searchblox_apikey) || !(searchblox_location())) {
        	$response = new RedirectResponse(\Drupal::url('searchblox.step1'));
		  	$response->send();
	    } elseif (!($this->searchblox_collection)) {
        	$response = new RedirectResponse(\Drupal::url('searchblox.step2'));
		  	$response->send();  
	    } else {
        	$response = new RedirectResponse(\Drupal::url('searchblox.step3'));
		  	$response->send();
	    }
	}

	public function indexStep() {
		// Return If Settings not configured
    
	    if (!(\Drupal::state()->get('searchblox_apikey')) || !(\Drupal::state()->get('searchblox_location'))) {
	        $response = new RedirectResponse(\Drupal::url('searchblox.step1'));
		  	$response->send();
	    } elseif (!(\Drupal::state()->get('searchblox_collection'))) {
 			$response = new RedirectResponse(\Drupal::url('searchblox.step2'));
		  	$response->send();
	    }
	    
	    if (isset($_POST['searchblox_re_configure'])) {
	        $response = new RedirectResponse(\Drupal::url('searchblox.admin.config'));
	        searchblox_clear_config();
	  		$response->send();
	    }
	    
	    $searchblox_full_page = array(
	        '#theme' => 'searchblox_full_page'
	    );

	    // Attach PHP vars to js script
	    $js_vars = searchblox_add_js_settings();

	    $searchblox_full_page['#attached']['drupalSettings']
	   	["searchblox"]["base_url"] = $js_vars["base_url"];

	   	$searchblox_full_page['#attached']['drupalSettings']
	   	["searchblox"]["total_docs"] = $js_vars["total_docs"];

	   	// Attach Library
    	$searchblox_full_page['#attached']['library'][] = 'searchblox/searchblox-deps';

	    return $searchblox_full_page;
	}

	public function indexDoc() {
		$offset = isset( $_GET['offset'] ) ? intval( $_GET['offset'] ) : 0;
		$batch_size = isset( $_GET['batch_size'] ) ? intval( $_GET['batch_size'] ) : 3 ;
		$resp = NULL;
		$num_written = 0 ;
		$query_limit =  $batch_size;
		
		$results = [];
		$range["start"] = $offset;
		$range["end"] = $query_limit;
		$results = searchblox_get_nodes_from_db($range);		
        $total_posts = count($results);

		if( $results ) {
			try {
				foreach ( $results as $result ) {
					//print_r($result); die;
                    $node = Node::load($result->nid);
					$error_count = searchblox_do_index($node, $result->nid) ;  // Indexes the node returns error if any

					// Connection refused by server
					if($error_count == -111) {
                        throw new \Exception("SearchBlox was unable to connect to the server provided during configuration.");
                    }
					global $statuscode ; 
					
					// Invalid Document Location
					if($statuscode == 502) {  
						throw new \Exception("SearchBlox could not find the requested documents on this location.");
					}	
					
					if($error_count == 0) { 
						$num_written++;
					} else {
						throw new \Exception("Some Documents could not be indexed. Please Try again !");
						return; 
					}
					$total_posts++;
				}
			} catch ( \Exception $e ) { 
				
				$error = array(
					'error' => $e->getMessage()
				) ; 
			}
		} else {
			$num_written = 0;
		}
	 
	    if( ! isset( $error )  && ( $error_count == 0 ) ) {
		   //If indexing successful , then set variable

            if( ! \Drupal::state()->get('searchblox_indexed') ) {
		       \Drupal::state()->set('searchblox_indexed' , 1 ) ; // TO KNOW THAT INDEXING AHS BEEN DONE
		    }		   
		   
		}
		
		
		header( 'Content-Type: application/json' );
		if( ! isset( $error ) ) {
			
		$response = array( 'num_written' => $num_written, 'total' => $total_posts ); 
		print( json_encode( $response ) );
			
		} else {
			
			print( json_encode( $error ) ); 
		}
      	 
        die() ; // end ajax response , no output now except one hooked with register_shutdown
	}
}
