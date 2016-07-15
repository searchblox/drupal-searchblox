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
class SecondStepForm extends FormBase {


  private $searchblox_apikey;
  private $searchblox_collection;
  private $searchblox_location;
  private $searchblox_portno;


  public function __construct() {
    $this->searchblox_collection = \Drupal::state()->get('searchblox_collection') ?: false;
  }

  /**
   * {@inheritdoc}
   */
  public function getFormId() {
    return 'searchblox_step2';
  }

  public function buildForm(array $form, FormStateInterface $form_state) {

    $form['header'] = array(
        '#type' => 'item',
        '#markup' => '<div class="searchblox-logo" > 
          <h2 class="searchblox-h2">SearchBlox Search Module</h2> 
        </div>'
    );
    
    $form['page_details'] = array(
        '#type' => 'fieldset',
        '#title' => t('Configure your Search Collection'),
        '#description' => t('Please enter the name of search collection for this module:')
    );
    
    $form['searchblox_collection'] = array(
        '#type' => 'textfield',
        '#title' => 'Collection Name',
        '#default_value' => Html::escape($this->searchblox_collection),
        '#description' => 'Search Collection Name .',
        '#required' => TRUE
    );

    $form['actions']['#type'] = 'actions';
    $form['actions']['submit'] = array(
      '#type' => 'submit',
      '#value' => $this->t('Save'),
      '#button_type' => 'primary',
    );
    return $form;
  }

  public function validateForm(array &$form, FormStateInterface $form_state) {
    $collection_name = Html::escape($form['searchblox_collection']['#value']);
    try {
      $resp  = searchblox_check_collection($collection_name); // check weather 
    } catch(\Exception $e) {
      $resp = $e->getMessage();
      $form_state->setErrorByName('error', $this->t($resp));
    }

    if($resp !== true) {
      $form_state->setErrorByName('error', $this->t($resp));
    }
  }

  public function submitForm(array &$form, FormStateInterface $form_state) {
    if (isset($form)) {
      if (isset($form['#token'])) {
        $collection_name = Html::escape($form['searchblox_collection']['#value']);        
        \Drupal::state()->set('searchblox_collection', $collection_name);
        $response = new RedirectResponse(\Drupal::url('searchblox.step3'));
        $response->send();
        return;
      }
    }
  }
}