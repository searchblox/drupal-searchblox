<?php

/*
 description : Core curl function for SearchBlox plugin
 Version: 0.1
 Author: SearchBlox 
 Author URI: http://searchblox.com
*/


	/*
	*  POST cURL request to Searchblox
	*
	*/
	
function searchblox_curl_request( $url , $xml_input = ''  ) 
{

	$error_count = 0 ;
	$options['headers']['Content-Type'] = 'text/xml';
	$options['method'] = 'POST';
	$options['data'] = trim( $xml_input );
	$options['max_redirects'] = 5 ;
	$options['timeout'] = 10 ;

	$response = drupal_http_request( $url, $options );

	if ( !$response || $response->code != 200 ||  ! empty( $response->error ) ) {
		$error_count++ ; 
		return $error_count ; // "Something wrong with server configuration"
	}  
	$body = $response->data;
	$xml = simplexml_load_string($body);

	global $statuscode; 
	$statuscode = (string) $xml->statuscode ;

	if (intval($statuscode)>=100 AND intval($statuscode)<=701) {

		if ( intval($statuscode) == 100 ) {
			$error_count = 0; // After first succesful connection, lets reset error counter 
		}
		else { 
			$error_count++; // Skip if document cannot be indexed

		}

	} elseif ( $statuscode != '' ) {

		$error_count++; // Skip if document cannot be indexed
	}

	return $error_count; 
}


	/*
	*  GET cURL request to Searchblox
	*
	*/


function searchblox_curl_get_request( $url ) 
{

	$options['headers']['Content-Type'] = 'text/xml';
	$options['method'] = 'GET';
	$options['max_redirects'] = 5 ;
	$options['timeout'] = 10 ;

	$response = drupal_http_request( $url, $options );

	if ( !$response || $response->code != 200 ||  ! empty( $response->error ) ) {
		return NULL ; // "Something wrong with server configuration"
	} 

	$results = $response->data ; 

	return $response = json_decode( $results ) ; 

}