/**
 * Created by cselvaraj on 4/29/14.
 */
   
angular.module('facetModule',['ngResource'])
    .factory('facetFactory',function($resource){
	
		 var facet_file =  Drupal.settings.searchblox.full_plugin_url + '/includes/data/facet.json' ; 
		return $resource(  facet_file ,{}, {
            get: {method:'GET'}
        });
    });