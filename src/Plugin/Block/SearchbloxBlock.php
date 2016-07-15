<?php
/**
 * @file
 * Contains \Drupal\search\Plugin\Block\SearchbloxBlock.
 */

namespace Drupal\searchblox\Plugin\Block;

use Drupal\Core\Access\AccessResult;
use Drupal\Core\Session\AccountInterface;
use Drupal\Core\Block\BlockBase;
  
/**
 * Provides a 'Search form' block.
 *
 * @Block(
 *   id = "search_form_block",
 *   admin_label = @Translation("SearchBlox Search"),
 *   category = @Translation("Forms")
 * )
 */
class SearchbloxBlock extends BlockBase {

  /**
   * Set Access For Module
  */
  protected function blockAccess(AccountInterface $account) {
    return AccessResult::allowedIfHasPermission($account, 'search content');
  }

  /**
   * {@inheritdoc}
  */
  public function build() {
    $is_admin = \Drupal::service('router.admin_context')->isAdminRoute();
    if ($is_admin) {
      return [];
    }

    global $base_url;
    $output['#theme'] = "searchblox_frontend_view";
    $output['#var1'] = "vars_set_from_module_file";
    $output['#type'] = "markup";
    $output['#attributes'] = ['class' => ['searchblox-div']];

    // Prepare Vars For Setting them in Drupal.Settings
    $search_url         = (searchblox_location()) ? searchblox_location() . '/searchblox/servlet/SearchServlet' : '';
    $auto_suggest_url   = (searchblox_location()) ? searchblox_location() . '/searchblox/servlet/AutoSuggest' : '';
    $report_servlet_url = (searchblox_location()) ? searchblox_location() . '/searchblox/servlet/ReportServlet' : '';
    $full_plugin_url    = searchblox_full_plugin_url();
    $collection_ids     = array();
    $collection_ids     = (\Drupal::state()->get('searchblox_collection_ids')) ? \Drupal::state()->get('searchblox_collection_ids') : '';

    $output['#attached']['drupalSettings']
    ["searchblox"]["base_url"] = $base_url;

    $output['#attached']['drupalSettings']
    ["searchblox"]["module_url"] = $base_url . "/modules/searchblox";

    $output['#attached']['drupalSettings']
    ["searchblox"]["search_url"] = $search_url;

    $output['#attached']['drupalSettings']
    ["searchblox"]["auto_suggest_url"] = $auto_suggest_url;
    
    $output['#attached']['drupalSettings']
    ["searchblox"]["report_servlet_url"] = $report_servlet_url;
    
    $output['#attached']['drupalSettings']
    ["searchblox"]["full_plugin_url"] = $full_plugin_url;

    $output['#attached']['drupalSettings']
    ["searchblox"]["collection_ids"] = $collection_ids;

    // Attach Library
    $output['#attached']['library'][] = 'searchblox/searchblox-frontpage';

    $this->rendered_content = $output;
    return $output;
  }
}