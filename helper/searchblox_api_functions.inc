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
					<searchblox apikey="'.\Drupal::state()->get('searchblox_apikey').'">
					<document colname="'.\Drupal::state()->get('searchblox_collection').'">
					</document>
					</searchblox>
					' ; 
		$url = searchblox_location()  . '/searchblox/api/rest/clear';
		searchblox_curl_request( $url , $xml_input ) ; 
		global $statuscode ;

		\Drupal::state()->delete('searchblox_apikey');
		\Drupal::state()->delete('searchblox_collection');
		\Drupal::state()->delete('searchblox_location');
		\Drupal::state()->delete('searchblox_portno');
		\Drupal::state()->delete('searchblox_collection_ids');
		\Drupal::state()->delete('searchblox_indexed');
		\Drupal::state()->delete('searchblox_search_collection');
		return;
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
			return t('Invalid API.');
		
		elseif( empty( $statuscode ) ) : 
			$step_verify = false ;
			return t('Invalid Server Name OR Port #. <br/>  OR   <br/> Unable to communicate with the server.');
		
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

		$xml_input ='<?xml version="1.0" encoding="utf-8"?>
				<searchblox apikey="'. \Drupal::state()->get('searchblox_apikey').'">
				<document colname="'.$collection_name.'" />
				</searchblox>
				';

		$url = searchblox_location() . '/searchblox/api/rest/clear';
		
		searchblox_curl_request( $url , $xml_input ) ; 
		global $searchblox_statuscode;

		if($searchblox_statuscode == 500) {
			return 'Please Enter A Valid Collection Name.' ;  // Invalid Collection 
		} else {
			return true ; 
		}
	}

	function searchblox_xml_form( $node, $data ) {

		$options = array('absolute' => TRUE);
		$url = \Drupal\Core\Url::fromRoute('entity.node.canonical', 
			['node' => $data['nid']],$options);
		$nodeurl = $url->toString();
		$lm_date =  gmdate ( 'd F Y h:i:s T' , $data['created']) ;  
		$content =	"" ; 
		$data['tags'] = "";
		$data['categories'] = "";

		if(isset($data['content'])) : 
			$content = $data['content'];
		endif ; 
			
		return  $xml_input = '<?xml version="1.0" encoding="utf-8"?>
			<searchblox apikey="'.\Drupal::state()->get('searchblox_apikey').'">
			<document colname="'.\Drupal::state()->get('searchblox_collection').'" 
			location="'.$nodeurl.'">
			<url>'.$nodeurl.'</url>
			<lastmodified>' . $lm_date . '</lastmodified>
			<title boost="1">' . searchblox_sanitize( $data['title'] ) . '</title>
			<keywords boost="1">' . $data['tags'] . '</keywords>
			<content boost="1">' .  searchblox_sanitize( $data['title'] ) . " " . searchblox_sanitize( $content ) . '</content>
			<description boost="1">' . searchblox_description( $content ) . '</description>
			<size>'.(2*strlen( $content )).'</size>
			<alpha></alpha>
			<contenttype>HTML</contenttype>'.$data['categories'].'
			</document>
			</searchblox>
		';
	}
	
/**
 * Indexes The Node Returns Error If Any.
 *
 */	
	function searchblox_do_index($node, $nid) {
		try {
		    $data['nid'] = $nid;
		    $data['title'] = $node->get('title')->getValue()[0]["value"];
			$data['created'] = $node->get('created')->getValue()[0]["value"];
			$data['content']= "";

			if( $node->get('langcode')->getValue()[0]["value"] ) { 
				// Language choosen 
				$data['lang'] = $node->get('langcode')->getValue()[0]["value"]; 
			}
			//print_r($node->get('body')->getValue()); die;
			if(null !== ($node->get('body'))) {
				if($node->get('body')->getValue()) {
					$data['content'] = $node->get('body')->getValue()[0]["value"];
				}
			}

			if(null !== ($node->get('field_tags'))) {
				$field_tags = $node->get('field_tags')->getValue();

				if($field_tags) {
					$tags = "";
					foreach($field_tags as $tag) {
						$val = \Drupal::entityManager()->getStorage("taxonomy_term")->load( $tag['target_id'] );
						$tags .= $val->get('name')->getValue()[0]["value"] . ", ";
					}
					$data['tags'] = rtrim($tags , ", ");
				}
			}

			if(null !== ($node->get('field_category'))) {
				$field_cats = $node->get('field_category')->getValue();

				if($field_cats) {
					$categories = "";
					foreach($field_cats as $cat) {
						$val = \Drupal::entityManager()->getStorage("taxonomy_term")->load( $cat['target_id'] );
						$categories .= '<category>'.$val->get('name')->getValue()[0]["value"].'</category>';
					}

					$data['categories'] = $categories;
				}
			}
		} catch(Exception $e) {
			//
		}

		$xml_input = searchblox_xml_form($node, $data);
		$url = searchblox_location() .'/searchblox/api/rest/add';

		return $error_count = searchblox_curl_request( $url , $xml_input );
	}
	
	
/**
 * Implements custom hook , hook_nodepostinsert_index()
 * Invoked By searchblox_post_save()
 */
 
 
	function searchblox_node_postinsert_index( $node ) {
	   	$nid = $node->get('nid')->getValue()[0]["value"];

		if(! $node->getType() == 'article' || ! $node->getType() == 'page' ) 
		return ;
		
        searchblox_do_index( $node, $nid ) ; // Index the newly created node	
	}

	
/**
 * Implements custom hook , hook_nodepostupdate_index()
 * Invoked By searchblox_post_save()
 */
 
	function searchblox_node_postupdate_index( $node ) {
		$nid = $node->get('nid')->getValue()[0]["value"];

		if(! $node->getType() == 'article' || ! $node->getType() == 'page' ) 
		return ;
		
		searchblox_do_index( $node, $nid ) ; // Index the newly updated node

	}	

	
/**
 * Implements custom hook , hook_nodepostdelete_removeindex()
 * Invoked By searchblox_post_save()
 */
 
 
	function searchblox_node_postdelete_removeindex( $node ) {
		$nid = $node->get('nid')->getValue()[0]["value"];

		if(! $node->getType() == 'article' || ! $node->getType() == 'page' ) 
		return ;
	     
		// Deletes the node
        $options = array('absolute' => TRUE);
		$url = \Drupal\Core\Url::fromRoute('entity.node.canonical', 
			['node' => $nid],$options);
		$node_url = $url->toString();

		$xml_input ='
			<?xml version="1.0" encoding="utf-8"?>
			<searchblox apikey="'. \Drupal::state()->get('searchblox_apikey').'">
			<document colname="'.\Drupal::state()->get('searchblox_collection').'" uid="'.$node_url.'"/>
			</searchblox>
		';
				
		$url = searchblox_location() . '/searchblox/api/rest/delete';
		$error_count = searchblox_curl_request( $url , $xml_input );
	}