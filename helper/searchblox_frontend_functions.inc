<?php 

		/**
		*     FUNCTIONS FOR FRONTEND OF SEARCHBLOX
		*
		*/

	
/**
 * Implements hook_search_info().
 */ 

function searchblox_search_info()
{
    return array(
        'title' => 'Content',
        'path' => 'node',
        'conditions_callback' => 'searchblox_search_execute'
    );
    
}


/**
 * Implements hook_search_execute().
 */

function searchblox_search_execute($keys = NULL)
{
    
    // If Settings Not Configured Return.  
    if (!searchblox_settings_configured()):
        return array();
    endif;
    
    
    $query          = urlencode(check_plain($keys));
    $collection_col = searchblox_get_col_ids();
    $url            = searchblox_location() . '/searchblox/servlet/SearchServlet?&query=' . $query . '&xsl=json&' . $collection_col;
    $response       = searchblox_curl_get_request($url);
    
    $results = $nodes = array();
    if (isset($response->results->result)) {
        
        $nodes = $response->results->result;
        
        if (!is_array($nodes)) {
            $node     = $nodes;
            $nodes    = array();
            $nodes[0] = $node;
        }
        
        foreach ($nodes as $node):
            $nodeurl = $node->uid;
            $node_id = basename($nodeurl); //extracts node id from url
            
        // If It's a Drupal Node 
            if (node_load($node_id)) {
                
                $node    = node_load($node_id);
                $options = array(
                    'absolute' => TRUE
                );
                $nodeurl = url('node/' . $node->nid, $options);
                $build   = node_view($node, 'search_result');
                unset($build['#theme']);
                $node->rendered = drupal_render($build);
                
                // Fetch comments for snippet.
                $node->rendered .= ' ' . module_invoke('comment', 'node_update_index', $node);
                $extra     = module_invoke_all('node_search_result', $node);
                $results[] = array(
                    'link' => $nodeurl,
                    'type' => check_plain(node_type_get_name($node)),
                    'title' => $node->title,
                    'user' => theme('username', array(
                        'account' => $node
                    )),
                    'date' => $node->changed,
                    'node' => $node,
                    'extra' => $extra,
                    'snippet' => search_excerpt($keys, $node->rendered),
                    'language' => entity_language('node', $node)
                );
            } else {
                // If It's NOT a Drupal Node
                
                if ($node) {
                    
                    $results[] = array(
                        'link' => $node->uid,
                        'title' => $node->title,
                        'date' => $node->lastmodified,
                        'snippet' => ($node->context->text[0]) ? $node->context->text[0] : ''
                    );
                }
                
            }
        endforeach;
        
        
    }
    
    return $results;
    
}


/**
*   Implemenets hook_block_info() 
*
*/


function searchblox_block_info()
{
    // This example comes from node.module.
    $blocks['searchblox_search'] = array(
        'info' => t('SearchBlox Search'),
        'cache' => DRUPAL_CACHE_GLOBAL
    );
    
    return $blocks;
    
}


/**
*  Implemenets hook_block_view() 
*
*/


function searchblox_block_view($delta = '')
{
    // This example is adapted from node.module.
    $block = array();
    
    switch ($delta) {
        
        case 'searchblox_search':
            
            if (user_access('access content')) {
                
                if (path_is_admin(current_path())) {
                    return;
                }
                
                if (module_exists('overlay') && path_is_admin($_GET['q'])) {
                    return;
                }
                
                
                // Prepare Vars For Setting them in Drupal.Settings

                $search_url         = (searchblox_location()) ? searchblox_location() . '/searchblox/servlet/SearchServlet' : '';
                $auto_suggest_url   = (searchblox_location()) ? searchblox_location() . '/searchblox/servlet/AutoSuggest' : '';
                $report_servlet_url = (searchblox_location()) ? searchblox_location() . '/searchblox/servlet/ReportServlet' : '';
                $full_plugin_url    = searchblox_full_plugin_url();
                $collection_ids     = array();
                $collection_ids     = (variable_get('searchblox_collection_ids')) ? variable_get('searchblox_collection_ids') : '';
                
                // Setting Vars in Drupal.Settings
                drupal_add_js(array(
                    'searchblox' => array(
                        
                        'search_url' => $search_url,
                        'auto_suggest_url' => $auto_suggest_url,
                        'report_servlet_url' => $report_servlet_url,
                        'full_plugin_url' => $full_plugin_url,
                        'collection_ids' => $collection_ids
                    )
                ), array(
                    'type' => 'setting',
                    'group' => 0
                    
                ));
                
                //Adds Angular Js First
                drupal_add_js(drupal_get_path('module', 'searchblox') . '/includes/js/angular.js');
               
                
                $block['content'] = array(
                    '#markup' => _searchblox_search_block(),
                    '#attached' => array(
                        'css' => array(
                            drupal_get_path('module', 'searchblox') . '/includes/css/autosuggest-style.css',
                            drupal_get_path('module', 'searchblox') . '/includes/css/custom-style.css',
                            drupal_get_path('module', 'searchblox') . '/includes/bootstrap/css/bootstrap.css'
                        ),
                        'js' => array(
                            
                            drupal_get_path('module', 'searchblox') . '/includes/lib/angular-resource.min.js',
                            drupal_get_path('module', 'searchblox') . '/includes/lib/angular-sanitize.min.js',
                            drupal_get_path('module', 'searchblox') . '/includes/lib/moment.js',
                            drupal_get_path('module', 'searchblox') . '/includes/lib/json2extension.js',
                            drupal_get_path('module', 'searchblox') . '/includes/lib/ui-bootstrap-tpls-0.10.0.js',
                            
                            drupal_get_path('module', 'searchblox') . '/includes/js/controllers/searchblox-controller.js',
                            drupal_get_path('module', 'searchblox') . '/includes/js/directives/autocomplete-directive.js',
                            drupal_get_path('module', 'searchblox') . '/includes/js/directives/custom-input-directive.js',
                            drupal_get_path('module', 'searchblox') . '/includes/js/directives/media-template-directive.js',
                            drupal_get_path('module', 'searchblox') . '/includes/js/directives/ads-template-directive.js',
                            drupal_get_path('module', 'searchblox') . '/includes/js/factories/searchblox-factory.js',
                            drupal_get_path('module', 'searchblox') . '/includes/js/filters/searchblox-filter.js',
                            drupal_get_path('module', 'searchblox') . '/includes/js/modules/facet-module.js',
                            drupal_get_path('module', 'searchblox') . '/includes/js/modules/searchblox-module.js',
                            drupal_get_path('module', 'searchblox') . '/includes/js/services/searchblox-service.js',
                            drupal_get_path('module', 'searchblox') . '/includes/js/custom-lib/myscripts.js'
                        )
                    )
                );
            } else {
                $block['content'] = t('Not Allowed To View Content.');
            }
            
            break;
    }
    return $block;
}


/**
*   Gets Block HTML 
*
*/

function _searchblox_search_block() 
{
    
    return '
            <div ng-app="searchbloxModule" class="container">
                <div ng-controller="searchbloxController" ng-init="init()">
                    <form class="center-block" >
                        <custominput id="xyz" ng-model="query" onsearch="startSearch" inputstyle="inputClass">
                            <autocomplete source="loadItems" ng-show="showAutoSuggest"></autocomplete>
                        </custominput>
                    </form>
                    <div ng-include 
                    src="\'' . searchblox_full_plugin_url() . '/includes/views/search-results.html' . '\'' . '" >
                    </div>
                </div>
            </div>
                ';
    
}