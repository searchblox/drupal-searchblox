<?php 

/**
*     FUNCTIONS FOR FRONTEND OF SEARCHBLOX
*
*/

/**
 * Implements hook_search_execute().
 */

function searchblox_search_execute($keys = NULL)
{
    
    // If Settings Not Configured Return.  
    if (!searchblox_settings_configured()):
        return array();
    endif;
    
    
    $query          = urlencode(Html::escape($keys));
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
                    'type' => Html::escape(node_type_get_name($node)),
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
 * Returns SearchBlox Index Page Table
 */

function searchblox_get_index_page_table() {

    $header = array(
        t('SearchBlox Module Settings'),
        array(
          'data' => '',
          'colspan' => 2
    ));

    $rows = array(
        array(
            'data' => array(
                'API Key:',
                \Drupal::state()->get('searchblox_apikey')
            )
        ),
        array(
            'data' => array(
                'Search Collection Name :',
                \Drupal::state()->get('searchblox_collection')
            )
        ),
        array(
            'data' => array(
                'Number of Searchable Posts:',
                ''
            ),
            'id' => array('index_doc_div')    
        )
    );

    return $page_table['table'] = array(
      '#theme' => 'table',
      '#header' => $header,
      '#rows' => $rows,
    );
}
