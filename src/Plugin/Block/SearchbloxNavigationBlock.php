 <?php
 function blockForm($form, &$form_state) {
    $form['book_block_mode'] = array(
      '#default_value' => $this->configuration['block_mode'],
// rest of the form omitted for brevity.
    );
    return $form;
  }
  public function blockSubmit($form, &$form_state) {
    $this->configuration['block_mode'] = $form_state['values']['searchblox_block_mode'];
  }