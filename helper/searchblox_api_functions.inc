<?php 

/**
 * Searchblox API Functions
 *
 */



/**
 * Searchblox Clear Configuration
 *
 */

 
	function searchblox_clear_config() {
	
		$xml_input = '
					<?xml version="1.0" encoding="utf-8"?>
					<searchblox apikey="'.variable_get('searchblox_apikey').'">
					<document colname="'.variable_get('searchblox_collection').'">
					</document>
					</searchblox>
					' ; 
		$url = searchblox_location()  . '/searchblox/api/rest/clear';
		searchblox_curl_request( $url , $xml_input ) ; 
		global $statuscode ;
		
	      // $statuscode == 400  Collection is cleared
			variable_del('searchblox_apikey') ; 
			variable_del('searchblox_collection') ; 
			variable_del('searchblox_location') ; 
			variable_del('searchblox_portno') ;
			variable_del('searchblox_collection_ids') ;
			variable_del('searchblox_indexed') ;
	        variable_del('searchblox_search_collection') ;
		
		drupal_goto( 'admin/config/search/searchblox' ) ; 
	}

 
 
 
 
 /**
 * Description : Validates the Form API Key and Location by getting status of a dummy doc
 *
 * @see searchblox_api_page_validade()
 */

 
    function searchblox_verify_api_loc ( $api_key  , $location , $port_no ) {
			 
		$url = $location . ':' . $port_no . '/searchblox/api/rest/status';  // Check the status of a dummy document to verify api key and location. 
		$collection_name = "test" ; 

		$xml_input = '<?xml version="1.0" encoding="utf-8"?>
		<searchblox apikey="'.$api_key .'">
		<document colname="'.$collection_name.'" uid="http://www.searchblox.com/">
		</document>
		</searchblox>' ; 


		searchblox_curl_request( $url , $xml_input ) ; 
		
		global $statuscode; 
		$step_verify = true ;
		
		
		if( $statuscode == 601 ) : 
			$step_verify = false ;		
			form_set_error('searchblox_api', t('Invalid API.') ); 
		
		elseif( empty( $statuscode ) ) : 
			 $step_verify = false ;
			  form_set_error('searchblox_location', 
		  t('Invalid Server Name OR Port #. <br/>  OR   <br/> Unable to communicate with the server.') );
		
		else : 
			$step_verify = true ; 
		
		endif;
		
		return $step_verify ;  
	} 

/*
 *  Clear The Collection First To verify the The collection name
 * 
 *  @see searchblox_collection_page_validate()
 */
 
 
	function searchblox_check_collection ( $collection_name = '' ) { 

		$xml_input ='
				<?xml version="1.0" encoding="utf-8"?>
				<searchblox apikey="'. variable_get('searchblox_apikey').'">
				<document colname="'.$collection_name.'" />
				</searchblox>
				';

		$url = searchblox_location() . '/searchblox/api/rest/clear';
		searchblox_curl_request( $url , $xml_input ) ; 
		global $statuscode;
		 
		if($statuscode == 500) {
		    
			form_set_error ( 'searchblox_collection','Please Enter A Valid Collection Name.'); // Set error 
			return false ;  // Invalid Collection 
		} else {
			return true ; 
		}
	}

	function searchblox_xml_form( $node  , $category = '' , $node_tags = ''  ) {
	    
		if( $node->language ) { 
			$lang = $node->language ;  // Language choosen 
		}
		
		$options = array('absolute' => TRUE);
        $nodeurl = url('node/' . $node->nid , $options) ;
		$lm_date =  gmdate ( 'd F Y h:i:s T' , $node->changed ) ;  
		$content =	"" ; 
		if(isset ( $node->body[$lang][0]['value'] ) ) : 
			$content = $node->body[$lang][0]['value'];	
		endif ; 
			
		return  $xml_input = '<?xml version="1.0" encoding="utf-8"?>
				<searchblox apikey="'.variable_get('searchblox_apikey').'">
				<document colname="'.variable_get('searchblox_collection').'" 
				location="'.$nodeurl.'">
				<url>'.$nodeurl.'</url>
				<lastmodified>' . $lm_date . '</lastmodified>
				<title boost="1">' . searchblox_sanitize( $node->title ) . '</title>
				<keywords boost="1">' . $node_tags . '</keywords>
				<content boost="1">' .  searchblox_sanitize( $node->title ) . " " . searchblox_sanitize( $content ) . '</content>
				<description boost="1">' . searchblox_description( $content ) . '</description>
				<size>'.(2*strlen( $content )).'</size>
				<alpha></alpha>
				<contenttype>HTML</contenttype>'.$category.'
				</document>
				</searchblox>
				';
	}	

	
