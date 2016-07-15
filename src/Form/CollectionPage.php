<?php
/**
 * @file
 * Contains \Drupal\searchblox\Form\CollectionPage.
 */

namespace Drupal\searchblox\Form;


use Drupal\Core\Form\FormBase;
use Drupal\Core\Form\FormStateInterface;
use Drupal\Component\Utility\Html;
use Symfony\Component\HttpFoundation\Request;
use Symfony\Component\HttpFoundation\RedirectResponse;

/**
 * Implements an example form.
 */
class CollectionPage extends FormBase {


  private $searchblox_apikey;
  private $searchblox_collection;
  private $searchblox_location;
  private $searchblox_portno;


  public function __construct() {
    $this->searchblox_apikey = \Drupal::state()->get('searchblox_apikey') ?: false; 
    $this->searchblox_collection = \Drupal::state()->get('searchblox_collection') ?: false;
    $this->searchblox_location = \Drupal::state()->get('searchblox_location') ?: false;
    $this->searchblox_portno = \Drupal::state()->get('searchblox_portno') ?: false;
  }

  /**
   * {@inheritdoc}
   */
  public function getFormId() {
    return 'searchblox_collection_page';
  }

  /**
   * {@inheritdoc}
   */
  public function buildForm(array $form, FormStateInterface $form_state) {
    $location = searchblox_location();
    $url      = $location . '/searchblox/servlet/SearchServlet?&query=""&xsl=json';
    $response = searchblox_curl_get_request($url);

    $collections = array();

    if ($response) {
      $collections = $response->searchform->collections;
    }

    if (!$location) {
      $form_state->setErrorByName('error', t('<p> SearchBlox settings not configured yet. </p>'));
    } elseif (empty($collections)) {
      $form_state->setErrorByName('error', t('<p> You have created no collections yet. </p>'));
    }
    $header = array(
      'id' => t('ID'),
      'collection_name' => t('Collection Name')
    );
    
    $options = $collec_names = $default_value = array();
    
    // Collections already selected by user
    $collec_names = \Drupal::state()->get('searchblox_search_collection'); 
    
    foreach ($collections as $collection) {
      $options[$collection->{'@name'}] = array(
        'id' => t(Html::escape($collection->{'@id'})),
        'collection_name' => t(Html::escape($collection->{'@name'}))
      );

      if ($collec_names) {
        // If the collection is the one already selected than mark it. 
        if (in_array($collection->{'@name'}, $collec_names)) {
          $default_value[$collection->{'@name'}] = true; // Should have same key as options array
        }
      }
    }
    $form['header'] = array(
      '#type' => 'item',
      '#markup' => '<div class="searchblox-logo" > 
       <h2 class="searchblox-h2">SearchBlox Search Module</h2> 
     </div>'
    );

    $form['page_details'] = array(
      '#type' => 'fieldset',
      '#title' => t('Mark the collections you want to show your search results from.'),
      '#description' => t('<p><strong>NOTE : If not marked, it will search from all of the available collections on your server.</strong></p>')
    );
    $form['searchblox_list_collection'] = array(
      '#type' => 'tableselect',
      '#header' => $header,
      '#options' => $options,
      '#empty' => t('No Collections found'),
      '#default_value' => $default_value
    );
    $form['actions']['#type'] = 'actions';
    $form['actions']['submit'] = array(
      '#type' => 'submit',
      '#value' => $this->t('Save'),
      '#button_type' => 'primary',
    );
    return $form;
  }

  /**
   * {@inheritdoc}
   */
  public function validateForm(array &$form, FormStateInterface $form_state) {
    // $token_generator = \Drupal::csrfToken();
    // $token = $token_generator->get();

    // // Check that the token is valid.
    // if ($token_generator->validate($token)) {
    //   $form_state->setErrorByName('error' , t('Validation error, please try again. If this error persists, please contact the site administrator.'));
    // }

    // if (strlen($form_state->getValue('phone_number')) < 3) {
    //   $form_state->setErrorByName('phone_number', $this->t('The phone number is too short. Please enter a full phone number.'));
    // }
  }

  /**
   * {@inheritdoc}
   */
  public function submitForm(array &$form, FormStateInterface $form_state) {
    
    if (isset($form)) {
      if (isset($form['#token'])) {
        $collections = $collection_name = $collection_ids = array();            
        // If collections selected

        // print_r($form['searchblox_list_collection']['#options']['cloudconvert']['id']->getUntranslatedString()); die;

        if (isset($form['searchblox_list_collection']['#value'])) {
            $collections = $form['searchblox_list_collection']['#value'];
            
            foreach ($collections as $collection) {
              $collection_name[] = Html::escape($collection);
              if ($collection) {
                $collection_ids[] = intval(Html::escape($form['searchblox_list_collection']['#options'][$collection]['id']->getUntranslatedString()));
              }
            }
          \Drupal::state()->set('searchblox_search_collection', $collection_name);
          \Drupal::state()->set('searchblox_collection_ids', $collection_ids);
            
        } else {
          \Drupal::state()->set('searchblox_search_collection', '');
          \Drupal::state()->set('searchblox_collection_ids', '');
        }
      }
    }
  }
}