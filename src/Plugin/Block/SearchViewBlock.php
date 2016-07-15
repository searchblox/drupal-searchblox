<?php
class SearchViewBlock extends BlockBase {

  /**
   * {@inheritdoc}
   */
  protected function blockAccess(AccountInterface $account) {
    //return (!$account->id() && !(arg(0) == 'user' && !is_numeric(arg(1))));
  }

  /**
   * {@inheritdoc}
   */
  public function build() {
    // Building of the form is omitted.
    return array(
      'user_login_form' => $form,
      'user_links' => array(
        '#theme' => 'item_list',
        '#items' => $items,
      ),
    );
  }
}