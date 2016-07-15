<?php
/**
 * @file
 * Contains \Drupal\searchblox\Form\FirstStepForm.
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
class FirstStepForm extends FormBase {


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
    return 'searchblox_step1';
  }

  /**
   * {@inheritdoc}
   */
  public function buildForm(array $form, FormStateInterface $form_state) {

   $form['header'] = array(
        '#type' => 'item',
        '#markup' => '<div class="searchblox-logo" > 
           <h2 class="searchblox-h2">SearchBlox Search Module</h2> 
        </div>'
    );
     
    $form['page_details'] = array(
        '#type' => 'fieldset',
        '#title' => t('Authorize the SearchBlox Search Module'),
        '#description' => t('Thanks for installing the SearchBlox Search Module for Drupal. Please enter your SearchBlox configuration details to get started.')
    );
    
    $form['searchblox_api'] = array(
        '#type' => 'textfield',
        '#title' => 'SearchBlox Api Key',
        '#default_value' => Html::escape($this->searchblox_apikey),
        '#description' => 'Enter the API key of SearchBlox application.',
        '#required' => TRUE
    );
    
    $form['searchblox_location'] = array(
        '#type' => 'textfield',
        '#title' => 'SearchBlox Server Name',
        '#default_value' => Html::escape($this->searchblox_location),
        '#description' => 'Enter the location of SearchBlox server.',
        '#required' => TRUE
    );
    
    $form['searchblox_portno'] = array(
        '#type' => 'textfield',
        '#title' => 'SearchBlox Port #',
        '#default_value' => Html::escape($this->searchblox_portno),
        '#description' => 'Enter the Port # of SearchBlox server.',
        '#required' => TRUE
    );
    $form['actions']['#type'] = 'actions';
    $form['actions']['submit'] = array(
      '#type' => 'submit',
      '#value' => $this->t('Authorize'),
      '#button_type' => 'primary',
    );
    return $form;
  }

  public function validateForm(array &$form, FormStateInterface $form_state) {
    $api      = Html::escape($form['searchblox_api']['#value']);
    $location = rtrim(Html::escape($form['searchblox_location']['#value']), '/');
    $location = searchblox_addhttp( $location ) ; 
    $port_no  = intval(Html::escape($form['searchblox_portno']['#value']));

    try {
      $resp = searchblox_verify_api_loc($api, $location, $port_no);
    } catch(\Exception $e) {
      $resp = $e->getMessage();
      $form_state->setErrorByName('error', $this->t($resp));
    }
  }

  public function submitForm(array &$form, FormStateInterface $form_state) {
    if (isset($form)) {
      if (isset($form['#token'])) {
        $api      = Html::escape($form['searchblox_api']['#value']);
        $location = rtrim(Html::escape($form['searchblox_location']['#value']), '/');
        $location = searchblox_addhttp( $location ) ; 
        $port_no  = intval(Html::escape($form['searchblox_portno']['#value']));
            
        \Drupal::state()->set('searchblox_apikey', $api);
        \Drupal::state()->set('searchblox_location', $location);
        \Drupal::state()->set('searchblox_portno', $port_no);
        
        $response = new RedirectResponse(\Drupal::url('searchblox.step2'));
        $response->send();
        return;
      }
    }
  }
}