/**
 * AJAX Function which indexes Docs to SearchBlox.
 *
 */
    function searchblox_index_doc() {
        
        if( ! $_GET['_ajax_token'] ==  $_SESSION['searchblox']['token'] ) :
	       return ; 
	    endif ;
        
		$offset = isset( $_GET['offset'] ) ? intval( $_GET['offset'] ) : 0;
		$batch_size = isset( $_GET['batch_size'] ) ? intval( $_GET['batch_size'] ) : 3 ;
		$resp = NULL;
		$num_written = 0 ;
		$query_limit =  $batch_size ; 
		
		$results = array() ;
		$query = db_select('node','n')->fields('n', array ('nid') );
        $db_or = db_or();
		$db_or->condition('n.type', 'page' , '=');
		$db_or->condition('n.type', 'article' , '=');
		$query->condition($db_or);
		$query->range( $offset ,  $query_limit );
		$results = $query->execute();
         
        
		$total_posts = $query->execute()->rowCount();
	   
		if( $total_posts > 0 ) {
			try {
				foreach ( $results as $result ) { 

                    $node = node_load( $result->nid );  				
  
					$error_count = searchblox_do_index( $node ) ;  // Indexes the node returns error if any
					
					global $statuscode ; 
		
					if( $statuscode == 502 )   // Invalid Document Location
					{
						throw new Exception("SearchBlox could not find the requested documents on this location.");
					}	
					
					if ( $error_count == 0 ) { 
						$num_written++;
					} else  {
					
						 throw new Exception("Some Documents could not be indexed. Please Try again !");
						 return ; 
					}
				}
				
			} catch ( Exception $e ) { 
				
				$error = array(
					'error' => $e->getMessage()
					) ; 
			}
			
		} else {
			$num_written = 0;
		}
	 
	    if( ! isset( $error )  && ( $error_count == 0 ) ) {
		   //If indexing successful , then set variable

            if( ! variable_get('searchblox_indexed') ) {
		       variable_set('searchblox_indexed' , 1 ) ; // TO KNOW THAT INDEXING AHS BEEN DONE
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
	
/**
 * Indexes The Node Returns Error If Any.
 *
 */	
	function searchblox_do_index( $node ) {
	    
		if( $node->language ) { 
			$lang = $node->language ;  // Language choosen 
		}
		$categories = "" ; 
		$tags = "" ; 
		
		//If tags are set for the node
		if( isset ( $node->field_tags[$lang][0]['tid'] ) ) {
		   
			$node_tags = $node->field_tags[$lang] ; 
			
			foreach( $node_tags as $node_tag ) {
			   $term = taxonomy_term_load( $node_tag['tid'] );
			   $tags .= $term->name. ', ' ;  
			 
			}
			$tags = rtrim( $tags , ', ' ) ; 
		}
		
		// If categories Set then collect Categories
		
		if( isset( $node->field_categories[$lang][0]['tid'] ) ) {
		
			$node_categories = $node->field_categories[$lang] ; 
			foreach( $node_categories as $node_category ) {
			   $term = taxonomy_term_load( $node_category['tid'] );
			   $categories .= '<category>' . $term->name . '</category>' ;  
			 
			}
		}
		
		$xml_input = searchblox_xml_form( $node , $categories , $tags ) ; 
       
		$url = searchblox_location() .'/searchblox/api/rest/add';
       
		return  $error_count = searchblox_curl_request( $url , $xml_input ) ;   		
	     
	}
	
	
/**
 * Implements custom hook , hook_nodepostinsert_index()
 * Invoked By searchblox_post_save()
 */
 
 
	function searchblox_node_postinsert_index( $node ) {
	   
		if(! $node->type == 'article' || ! $node->type == 'page' ) 
		return ;
		
        searchblox_do_index( $node ) ; // Index the newly created node
		
	}

	
/**
 * Implements custom hook , hook_nodepostupdate_index()
 * Invoked By searchblox_post_save()
 */
 
 
	function searchblox_node_postupdate_index( $node ) {

		if(! $node->type == 'article' || ! $node->type == 'page' ) 
		return ;
		
		searchblox_do_index( $node ) ; // Index the newly updated node

	}	

	
/**
 * Implements custom hook , hook_nodepostdelete_removeindex()
 * Invoked By searchblox_post_save()
 */
 
 
	function searchblox_node_postdelete_removeindex( $node ) {

		if(! $node->type == 'article' || ! $node->type == 'page' ) 
		return ;
	     
		// Deletes the node  
		$options = array('absolute' => TRUE);
        $node_url = url('node/' . $node->nid , $options) ;
		$xml_input ='
				<?xml version="1.0" encoding="utf-8"?>
				<searchblox apikey="'. variable_get('searchblox_apikey').'">
				<document colname="'.variable_get('searchblox_collection').'" uid="'.$node_url.'"/>
				</searchblox>
				';
				
		$url = searchblox_location() . '/searchblox/api/rest/delete';
		
		$error_count = searchblox_curl_request( $url , $xml_input ); 

	}
