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
	//die("Asas");
	$error_count = 0 ;
	$options['headers'] = ['Content-Type' => 'text/xml'] ;
	$options['method'] = 'POST';
	$options['data'] = trim( $xml_input );
	$options['max_redirects'] = 5 ;
	$options['timeout'] = 111300 ;
	$options['http_errors'] = false ;

	$client = new GuzzleHttp\Client([
    	'timeout'  => 111300
	]);

	try {
	    $response = $client->request('POST', $url, [
		    'body' => $options['data'],
		    'http_errors' => false
		]);
	} catch (RequestException $e) {
	    if ($e->hasResponse()) {
	        return Psr7\str($e->getResponse());
	    }
	}

	$body = $response->getBody()->getContents();
	$xml = simplexml_load_string($body);

	global $searchblox_statuscode;
	$searchblox_statuscode = $xml->statuscode;

	if (intval($searchblox_statuscode)>=100 AND intval($searchblox_statuscode)<=701) {

		if ( intval($searchblox_statuscode) == 100 ) {
			$error_count = 0; // After first succesful connection, lets reset error counter 
		}
		else { 
			$error_count++; // Skip if document cannot be indexed

		}

	} elseif ( $searchblox_statuscode != '' ) {

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

	$options['headers'] =['Content-Type' => 'text/xml'] ;
	$options['method'] = 'GET';
	$options['max_redirects'] = 5 ;
	$options['timeout'] = 30 ;
	$options['http_errors'] = false ;

	$client = new GuzzleHttp\Client([
    	'timeout'  => 30
	]);

	try {
	    $response = $client->request('GET', $url , $options['headers']);
	} catch (RequestException $e) {
	    if ($e->hasResponse()) {
	        return Psr7\str($e->getResponse());
	    }
	}

	if ( !$response || $response->getStatusCode() != 200 ) {
		return NULL ; // "Something wrong with server configuration"
	} 

	$results = $response->getBody()->getContents(); 

	return $response = json_decode($results) ; 

}