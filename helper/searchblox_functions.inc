<?php
/**
 * Searchblox Helper Functions
 *
 */


/**
 * Returns SearchBlox Articles and Pages from DB
 */

function searchblox_get_nodes_from_db($range)
{
    $query = \Drupal::database()->select('node', 'n');
    $query->fields('n', ['nid']);
    $query->condition(db_or()->condition('n.type', 'article' , '=')
        ->condition('n.type', 'page', '='));
    if($range) {
        $query->range( $range["start"], $range["end"]);    
    }

    return $results = $query->execute()->fetchAllAssoc('nid');
}

/**
 * Returns SearchBlox Location with Port #
 */

function searchblox_location()
{   
    if (\Drupal::state()->get('searchblox_location') &&  
        \Drupal::state()->get('searchblox_portno')):
        return \Drupal::state()->get('searchblox_location') . ':' . 
            \Drupal::state()->get('searchblox_portno');
    else:
        return false;
    endif;
}

/*
 * Strips tags and takes an excerpt from the content 
 * @value: Content to be excerpted
 * @length: length of the excerpt
 */

function searchblox_description($value, $length = 100)
{
    
    $value = searchblox_sanitize($value);
    if (is_array($value)) {
        list($string, $match_to) = $value;
    } else {
        $string = $value;
        if (isset($value[0])) {
            $match_to = $value{0};
        } else {
            $mathc_to = $value;
        }
    }
    
    $match_start   = @stristr($string, $match_to);
    $match_compute = strlen($string) - strlen($match_start);
    
    if (strlen($string) > $length) {
        if ($match_compute < ($length - strlen($match_to))) {
            $pre_string = substr($string, 0, $length);
            $pos_end    = strrpos($pre_string, " ");
            if ($pos_end === false)
                $string = $pre_string . "...";
            else
                $string = substr($pre_string, 0, $pos_end) . "...";
        } else if ($match_compute > (strlen($string) - ($length - strlen($match_to)))) {
            $pre_string = substr($string, (strlen($string) - ($length - strlen($match_to))));
            $pos_start  = strpos($pre_string, " ");
            $string     = "..." . substr($pre_string, $pos_start);
            if ($pos_start === false)
                $string = "..." . $pre_string;
            else
                $string = "..." . substr($pre_string, $pos_start);
        } else {
            $pre_string = substr($string, ($match_compute - round(($length / 3))), $length);
            $pos_start  = strpos($pre_string, " ");
            $pos_end    = strrpos($pre_string, " ");
            $string     = "..." . substr($pre_string, $pos_start, $pos_end) . "...";
            if ($pos_start === false && $pos_end === false)
                $string = "..." . $pre_string . "...";
            else
                $string = "..." . substr($pre_string, $pos_start, $pos_end) . "...";
        }
        
        $match_start   = stristr($string, $match_to);
        $match_compute = strlen($string) - strlen($match_start);
    }
    
    return $string;
}


/*
 * Sanitizes text 
 * 
 * 
 */
function searchblox_sanitize($text)
{
    
    if (trim($text) == '')
        return $text;
    
    $text = html_entity_decode($text, ENT_COMPAT, "UTF-8");
    $text = preg_replace('@<[\/\!]*?[^<>]*?>@si', ' ', $text);
    $text = str_replace(array(
        "&",
        "   ",
        "  "
    ), array(
        "and",
        " ",
        " "
    ), $text);
    return $text;
    
}

/*
 *
 *  Checks Collection selected in the form
 *  @see searchblox_collection_page.tpl 
 */
function searchblox_form_collection($collection)
{
    if (\Drupal::state()->get('searchblox_search_collection')):
        if (in_array($collection->{'@id'}, \Drupal::state()->get('searchblox_search_collection'))) {
            echo 'checked="checked"';
        }
    endif;
}


/*
 *
 * Checks If All settings Configured For Module
 */

function searchblox_settings_configured()
{
    
    $api_key    = \Drupal::state()->get('searchblox_apikey');
    $collection = \Drupal::state()->get('searchblox_collection');
    $location   = searchblox_location();
    
    if (!$api_key || !$collection || !$location) {
        return false;
    } else {
        return true;
    }
}

/**
* Functions For Post Action Triggers 
*
*/


/**
 * implements hook_entity_insert
 */
function searchblox_entity_insert(Drupal\Core\Entity\EntityInterface $entity)
{
    drupal_register_shutdown_function('_searchblox_post_save', $entity, 'insert');
}

/**
 * implements hook_entity_update
 */
function searchblox_entity_update(Drupal\Core\Entity\EntityInterface $entity)
{
    drupal_register_shutdown_function('_searchblox_post_save', $entity, 'update');
}

/**
 * implements hook_entity_delete
 */
function searchblox_entity_delete(Drupal\Core\Entity\EntityInterface $entity)
{
    drupal_register_shutdown_function('_searchblox_post_save', $entity, 'delete');
}


/**
 * @param $entity
 * @param $type
 * @param $option
 * Invkoes Node Post Action Hooks
 */

function _searchblox_post_save($entity, $option)
{
    
    // Don't Perform any action if settings not configured 
    if (!searchblox_settings_configured()):
        return;
    endif;
    
    // If Sync Not performed
    if (!\Drupal::state()->get('searchblox_indexed')):
        return;
    endif;
    
    $saved        = FALSE;
    $new_entity   = NULL;
    $index_status = NULL;
    $type = $entity->getEntityTypeId();
    $nid = $entity->get('nid')->getValue()[0]["value"];

    if ($option == 'insert' || $option == 'update') {
        $saved        = TRUE;
        $index_status = '_index';
    }
    
    if ($option == 'delete') {
        $new_entity = \Drupal::entityTypeManager()->getStorage($type)->load($nid);
        if (!$new_entity) {
            $saved        = TRUE;
            $index_status = '_removeindex';
        }
    }

    if ($saved) {        
        if ($type == 'node') {
            call_user_func('searchblox_node_post'. $option . $index_status, $entity);
            //Drupal\Core\Extension\ModuleHandler::invokeAll('node_post' . $option . $index_status, $entity);
        }
    }
}

/*
*   Gives The Collection Ids In SeacrhBlox Compatible Format
*
*/


function searchblox_get_col_ids()
{
    
    $collection_ids = array();
    $collection_ids = \Drupal::state()->get('searchblox_collection_ids');
    if (empty($collection_ids)) {
        return NULL;
    }
    
    $collec_col = 'col=';
    $count      = 0;
    foreach ($collection_ids as $collection_id) {
        $collection_id = intval($collection_id);
        if ($count == 0) {
            $collec_col .= $collection_id;
        } else {
            $collec_col .= '&col=' . $collection_id;
        }
        
        $count++;
    }
    
    return $collec_col;
}


/*
*   Gives The SeacrhBlox Full Plugin url
*
*/

function searchblox_full_plugin_url()
{
    $url =  \Drupal\Core\Url::fromRoute('searchblox.admin.config');
    return $nodeurl = \Drupal::request()->getHost() . $url->toString();
}

/**
 * Adds HTTP to url if not set 
 */
	

function searchblox_addhttp($url) 
{
	if (!preg_match("~^(?:f|ht)tps?://~i", $url)) {
		$url = "http://" . $url;
	}
	return $url;
}