/**
 * Created by cselvaraj on 4/29/14.
 */
  

  
// FACTORY
angular.module('searchbloxDPConfigFactory',[]).factory('searchbloxDPConfig', [function () {
   
   
   var a  = 0 ; 
   var searchblox_searchurl = 1 ;
  
Drupal.behaviors.searchblox = {

        attach: function ( context , settings ) { 
            searchblox_searchurl      =  settings.searchblox.search_url ; 
            searchblox_autosuggest    =  settings.searchblox.auto_suggest_url ; 
            searchblox_reportservlet  =  settings.searchblox.report_servlet_url ;
			 //alert(searchblox_reportservlet) ;
			
		}

		
}; 
   
	var searchFactory = new Object(),
		searchUrl = undefined,
		autoSuggestUrl = undefined,
		reportServletUrl = undefined;
	
	searchFactory.searchUrl = searchUrl
	searchFactory.autoSuggestUrl = autoSuggestUrl
	searchFactory.reportServletUrl = reportServletUrl
	
    return searchFactory;
}]);


/*
for (var x in searchblox) {
  console.log(x) ;
}
 //alert(searchblox_reportservlet) ;
*/

  
